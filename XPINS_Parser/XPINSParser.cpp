//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;

const int kPMajor=0;
const int kPMinor=12;

void* runScript(XPINSScriptSpace& script);
enum exitReason {
	ENDOFBLOCK,
	LOOPBREAK,
	SCRIPTRETURN,
};
exitReason ParseCode(XPINSScriptSpace& script, vector<Instruction> instructions);

size_t garbageCapcity=0x400/sizeof(double); //Default is about 1 KB
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
void clearArr(XPINSArray* arr, size_t index=-1)
{
	if (index==-1)
	{
		for (size_t i=0; i<arr->values.size(); ++i)
		{
			clearArr(arr,i);
		}
		arr->values.resize(0);
		arr->types="";
	}
	else
	{
		switch (arr->types[index])
		{
			case 'M':
				((Matrix*)arr->values[index])->Clear();
				break;
			case 'P':
				((Polynomial*)arr->values[index])->Clear();
				break;
			case 'F':
				((VectorField*)arr->values[index])->Clear();
				break;
			case 'A':
				clearArr((XPINSArray*)arr->values[index]);
				break;
		}
	}
}
size_t arrSize(XPINSArray* arr)
{
	int size=0;
	for (int i=0; i<arr->types.length(); ++i)
	{
		switch (arr->types[i])
		{
			case 'B':
			case 'N':
			case 'O':
				++size;
				break;
			case 'V':
				size+=3;
				break;
			case 'Q':
				size+=4;
				break;
			case 'M':
				size+=((Matrix*)arr->values[i])->GetRows()*((Matrix*)arr->values[i])->GetCols();
				break;
			case 'P':
				size+=((Polynomial*)arr->values[i])->Size()*2;
				break;
			case 'F':
				size+=((VectorField*)arr->values[i])->P.Size()*3;
				size+=((VectorField*)arr->values[i])->Q.Size()*3;
				size+=((VectorField*)arr->values[i])->R.Size()*3;
				break;
			case 'S':
				size+=((string*)arr->values[i])->length()/8;
				break;
			case 'A':
				size+=arrSize((XPINSArray*)arr->values[i]);
				break;
			default:
				break;
		}
	}
	return size;
}

#pragma mark Argument Parsing

void*  XPINSParser::ParseArg(XPINSScriptSpace& script, Argument arg,DataType& type)
{
	type=arg.dataType;
	switch (type) {
		case BOOLEAN:
			return ParseBoolArg(script, arg);
		case NUMBER:
			return ParseNumArg(script, arg);
		case VECTOR:{
			Vector* vec=ParseVecArg(script, arg);
			if (arg.subscripts.size()>0) {
				type=NUMBER;
				int num=*ParseNumArg(script,arg.subscripts[0]);
				switch (num) {
					case 0:
						return &vec->x;
					case 1:
						return &vec->y;
					case 2:
						return &vec->z;
				}
			}
			return vec;
		}break;
		case QUATERNION:{
			Quaternion* quat=ParseQuatArg(script, arg);
			if (arg.subscripts.size()>0) {
				type=NUMBER;
				int num=*ParseNumArg(script,arg.subscripts[0]);
				switch (num) {
					case 0:
						return &quat->r;
					case 1:
						return &quat->v.x;
					case 2:
						return &quat->v.y;
					case 3:
						return &quat->v.z;
				}
			}
			return quat;
		}
		case MATRIX:{
			Matrix*mat= ParseMatArg(script, arg);
			if (arg.subscripts.size()>=2){
				type=NUMBER;
				int rows=*ParseNumArg(script,arg.subscripts[0]);
				int cols=*ParseNumArg(script,arg.subscripts[1]);
				return mat->values+mat->cols*rows+cols;
			}
			else return mat;
		}break;
		case POLYNOMIAL:
			return ParsePolyArg(script, arg);
		case FIELD:{
			VectorField* field=ParseFieldArg(script, arg);
			if (arg.subscripts.size()>0) {
				type=POLYNOMIAL;
				int num=*ParseNumArg(script,arg.subscripts[0]);
				switch (num) {
					case 0:
						return &field->P;
					case 1:
						return &field->Q;
					case 2:
						return &field->R;
				}
			}
			return field;
		}
		case STRING:
			return ParseStrArg(script, arg);
		case OBJECT:
			return ParsePointerArg(script, arg);
		case ARRAY:
		{
			XPINSArray* arr=ParseArrayArg(script, arg,true);
			if(arg.subscripts.size()==0) return arr;
			else{
				for (int i=0; i<arg.subscripts.size()-1; ++i) {
					int index=*ParseNumArg(script, arg.subscripts[i]);
					arr=(XPINSArray*)arr->values[index];
				}
				int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
				type=arr->dataTypes[index];
				return arr->values[index];
			}
		}
	}
	return NULL;
}
bool* XPINSParser::ParseBoolArg(XPINSScriptSpace& script,Argument arg)
{
	bool* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal= (bool*)arr->values[index];
	}
	else switch (arg.type) {
		case VAR:
			retVal=&script.data->bVars[arg.number];
			break;
		case CONST:
			retVal=new bool(*(bool*)arg.literalValue);
			break;
		case FUNC:
			retVal=(bool*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new bool(XPINSBuiltIn::ParseBoolBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new bool(XPINSBuiltIn::ParseBoolExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=BOOLEAN;
		++script.data->GarbageCost;
	}
	return retVal;
}

double* XPINSParser::ParseNumArg(XPINSScriptSpace& script,Argument arg)
{
	double* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal= (double*)arr->values[index];
	}
	else if (arg.dataType==MATRIX){
		Matrix* mat=ParseMatArg(script, arg);
		int rows=*ParseNumArg(script,arg.subscripts[0]);
		int cols=*ParseNumArg(script,arg.subscripts[1]);
		retVal=mat->values+mat->cols*rows+cols;
	}
	else if (arg.dataType==VECTOR){
		Vector* vec=ParseVecArg(script, arg);
		int num=*ParseNumArg(script, arg.subscripts[0]);
		switch (num) {
			case 0:
				retVal=&vec->x;
				break;
			case 1:
				retVal=&vec->y;
				break;
			case 2:
				retVal=&vec->z;
				break;
		}
	}
	else if (arg.dataType==QUATERNION){
		Quaternion* quat=ParseQuatArg(script, arg);
		int num=*ParseNumArg(script, arg.subscripts[0]);
		switch (num) {
			case 0:
				retVal=&quat->r;
				break;
			case 1:
				retVal=&quat->v.x;
				break;
			case 2:
				retVal=&quat->v.y;
				break;
			case 3:
				retVal=&quat->v.z;
				break;
		}
	}
	else switch (arg.type) {
		case VAR:
			retVal=&script.data->nVars[arg.number];
			break;
		case CONST:
			retVal=new double(*(double*)arg.literalValue);
			break;
		case FUNC:
			retVal=(double*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new double(XPINSBuiltIn::ParseNumBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new double(XPINSBuiltIn::ParseNumExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=NUMBER;
		++script.data->GarbageCost;
	}
	return retVal;
}
Vector* XPINSParser::ParseVecArg(XPINSScriptSpace& script,Argument arg)
{
	Vector* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal= (Vector*)arr->values[index];
	} else if (arg.dataType==MATRIX) {
		Matrix mat=*ParseMatArg(script, arg);
		retVal=new Vector(Matrix::VectorForMatrix(mat));
	}
	else switch (arg.type) {
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
			retVal=(Vector*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new Vector(XPINSBuiltIn::ParseVecBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new Vector(XPINSBuiltIn::ParseVecExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	}
	if(arg.type!=VAR||arg.dataType==MATRIX){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=VECTOR;
		script.data->GarbageCost+=3;
	}
	return retVal;
}
Quaternion* XPINSParser::ParseQuatArg(XPINSScriptSpace& script,Argument arg)
{
	Quaternion* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal = (Quaternion*)arr->values[index];
	}
	else switch (arg.type) {
		case VAR:
			retVal=&script.data->qVars[arg.number];
			break;
		case CONST:{
			double r=*ParseNumArg(script, arg.arguments[0]);
			Vector v=*ParseVecArg(script, arg.arguments[1]);
			retVal=new Quaternion(r,v);
		}break;
		case FUNC:
			retVal=(Quaternion*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new Quaternion(XPINSBuiltIn::ParseQuatBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new Quaternion(XPINSBuiltIn::ParseQuatExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=QUATERNION;
		script.data->GarbageCost+=4;
	}
	return retVal;
}
Matrix *XPINSParser::ParseMatArg(XPINSScriptSpace& script,Argument arg)
{
	Matrix* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal = (Matrix*)arr->values[index];
	} else if (arg.dataType==VECTOR){
		Vector vec=*ParseVecArg(script, arg);
		retVal=new Matrix(Matrix::MatrixForVector(vec));
	} else if (arg.dataType==NUMBER){
		double num=*ParseNumArg(script, arg);
		retVal=new Matrix(1,1,num);
	} else switch (arg.type) {
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
			retVal=(Matrix*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new Matrix(XPINSBuiltIn::ParseMatBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new Matrix(XPINSBuiltIn::ParseMatExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	} if(arg.type!=VAR||arg.dataType==VECTOR||arg.dataType==NUMBER){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=MATRIX;
		script.data->GarbageCost+=retVal->rows*retVal->cols;
	}
	return retVal;
}
Polynomial* XPINSParser::ParsePolyArg(XPINSScriptSpace& script, Argument arg)
{
	Polynomial* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal = (Polynomial*)arr->values[index];
	} else if (arg.dataType==NUMBER){
		double num=*ParseNumArg(script, arg);
		retVal= new Polynomial(num);
	} else if (arg.dataType==FIELD){
		VectorField* field=ParseFieldArg(script, arg);
		int num=*ParseNumArg(script, arg.subscripts[0]);
		switch (num) {
			case 0:
				retVal=&field->P;
				break;
			case 1:
				retVal=&field->Q;
				break;
			case 2:
				retVal=&field->R;
				break;
		}
	}
	else switch (arg.type) {
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
			retVal=(Polynomial*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new Polynomial(XPINSBuiltIn::ParsePolyBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new Polynomial(XPINSBuiltIn::ParsePolyExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	} if(arg.type!=VAR||arg.dataType==NUMBER||arg.dataType==FIELD){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=POLYNOMIAL;
		script.data->GarbageCost+=3*retVal->Size();
	}
	return retVal;
}
VectorField* XPINSParser::ParseFieldArg(XPINSScriptSpace& script,Argument arg)
{
	VectorField* retVal=NULL;
	if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size()-1; ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			arr=(XPINSArray*)arr->values[index];
		}
		int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		retVal = (VectorField*)arr->values[index];
	} else if (arg.dataType==VECTOR) {
		Vector vec=*ParseVecArg(script, arg);
		retVal=new VectorField(vec);
	}
	else switch (arg.type) {
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
			retVal=(VectorField*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
		case BIF:
			retVal=new VectorField(XPINSBuiltIn::ParseFieldBIF(arg.number,script,arg.arguments));
			break;
		case EXP:
			retVal=new VectorField(XPINSBuiltIn::ParseFieldExp((opCode)arg.modNumber,arg.number!=0,script,arg.arguments));
			break;
	}
	if(arg.type!=VAR||arg.dataType==VECTOR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=FIELD;
		script.data->GarbageCost+=3*(retVal->P.Size()+retVal->Q.Size()+retVal->R.Size());
	}
	return retVal;
}
string* XPINSParser::ParseStrArg(XPINSScriptSpace& script,Argument arg)
{
	 string* retVal=NULL;
	 if (arg.dataType==ARRAY){
		XPINSArray* arr=ParseArrayArg(script, arg,true);
		 for (int i=0; i<arg.subscripts.size()-1; ++i) {
			 int index=*ParseNumArg(script, arg.subscripts[i]);
			 arr=(XPINSArray*)arr->values[index];
		 }
		 int index=*ParseNumArg(script, arg.subscripts[arg.subscripts.size()-1]);
		 retVal = (string*)arr->values[index];
	 }
	 else switch (arg.type) {
		case VAR:
			 retVal=&script.data->sVars[arg.number];
			 break;
		case CONST:
			 retVal=new string(*(string*)arg.literalValue);
			 break;
		case FUNC:
			 retVal=(string*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			 break;
	 }
	 if(arg.type!=VAR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		 script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		 script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=STRING;
		script.data->GarbageCost+=retVal->length()/8;
	 }
	 return retVal;
}
void** XPINSParser::ParsePointerArg(XPINSScriptSpace& script,Argument arg, DataType* type)
{
	void** retVal=NULL;
	if(arg.dataType==OBJECT){
		switch (arg.type) {
			case VAR:
				retVal=&script.data->oVars[arg.number];
				break;
			case FUNC:
				retVal=(void**)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
				break;
		}
	} else {
		if(!type)type=new DataType();
		retVal=new void*(ParseArg(script, arg, *type));
		switch (*type) {
			case BOOLEAN: retVal=new void*(new bool(*(bool*)retVal));break;
			case NUMBER: retVal=new void*(new double(*(double*)retVal));break;
			case VECTOR: retVal=new void*(new Vector(*(Vector*)retVal));break;
			case QUATERNION: retVal=new void*(new Quaternion(*(Quaternion*)retVal));break;
			case MATRIX: retVal=new void*(new Matrix(((Matrix*)retVal)->Copy()));break;
			case POLYNOMIAL: retVal=new void*(new Polynomial(((Polynomial*)retVal)->Copy()));break;
			case FIELD: retVal=new void*(new VectorField(((VectorField*)retVal)->Copy()));break;
			case STRING: retVal=new void*(new string(*(string*)retVal));break;
			case OBJECT:retVal=*(void***)retVal;break;
			case ARRAY:retVal=new void*(new XPINSArray(*(XPINSArray*)retVal));break;
		}
	}
	return retVal;
}

XPINSArray* XPINSParser::ParseArrayArg(XPINSScriptSpace& script,Argument arg, bool ignoreSubscripts)
{
	XPINSArray* retVal = nullptr;
	if (!ignoreSubscripts){
		retVal=ParseArrayArg(script, arg,true);
		for (int i=0; i<arg.subscripts.size(); ++i) {
			int index=*ParseNumArg(script, arg.subscripts[i]);
			retVal=(XPINSArray*)retVal->values[index];
		}
	}
	else switch (arg.type) {
		case VAR:
			retVal=&script.data->aVars[arg.number];
			break;
		case CONST:{
			retVal=new XPINSArray();
			retVal->values.resize(arg.number);
			for(int i=0;i<arg.number;++i)
			{
				DataType type=arg.dataType;
				retVal->values[i]=*ParsePointerArg(script, arg,&type);
				retVal->dataTypes.resize(retVal->dataTypes.size()+1);
				retVal->dataTypes[retVal->dataTypes.size()-1]=type;
			}
		}break;
		case FUNC:
			retVal=(XPINSArray*)script.bindings[arg.modNumber]->BindFunction(arg.number, script,arg.arguments);
			break;
	}
	if(arg.type!=VAR){
		script.data->Trash.values.resize(script.data->Trash.values.size()+1);
		script.data->Trash.values[script.data->Trash.values.size()-1]=retVal;
		script.data->Trash.dataTypes.resize(script.data->Trash.dataTypes.size()+1);
		script.data->Trash.dataTypes[script.data->Trash.dataTypes.size()-1]=ARRAY;
		script.data->GarbageCost+=arrSize(retVal);
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
	clearArr(&vars.Garbage);
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
				DataType type;
				void* leftArg=ParseArg(script, instruction.left, type);
				if(instruction.left.dataType==ARRAY&&instruction.left.subscripts.size()>0)type=OBJECT;
				switch (type) {
					case BOOLEAN:
						*(bool*)leftArg=*ParseBoolArg(script, instruction.right);
						break;
					case NUMBER:
						*(double*)leftArg=*ParseNumArg(script, instruction.right);
						break;
					case VECTOR:
						*(Vector*)leftArg=*ParseVecArg(script, instruction.right);
						break;
					case QUATERNION:
						*(Quaternion*)leftArg=*ParseQuatArg(script, instruction.right);
						break;
					case MATRIX:
						*(Matrix*)leftArg=*ParseMatArg(script, instruction.right);
						break;
					case POLYNOMIAL:
						*(Polynomial*)leftArg=*ParsePolyArg(script, instruction.right);
						break;
					case FIELD:
						*(VectorField*)leftArg=*ParseFieldArg(script, instruction.right);
						break;
					case STRING:
						*(string*)leftArg=*ParseStrArg(script, instruction.right);
						break;
					case OBJECT:
						*(void**)leftArg=*ParsePointerArg(script, instruction.right);
						break;
					case ARRAY:
						*(XPINSArray*)leftArg=*ParseArrayArg(script, instruction.right);
						break;
				}
			}break;
			case VOIDFUNC:{
				Argument function=instruction.right;
				switch (function.type) {
					case FUNC:
						script.bindings[function.modNumber]->BindFunction(function.number, script,function.arguments);
						break;
					case BIF:
						XPINSBuiltIn::ParseVoidBIF(function.number, script, function.arguments);
						break;
					case EXP:
						switch (function.dataType)
						{
							case BOOLEAN:
								XPINSBuiltIn::ParseBoolExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case NUMBER:
								XPINSBuiltIn::ParseNumExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case VECTOR:
								XPINSBuiltIn::ParseVecExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case QUATERNION:
								XPINSBuiltIn::ParseQuatExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case MATRIX:
								XPINSBuiltIn::ParseMatExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case POLYNOMIAL:
								XPINSBuiltIn::ParsePolyExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
								break;
							case FIELD:
								XPINSBuiltIn::ParseFieldExp((opCode)function.modNumber,function.number!=0,script,function.arguments);
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
				//script.returnVal=*ParsePointerArg(script, instruction.right);
				return SCRIPTRETURN;
		}
		//Manage Memory
		size_t oldSize=script.data->Garbage.values.size();
		script.data->Garbage.values.resize(oldSize+script.data->Trash.values.size());
		for(int i=0;i<script.data->Trash.values.size();++i)
		{
			script.data->Garbage.values[oldSize+i]=script.data->Trash.values[i];
			script.data->Garbage.types+=script.data->Trash.types[i];
		}
		clearArr(&script.data->Trash);
		if(script.data->Garbage.values.size()>garbageCapcity)
		{
			EmptyGarbage(*script.data);
		}
	}
	return ENDOFBLOCK;
}