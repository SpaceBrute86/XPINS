//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>

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
bool* ParseBoolArg(XPINSScriptSpace& script, Argument arg);
double* ParseNumArg(XPINSScriptSpace& script, Argument arg);
XPINSScriptableMath::Vector* ParseVecArg(XPINSScriptSpace& script, Argument arg);
XPINSScriptableMath::Quaternion* ParseQuatArg(XPINSScriptSpace& script, Argument arg);
XPINSScriptableMath::Matrix* ParseMatArg(XPINSScriptSpace& script, Argument arg);
XPINSScriptableMath::Polynomial* ParsePolyArg(XPINSScriptSpace& script, Argument arg);
XPINSScriptableMath::VectorField* ParseFieldArg(XPINSScriptSpace& script, Argument arg);
string* ParseStrArg(XPINSScriptSpace& script, Argument arg);
void** ParseObjectArg(XPINSScriptSpace& script, Argument arg);
XPINSArray* ParseArrayArg(XPINSScriptSpace& script, Argument arg);


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
	instructions=XPINSInstructions::instructionsForScriptText(script);
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
	instructions=XPINSInstructions::instructionsForScriptText(script);
	bindings=bind;
}

#pragma mark Argument Parsing
void*  XPINSParser::ParseArg(XPINSScriptSpace& script, Argument arg,DataType& type,bool ignoreLastArrayIndex)
{
	void* baseValue=NULL;
	type=arg.dataType;
	switch (type) {
		case BOOLEAN:	baseValue = ParseBoolArg(script, arg); break;
		case NUMBER:	baseValue = ParseNumArg(script, arg);break;
		case VECTOR:	baseValue = ParseVecArg(script, arg);break;
		case QUATERNION:baseValue = ParseQuatArg(script, arg);break;
		case MATRIX:	baseValue = ParseMatArg(script, arg);break;
		case POLYNOMIAL:baseValue = ParsePolyArg(script, arg);break;
		case FIELD:		baseValue = ParseFieldArg(script, arg);break;
		case STRING:	baseValue = ParseStrArg(script, arg);break;
		case OBJECT:	baseValue = ParseObjectArg(script, arg);break;
		case ARRAY:		baseValue = ParseArrayArg(script, arg);break;
	}
	return ParseSubScripts(script, baseValue, type, arg.subscripts,ignoreLastArrayIndex);
	return NULL;
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
			switch ((int)*ParseNumArg(script,subscripts[startIndex])) {
				case 0: return &((Vector*)inputVal)->x;
				case 1: return &((Vector*)inputVal)->y;
				case 2: return &((Vector*)inputVal)->z;
				default:
					type=VECTOR;
					return inputVal;
			}
		case QUATERNION:
			type=NUMBER;
			switch ((int)*ParseNumArg(script,subscripts[startIndex])) {
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
			int rows=*ParseNumArg(script,subscripts[startIndex]);
			int cols=*ParseNumArg(script,subscripts[startIndex+1]);
			return ((Matrix*)inputVal)->values+((Matrix*)inputVal)->cols*rows+cols;
		}
		case FIELD:
			type=POLYNOMIAL;
			switch ((int)*ParseNumArg(script,subscripts[startIndex])) {
				case 0: return &((VectorField*)inputVal)->P;
				case 1: return &((VectorField*)inputVal)->Q;
				case 2: return &((VectorField*)inputVal)->R;
				default:
					type=FIELD;
					return inputVal;
			}
		case OBJECT:return inputVal;
		case ARRAY:{
			int index=*ParseNumArg(script, subscripts[startIndex]);
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
XPINSArray parseFunctionArgs(XPINSParser::XPINSScriptSpace &script,vector<Argument> args){
	XPINSArray values=XPINSArray();
	values.resize(args.size());
	for (int i=0; i<args.size(); ++i) {
		DataType type;
		void* val=ParseArg(script, args[i], type);
		values.setItemAtIndex(val, type, i);
		
	}
	return values;
}
bool* ParseBoolArg(XPINSScriptSpace& script,Argument arg)
{
	bool* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->bVars[arg.number];
			break;
		case CONST:
			retVal=new bool(*(bool*)arg.literalValue);
			break;
		case FUNC:
			retVal=(bool*)script.bindings[arg.modNumber]->BindFunction(arg.number,parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new bool(XPINSBuiltIn::ParseBoolBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new bool(XPINSBuiltIn::ParseBoolExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, BOOLEAN, script.data->Trash.size()-1);
	}
	return retVal;
}

double* ParseNumArg(XPINSScriptSpace& script,Argument arg)
{
	double* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->nVars[arg.number];
			break;
		case CONST:
			retVal=new double(*(double*)arg.literalValue);
			break;
		case FUNC:
			retVal=(double*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new double(XPINSBuiltIn::ParseNumBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new double(XPINSBuiltIn::ParseNumExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, NUMBER, script.data->Trash.size()-1);
	}
	return retVal;
}
Vector* ParseVecArg(XPINSScriptSpace& script,Argument arg)
{
	Vector* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->vVars[arg.number];
			break;
		case CONST:{
			double x=*ParseNumArg(script, arg.arguments[0]);
			double y=*ParseNumArg(script, arg.arguments[1]);
			double z=*ParseNumArg(script, arg.arguments[2]);
			retVal=new Vector(x,y,z,(Vector::coordSystem)arg.number);
		}break;
		case FUNC:
			retVal=(Vector*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new Vector(XPINSBuiltIn::ParseVecBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new Vector(XPINSBuiltIn::ParseVecExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, VECTOR, script.data->Trash.size()-1);
	}
	return retVal;
}
Quaternion* ParseQuatArg(XPINSScriptSpace& script,Argument arg)
{
	Quaternion* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->qVars[arg.number];
			break;
		case CONST:{
			double r=*ParseNumArg(script, arg.arguments[0]);
			Vector v=*ParseVecArg(script, arg.arguments[1]);
			retVal=new Quaternion(r,v);
		}break;
		case FUNC:
			retVal=(Quaternion*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new Quaternion(XPINSBuiltIn::ParseQuatBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new Quaternion(XPINSBuiltIn::ParseQuatExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, QUATERNION, script.data->Trash.size()-1);
	}
	return retVal;
}
Matrix* ParseMatArg(XPINSScriptSpace& script,Argument arg)
{
	Matrix* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->mVars[arg.number];
			break;
		case CONST:{
			retVal=new Matrix(arg.modNumber,arg.number);
			for (int i=0;i<arg.modNumber*arg.number;++i){
				retVal->values[i]=*ParseNumArg(script, arg.arguments[i]);
			}
		}break;
		case FUNC:
			retVal=(Matrix*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new Matrix(XPINSBuiltIn::ParseMatBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new Matrix(XPINSBuiltIn::ParseMatExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	} if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, MATRIX, script.data->Trash.size()-1);
	}
	return retVal;
}
Polynomial* ParsePolyArg(XPINSScriptSpace& script, Argument arg)
{
	Polynomial* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->pVars[arg.number];
			break;
		case CONST:{
			vector<Polynomial::Monomial> mons=*(vector<Polynomial::Monomial>*)arg.literalValue;
			for (int i=0;i<arg.number;++i){
				mons[i].coeff=*ParseNumArg(script, arg.arguments[i]);
			}
			retVal=new Polynomial(mons);
		}break;
		case FUNC:
			retVal=(Polynomial*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new Polynomial(XPINSBuiltIn::ParsePolyBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new Polynomial(XPINSBuiltIn::ParsePolyExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	} if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, POLYNOMIAL, script.data->Trash.size()-1);
	}
	return retVal;
}
VectorField* ParseFieldArg(XPINSScriptSpace& script,Argument arg)
{
	VectorField* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->fVars[arg.number];
			break;
		case CONST:{
			Polynomial x=*ParsePolyArg(script, arg.arguments[0]);
			Polynomial y=*ParsePolyArg(script, arg.arguments[1]);
			Polynomial z=*ParsePolyArg(script, arg.arguments[2]);
			retVal=new VectorField(x,y,z);
		}break;
		case FUNC:
			retVal=(VectorField*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
		case BIF:
			retVal=new VectorField(XPINSBuiltIn::ParseFieldBIF(arg.number,parseFunctionArgs(script, arg.arguments)));
			break;
		case EXP:
			retVal=new VectorField(XPINSBuiltIn::ParseFieldExp((opCode)arg.modNumber,arg.number!=0,parseFunctionArgs(script, arg.arguments)));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, FIELD, script.data->Trash.size()-1);
	}
	return retVal;
}
string* ParseStrArg(XPINSScriptSpace& script,Argument arg)
{
	string* retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->sVars[arg.number];
			break;
		case CONST:
			retVal=new string(*(string*)arg.literalValue);
			break;
		case FUNC:
			retVal=(string*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, STRING, script.data->Trash.size()-1);
	}
	return retVal;
}
void** ParseObjectArg(XPINSScriptSpace& script,Argument arg)
{
	void** retVal=NULL;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->oVars[arg.number];
			break;
		case FUNC:
			retVal=(void**)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
	}
	return retVal;
}

XPINSArray* ParseArrayArg(XPINSScriptSpace& script,Argument arg)
{
	XPINSArray* retVal = nullptr;
	switch (arg.type) {
		case VAR:
			retVal=&script.data->aVars[arg.number];
			break;
		case CONST:{
			retVal=new XPINSArray();
			arg.number=arg.arguments.size();
			retVal->resize(arg.number);
			retVal->resize(arg.number);
			for(int i=0;i<arg.number;++i)
			{
				DataType type=arg.dataType;
				void*val=ParseArg(script, arg.arguments[i],type);
				retVal->setItemAtIndex(val, type, i);
			}
		}break;
		case FUNC:
			retVal=(XPINSArray*)script.bindings[arg.modNumber]->BindFunction(arg.number, parseFunctionArgs(script, arg.arguments));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.resize(script.data->Trash.size()+1);
		script.data->Trash.setItemAtIndex(retVal, ARRAY, script.data->Trash.size()-1);
	}
	return retVal;
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
	EmptyGarbage(*script.data);
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
				DataType leftType,rightType;
				void* leftArg=ParseArg(script, instruction.left, leftType,true);
				void* rightArg=ParseArg(script, instruction.right, rightType);
				if (leftType==ARRAY&& instruction.left.subscripts.size()>0) {
					int index=*ParseNumArg(script, instruction.left.subscripts[instruction.left.subscripts.size()-1]);
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
				Argument function=instruction.right;
				switch (function.type) {
					case FUNC:
						script.bindings[function.modNumber]->BindFunction(function.number, parseFunctionArgs(script, function.arguments));
						break;
					case BIF:
						XPINSBuiltIn::ParseVoidBIF(function.number, parseFunctionArgs(script, function.arguments));
						break;
					case EXP:
						switch (function.dataType)
					{
						case BOOLEAN:
							XPINSBuiltIn::ParseBoolExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case NUMBER:
							XPINSBuiltIn::ParseNumExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case VECTOR:
							XPINSBuiltIn::ParseVecExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case QUATERNION:
							XPINSBuiltIn::ParseQuatExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case MATRIX:
							XPINSBuiltIn::ParseMatExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case POLYNOMIAL:
							XPINSBuiltIn::ParsePolyExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
						case FIELD:
							XPINSBuiltIn::ParseFieldExp((opCode)function.modNumber,function.number!=0,parseFunctionArgs(script, function.arguments));
							break;
					}
						break;
				}
			}break;
			case IF:
			case ELSE:
				if (*ParseBoolArg(script, instruction.right)) {
					exitReason reason=ParseCode(script, instruction.block);
					if(reason!=ENDOFBLOCK)return reason;
					while (instructions[++i].type==ELSE);
					--i;
				}
				break;
			case WHILE:
				while (*ParseBoolArg(script, instruction.right)) {
					exitReason reason=ParseCode(script, instruction.block);
					if(reason==SCRIPTRETURN)return SCRIPTRETURN;
					else if (reason==LOOPBREAK) break;
				}
				break;
			case LOOP:{
				int numTimes=*ParseNumArg(script, instruction.right);
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
			EmptyGarbage(*script.data);
		}
	}
	return ENDOFBLOCK;
}