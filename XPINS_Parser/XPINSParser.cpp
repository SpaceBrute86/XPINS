//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>
#include "XPINSCompiler.h"

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;


void* runScript(XPINSScriptSpace& script);
enum exitReason {
	ENDOFBLOCK,
	LOOPBREAK,
	SCRIPTRETURN,
};
exitReason ParseCode(XPINSScriptSpace& script, vector<Instruction> instructions);
int dumpCount=0;
void* ParseSubScripts(XPINSScriptSpace& script, void* inputVal,DataType& type, vector<Argument>subscripts, int startIndex=0,bool ignoreLastArrayIndex=false);					//Any type
void* ParseVariableArg(XPINSVarSpace* vars,DataType type,int argNum);
void* ParseConstArg(DataType type,int modNum,int argNum,void*literalValue,XPINSArray arguments);
void* ParseExpArg(DataType type,opCode operation, bool assign,XPINSArray arguments);
void* ParseBIFArg(DataType type,int number,XPINSArray arguments);

size_t garbageCapcity=0x100000/sizeof(double); //Default is about 1 MB
vector<XPINSVarSpace*>allVarSpaces;

#pragma mark Variable Space, Script Space, and Array Management

void SetGarbageCapcaity(size_t capacity)
{
	garbageCapcity=capacity;
}

XPINSVarSpace::~XPINSVarSpace(){
	delete[] bVars;
	delete[] nVars;
	delete[] vVars;
	delete[] qVars;
	delete[] mVars;
	delete[] pVars;
	delete[] fVars;
	delete[] sVars;
	delete[] oVars;
	delete[] aVars;
}
XPINSScriptSpace::XPINSScriptSpace(string script,vector<XPINSBindings*> bind)
{
	instructions=XPINSCompiler::compileScript(script);
	bindings=bind;
}
XPINSScriptSpace::XPINSScriptSpace(string cluster,string name,vector<XPINSBindings*> bind)
{
	clusterURL=cluster;
	ifstream inFile;
	inFile.open(clusterURL+name+".XPINSX");
	string script="";
	char ch;
	while (inFile.get(ch))script+=ch;
	inFile.close();
	instructions=XPINSCompiler::compileScript(script);
	bindings=bind;
}

#pragma mark Argument Parsing
XPINSArray parseFunctionArgs(XPINSParser::XPINSScriptSpace &script,vector<Argument> args){
	XPINSArray values=XPINSArray();
	values.resize(args.size());
	for (int i=0; i<args.size(); ++i) {
		DataType type=VOID;
		void* val=ParseArg(script, args[i], type);
		values.setItemAtIndex(val, type, i);
	}
	return values;
}
Argument subscriptValueForNum(int num)
{
	Argument subscript=Argument();
	subscript.dataType=NUMBER;
	subscript.type=CONST;
	subscript.literalValue=new double(num);
	return subscript;
}
void* XPINSParser::ParseRawArg(XPINSScriptSpace& script, Argument arg,  DataType type){ return ParseArg(script, arg, type); }
void* ParseRawSubScript(XPINSScriptSpace& script, void* inputVal, DataType type, vector<Argument>subscripts){
	return ParseSubScripts(script, inputVal, type, subscripts);
}

void*  XPINSParser::ParseArg(XPINSScriptSpace& script, Argument arg,DataType& type,bool ignoreLastArrayIndex)
{
	void* baseValue=NULL;
	if(type==VOID)type=arg.dataType;
	XPINSArray funcArgs=parseFunctionArgs(script, arg.arguments);
	if (arg.isElemental) {
		type=funcArgs.typeAtIndex(0);
		void* input=NULL;
		void* output=NULL;
		size_t length=3;
		switch (type) {
			case VECTOR:
				input=funcArgs.vecAtIndex(0);
				output=new Vector();
				break;
			case QUATERNION:
				input=funcArgs.quatAtIndex(0);
				output=new Quaternion();
				length=4;
				break;
			case MATRIX:
				input=funcArgs.matAtIndex(0);
				break;
			case FIELD:
				input=funcArgs.fieldAtIndex(0);
				output=new VectorField();
				break;
			case ARRAY:
				input=funcArgs.arrAtIndex(0);
				length=((XPINSArray*)input)->size();
				output=new XPINSArray();
				((XPINSArray*)output)->resize(length);
				break;
		}
		if (type==ARRAY) {
			for (int i=0; i<length; ++i)
			{
				DataType t=((XPINSArray*)input)->typeAtIndex(i);
				vector<Argument> subscript=vector<Argument>(1,subscriptValueForNum(i));
				void* val=ParseRawSubScript(script, input, type, subscript);
				funcArgs.setItemAtIndex(val, t, 0);
				switch (arg.type)
				{
					case BIF:	val = ParseBIFArg(arg.dataType,arg.number, funcArgs);break;
					case FUNC:	val = script.bindings[arg.modNumber]->BindFunction(arg.number,funcArgs);break;
				}
				((XPINSArray*)output)->setItemAtIndex(val, arg.dataType, i);
			}
		} else if (type==MATRIX) {
			Matrix* m=(Matrix*)input;
			size_t rows=m->rows;
			size_t cols=m->cols;
			output=new Matrix(rows, cols);
			for (int r=0; r<rows; ++r)
			{
				for (int c=0; c<cols; ++c) {
					vector<Argument> subscript=vector<Argument>(2);
					subscript[0]=subscriptValueForNum(r);
					subscript[1]=subscriptValueForNum(c);
					void* val=ParseRawSubScript(script, input, MATRIX, subscript);
					funcArgs.setItemAtIndex(val, NUMBER, 0);
					switch (arg.type)
					{
						case BIF:	val = ParseBIFArg(arg.dataType,arg.number, funcArgs);break;
						case FUNC:	val = script.bindings[arg.modNumber]->BindFunction(arg.number,funcArgs);break;
					}
					*(double*)ParseRawSubScript(script, output, MATRIX, subscript)=*(double*)val;
				}
			}
		} else {
			for (int i=0; i<length; ++i)
			{
				XPINSArray array=*(XPINSArray*)output;
				vector<Argument> subscript=vector<Argument>(1,subscriptValueForNum(i));
				void* rightArg=ParseRawSubScript(script, input, type, subscript);
				DataType t = type==FIELD ? POLYNOMIAL : NUMBER;
				funcArgs.setItemAtIndex(rightArg, t, 0);
				switch (arg.type)
				{
					case BIF:	rightArg = ParseBIFArg(arg.dataType,arg.number, funcArgs);break;
					case FUNC:	rightArg = script.bindings[arg.modNumber]->BindFunction(arg.number,funcArgs);break;
				}
				void*leftArg=ParseRawSubScript(script, output, type, subscript);
				switch (t) {
					case BOOLEAN:	*(bool*)leftArg=*(bool*)rightArg; break;
					case NUMBER:	*(double*)leftArg=*(double*)rightArg;break;
					case VECTOR:	*(Vector*)leftArg=*(Vector*)rightArg;break;
					case QUATERNION:*(Quaternion*)leftArg=*(Quaternion*)rightArg;break;
					case MATRIX:	*(Matrix*)leftArg=*(Matrix*)rightArg;break;
					case POLYNOMIAL:*(Polynomial*)leftArg=*(Polynomial*)rightArg;break;
					case FIELD:		*(VectorField*)leftArg=*(VectorField*)rightArg;break;
					case STRING:	*(string*)leftArg=*(string*)rightArg;break;
					case OBJECT:	*(void**)leftArg=*(void**)rightArg;break;
					case ARRAY:		*(XPINSArray*)leftArg=*(XPINSArray*)rightArg;break;
				}
			}
		}
		baseValue=output;
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(baseValue, type, script.data->Trash.size()-1);
	}
	else if(type==arg.dataType)
	{
		switch (arg.type)
		{
			case VAR:	baseValue = ParseVariableArg(script.data,type,arg.number);break;
			case CONST:	baseValue = ParseConstArg(type, arg.modNumber,arg.number, arg.literalValue, funcArgs);break;
			case EXP:	baseValue = ParseExpArg(type,(opCode)arg.modNumber,arg.number!=0, funcArgs);break;
			case BIF:	baseValue = ParseBIFArg(type,arg.number, funcArgs);break;
			case FUNC:	baseValue = script.bindings[arg.modNumber]->BindFunction(arg.number,funcArgs);break;
		}
		if(arg.type!=VAR)
		{
			script.data->Trash.resize(script.data->Trash.size()+1);
			script.data->Trash.setItemAtIndex(baseValue, type, script.data->Trash.size()-1);
		}
	}
	return ParseSubScripts(script, baseValue, type, arg.subscripts,ignoreLastArrayIndex);
}
void* ParseSubScripts(XPINSScriptSpace& script, void* inputVal, DataType& type, vector<Argument>subscripts, int startIndex,bool ignoreLastArrayIndex)
{
	if (subscripts.size()<=startIndex||(ignoreLastArrayIndex&&type==ARRAY&&subscripts.size()<=startIndex+1))  return inputVal;
	switch (type) {
		case BOOLEAN:
		case NUMBER:
		case POLYNOMIAL:
		case STRING:
			return inputVal;
		case VECTOR:
			type=NUMBER;
			switch ((int)*(double*)ParseRawArg(script,subscripts[startIndex],NUMBER)) {
				case 0: return &((Vector*)inputVal)->x;
				case 1: return &((Vector*)inputVal)->y;
				case 2: return &((Vector*)inputVal)->z;
				default:
					type=VECTOR;
					return inputVal;
			}
		case QUATERNION:
			type=NUMBER;
			switch ((int)*(double*)ParseRawArg(script,subscripts[startIndex],NUMBER)) {
				case 0: return &((Quaternion*)inputVal)->r;
				case 1: return &((Quaternion*)inputVal)->v.x;
				case 2: return &((Quaternion*)inputVal)->v.y;
				case 3: return &((Quaternion*)inputVal)->v.z;
				default:
					type=QUATERNION;
					return inputVal;
			}
		case MATRIX: {
			type=NUMBER;
			int rows=(int)*(double*)ParseRawArg(script,subscripts[startIndex],NUMBER);
			int cols=(int)*(double*)ParseRawArg(script,subscripts[startIndex+1],NUMBER);
			return ((Matrix*)inputVal)->values+((Matrix*)inputVal)->cols*rows+cols;
		}
		case FIELD:
			type=POLYNOMIAL;
			switch ((int)*(double*)ParseRawArg(script,subscripts[startIndex],NUMBER)) {
				case 0: return &((VectorField*)inputVal)->P;
				case 1: return &((VectorField*)inputVal)->Q;
				case 2: return &((VectorField*)inputVal)->R;
				default:
					type=FIELD;
					return inputVal;
			}
		case OBJECT:return inputVal;
		case ARRAY:{
			int index=(int)*(double*)ParseRawArg(script,subscripts[startIndex],NUMBER);
			type=((XPINSArray*)inputVal)->typeAtIndex(index);
			void*val=NULL;
			switch (type)
			{
				case BOOLEAN:	val = ((XPINSArray*)inputVal)->boolAtIndex(index);break;
				case NUMBER:	val = ((XPINSArray*)inputVal)->numAtIndex(index);break;
				case VECTOR:	val = ((XPINSArray*)inputVal)->vecAtIndex(index);break;
				case QUATERNION:val = ((XPINSArray*)inputVal)->quatAtIndex(index);break;
				case MATRIX:	val = ((XPINSArray*)inputVal)->matAtIndex(index);break;
				case POLYNOMIAL:val = ((XPINSArray*)inputVal)->polyAtIndex(index);break;
				case FIELD:		val = ((XPINSArray*)inputVal)->fieldAtIndex(index);break;
				case STRING:	val = ((XPINSArray*)inputVal)->strAtIndex(index);break;
				case OBJECT:	val = ((XPINSArray*)inputVal)->objAtIndex(index);break;
				case ARRAY:		val = ((XPINSArray*)inputVal)->arrAtIndex(index);break;
			}
			return ParseSubScripts(script, val, type, subscripts,startIndex+1);
		}
		default: return inputVal;
	}
}
void* ParseVariableArg(XPINSVarSpace* vars,DataType type,int argNum)
{
	switch (type)
	{
		case BOOLEAN:	return &vars->bVars[argNum];
		case NUMBER:	return &vars->nVars[argNum];
		case VECTOR:	return &vars->vVars[argNum];
		case QUATERNION:return &vars->qVars[argNum];
		case MATRIX:	return &vars->mVars[argNum];
		case POLYNOMIAL:return &vars->pVars[argNum];
		case FIELD:		return &vars->fVars[argNum];
		case STRING:	return &vars->sVars[argNum];
		case OBJECT:	return &vars->oVars[argNum];
		case ARRAY:		return &vars->aVars[argNum];
		default:return NULL;
	}
}
void* ParseConstArg(DataType type,int modNum,int argNum,void*literalValue,XPINSArray arguments)
{
	switch (type)
	{
			//Primative Constants
		case BOOLEAN:	return new bool(*(bool*)literalValue);
		case NUMBER:	return new double(*(double*)literalValue);
		case STRING:	return new string(*(string*)literalValue);
			//Complex Constants
		case VECTOR:{
			double x=*arguments.numAtIndex(0);
			double y=*arguments.numAtIndex(1);
			double z=*arguments.numAtIndex(2);
			return new Vector(x,y,z,(Vector::coordSystem)argNum);
		}
		case QUATERNION:{
			double r=*arguments.numAtIndex(0);
			Vector v=*arguments.vecAtIndex(1);
			return new Quaternion(r,v);
		}
		case MATRIX:{
			Matrix* mat=new Matrix(modNum,argNum);
			for (int i=0;i<modNum*argNum;++i){
				mat->values[i]=*arguments.numAtIndex(i);
			}
			return mat;
		}
		case POLYNOMIAL:{
			vector<Polynomial::Monomial> mons=*(vector<Polynomial::Monomial>*)literalValue;
			for (int i=0;i<argNum;++i){
				mons[i].coeff=*arguments.numAtIndex(i);
			}
			return new Polynomial(mons);
		}
		case FIELD:	{
			Polynomial P=*arguments.polyAtIndex(0);
			Polynomial Q=*arguments.polyAtIndex(1);
			Polynomial R=*arguments.polyAtIndex(2);
			return new VectorField(P,Q,R);
		}
		case ARRAY:	return new XPINSArray(arguments);
		default:return NULL;
	}
}
void* ParseExpArg(DataType type,opCode operation, bool assign,XPINSArray arguments)
{
	switch (type)
	{
		case BOOLEAN:	return new bool(XPINSBuiltIn::ParseBoolExp(operation,assign,arguments));
		case NUMBER:	return new double(XPINSBuiltIn::ParseNumExp(operation,assign,arguments));
		case VECTOR:	return new Vector(XPINSBuiltIn::ParseVecExp(operation,assign,arguments));
		case QUATERNION:return new Quaternion(XPINSBuiltIn::ParseQuatExp(operation,assign,arguments));
		case MATRIX:	return new Matrix(XPINSBuiltIn::ParseMatExp(operation,assign,arguments));
		case POLYNOMIAL:return new Polynomial(XPINSBuiltIn::ParsePolyExp(operation,assign,arguments));
		case FIELD:		return new VectorField(XPINSBuiltIn::ParseVecExp(operation,assign,arguments));
		default:return NULL;
	}
}
void* ParseBIFArg(DataType type,int number,XPINSArray arguments)
{
	switch (type)
	{
		case BOOLEAN:	return new bool(XPINSBuiltIn::ParseBoolBIF(number,arguments));
		case NUMBER:	return new double(XPINSBuiltIn::ParseNumBIF(number,arguments));
		case VECTOR:	return new Vector(XPINSBuiltIn::ParseVecBIF(number,arguments));
		case QUATERNION:return new Quaternion(XPINSBuiltIn::ParseQuatBIF(number,arguments));
		case MATRIX:	return new Matrix(XPINSBuiltIn::ParseMatBIF(number,arguments));
		case POLYNOMIAL:return new Polynomial(XPINSBuiltIn::ParsePolyBIF(number,arguments));
		case FIELD:		return new VectorField(XPINSBuiltIn::ParseVecBIF(number,arguments));
		case VOID:		XPINSBuiltIn::ParseVoidBIF(number,arguments);
		default:return NULL;
	}
}

#pragma mark Script Execution

void allocVars(XPINSScriptSpace& script)//Allocate Variable space
{
	script.data=new XPINSVarSpace();
	script.data->bVars=new bool[script.instructions.varSizes[0]];
	script.data->nVars=new double[script.instructions.varSizes[1]];
	script.data->vVars=new Vector[script.instructions.varSizes[2]];
	script.data->qVars=new Quaternion[script.instructions.varSizes[3]];
	script.data->mVars=new Matrix[script.instructions.varSizes[4]];
	script.data->pVars=new Polynomial[script.instructions.varSizes[5]];
	script.data->fVars=new VectorField[script.instructions.varSizes[6]];
	script.data->sVars=new string[script.instructions.varSizes[7]];
	script.data->oVars=new void*[script.instructions.varSizes[8]];
	script.data->aVars=new XPINSArray[script.instructions.varSizes[9]];
	script.data->Garbage=XPINSArray();
}
void XPINSParser::EmptyGarbage(XPINSParser::XPINSVarSpace &vars)
{
	vars.Garbage.clearArray();
}
void XPINSParser::EmptyAllGarbage()
{
	for (int i=0; i<allVarSpaces.size(); ++i)
	{
		EmptyGarbage(*allVarSpaces[i]);
	}
}
void XPINSParser::ParseScript(string scriptText,vector<XPINSBindings*> bindings)
{
	if(mathMod==NULL)mathMod=new XPINSMathModule();//Check Math Module
	XPINSScriptSpace script=XPINSScriptSpace(scriptText,bindings);
	allocVars(script);//Set up scriptVars Space
	allVarSpaces.resize(allVarSpaces.size()+1,script.data);
	ParseCode(script,script.instructions.instructions);//Run Script
	//Clean up
	//EmptyGarbage(*script.data);
	allVarSpaces.resize(allVarSpaces.size()-1);
	delete script.data;
	while (script.toDelete.size()>0)
	{
		delete script.toDelete.front();
		script.toDelete.pop_front();
	}
}
void ParseScriptCluster(string directory,vector<XPINSBindings*> bindings)
{
	directory+='/';
	XPINSScriptSpace script=XPINSScriptSpace(directory,"MAIN",bindings);
	allocVars(script);//Set up scriptVars Space
	allVarSpaces.resize(allVarSpaces.size()+1);
	allVarSpaces[allVarSpaces.size()-1]=script.data;
	ParseCode(script,script.instructions.instructions);//Run Script
	//Clean up
	//EmptyGarbage(*script.data);
	allVarSpaces.resize(allVarSpaces.size()-1);
	delete script.data;
	while (script.toDelete.size()>0)
	{
		delete script.toDelete.front();
		script.toDelete.pop_front();
	}
}
void* runScript(XPINSParser::XPINSScriptSpace& rootScript)
{
	/*//Parse Arguments
	 string scriptName=*ParseStrArg(rootScript, ',');
	 XPINSArray* params=ParseArrayArg(rootScript, ']');
	 //Execute script like normal
	 XPINSScriptSpace script=XPINSScriptSpace(rootScript.clusterURL,scriptName,rootScript.bindings);
	 if(!checkVersion(script))return NULL;//Check Script Version
	 script.scriptParams=params;
	 allocVars(script);//Set up scriptVars Space
	 allVarSpaces.resize(allVarSpaces.size()+1);
	 allVarSpaces[allVarSpaces.size()-1]=script.data;
	 ParseCode(script,0,-1);//Run Script
	 //Clean up
	 EmptyGarbage(*script.data);
	 allVarSpaces.resize(allVarSpaces.size()-1);
	 delete script.data;
	 while (script.toDelete.size()>0)
	 {
		delete script.toDelete.front();
		script.toDelete.pop_front();
	 }
	 return script.returnVal;*/
	return NULL;
}
//This is the actual code parser (makes loops easier). Call ParseCode and it will call this.
exitReason ParseCode(XPINSScriptSpace& script, vector<Instruction> instructions)
{
	for (int i=0; i<instructions.size(); ++i) {
		Instruction instruction=instructions[i];
		switch (instruction.type) {
			case ASSIGN:{
				DataType leftType=VOID,rightType=VOID;
				void* leftArg=ParseArg(script, instruction.left, leftType,true);
				void* rightArg=ParseArg(script, instruction.right, rightType);
				if (leftType==ARRAY&& instruction.left.subscripts.size()>0) {
					int index=*(double*)ParseRawArg(script,instruction.left.subscripts[instruction.left.subscripts.size()-1],NUMBER);
					switch (rightType)
					{
						case BOOLEAN:	((XPINSArray*)leftArg)->setBoolAtIndex(*(bool*)rightArg,index);break;
						case NUMBER:	((XPINSArray*)leftArg)->setNumAtIndex(*(double*)rightArg,index);break;
						case VECTOR:	((XPINSArray*)leftArg)->setVecAtIndex(*(Vector*)rightArg,index);break;
						case QUATERNION:((XPINSArray*)leftArg)->setQuatAtIndex(*(Quaternion*)rightArg,index);break;
						case MATRIX:	((XPINSArray*)leftArg)->setMatAtIndex(*(Matrix*)rightArg,index);break;
						case POLYNOMIAL:((XPINSArray*)leftArg)->setPolyAtIndex(*(Polynomial*)rightArg,index);break;
						case FIELD:		((XPINSArray*)leftArg)->setFieldAtIndex(*(VectorField*)rightArg,index);break;
						case STRING:	((XPINSArray*)leftArg)->setStrAtIndex(*(string*)rightArg,index);break;
						case OBJECT:	((XPINSArray*)leftArg)->setObjAtIndex(*(void**)rightArg,index);break;
						case ARRAY:		((XPINSArray*)leftArg)->setArrAtIndex(*(XPINSArray*)rightArg,index);break;
					}
				} else {
					switch (leftType) {
						case BOOLEAN:	*(bool*)leftArg=*(bool*)rightArg; break;
						case NUMBER:	*(double*)leftArg=*(double*)rightArg;break;
						case VECTOR:	*(Vector*)leftArg=*(Vector*)rightArg;break;
						case QUATERNION:*(Quaternion*)leftArg=*(Quaternion*)rightArg;break;
						case MATRIX:	*(Matrix*)leftArg=*(Matrix*)rightArg;break;
						case POLYNOMIAL:*(Polynomial*)leftArg=*(Polynomial*)rightArg;break;
						case FIELD:		*(VectorField*)leftArg=*(VectorField*)rightArg;break;
						case STRING:	*(string*)leftArg=*(string*)rightArg;break;
						case OBJECT:	*(void**)leftArg=*(void**)rightArg;break;
						case ARRAY:		*(XPINSArray*)leftArg=*(XPINSArray*)rightArg;break;
					}
				}
			}break;
			case VOIDFUNC:{
				ParseRawArg(script, instruction.right, VOID);
			}break;
			case IF:
			case ELSE:
				if (*(bool*)ParseRawArg(script, instruction.right,BOOLEAN)) {
					exitReason reason=ParseCode(script, instruction.block);
					if(reason!=ENDOFBLOCK)return reason;
					while (instructions[++i].type==ELSE);
					--i;
				}
				break;
			case WHILE:
				while (*(bool*)ParseRawArg(script, instruction.right,BOOLEAN)) {
					exitReason reason=ParseCode(script, instruction.block);
					if(reason==SCRIPTRETURN)return SCRIPTRETURN;
					else if (reason==LOOPBREAK) break;
				}
				break;
			case LOOP:{
				int numTimes=*(double*)ParseRawArg(script, instruction.right,NUMBER);
				for (int j=0; j<numTimes; ++j) {
					exitReason reason=ParseCode(script, instruction.block);
					if(reason==SCRIPTRETURN)return SCRIPTRETURN;
					else if (reason==LOOPBREAK) break;
				}
			}break;
			case BREAK:
				return LOOPBREAK;
			case RETURN:
				//script.returnVal=ParseArg(script, instruction.right);
				return SCRIPTRETURN;
		}
		//Manage Memory
		script.data->Garbage.combineWithArray(script.data->Trash);
		if(script.data->Garbage.size()>garbageCapcity)
		{
			//cout<<"DUMP "<<++dumpCount<<"\n";
			//EmptyGarbage(*script.data);
		}
	}
	return ENDOFBLOCK;
}