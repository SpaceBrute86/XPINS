//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;

const int kPMajor=0;
const int kPMinor=11;
const size_t garbageCapacity=0x100;

void* runScript(XPINSScriptSpace& script);
void ParseCode(XPINSScriptSpace& script, int, int);

vector<XPINSVarSpace*>allVarSpaces;

#pragma mark Variable Space, Script Space, and Array Management

XPINSVarSpace::~XPINSVarSpace(){
	delete[] bVars;
	delete[] nVars;
	delete[] vVars;
	delete[] mVars;
	delete[] pVars;
	delete[] sVars;
	delete[] oVars;
	delete[] aVars;
}
XPINSScriptSpace::XPINSScriptSpace(string script,vector<XPINSBindings*> bind)
{
	instructions=script;
	index=0;
	bindings=bind;
	
}
XPINSScriptSpace::XPINSScriptSpace(string cluster,string name,vector<XPINSBindings*> bind)
{
	clusterURL=cluster;
	ifstream inFile;
	inFile.open(clusterURL+name+".XPINSX");
	instructions="";
	char ch;
	while (inFile.get(ch))instructions+=ch;
	inFile.close();
	index=0;
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
	}
	else
	{
		switch (arr->types[index]) {
			case 'M':
				((Matrix*)arr->values[index])->Clear();
				break;
			case 'P':
				((Polynomial*)arr->values[index])->Clear();
				break;
			case 'A':
				clearArr((XPINSArray*)arr->values[index]);
				break;
			default:
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
			case 'M':
				size+=((Matrix*)arr->values[i])->GetRows()*((Matrix*)arr->values[i])->GetCols();
				break;
			case 'P':
				size+=((Polynomial*)arr->values[i])->Size()*2;
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
char XPINSScriptSpace::currentChar()
{
	return index<instructions.length()?instructions[index]:' ';
}
bool XPINSScriptSpace::matchesString(string testString)
{
	for(int i=0;i<testString.length();++i)
	{
		if(i+index>=instructions.length()|| instructions[index+i]!=testString[i]) return false;
	}
	return true;
}

#pragma mark Helper functions

char readType(XPINSScriptSpace& script)
{
	if(script.instructions[script.index]!='~')
		return script.instructions[script.index+1]=='A'?'O':script.instructions[script.index+1];
	else switch(script.instructions[script.index+1])
	{
		case 'T':
		case 'F':
			return'B';
		case '<':
		case 'S':
		case 'P':
			return'V';
		case '[':
			return'M';
		case '\"':
			return'S';
		case '(':
			return'P';
		case '{':
			return'A';
		default:return'N';
	}
}
int XPINSParser::readInt(XPINSScriptSpace& script,char expectedEnd)//Read an Integer from the script
{
	int index=0;
	for(;script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!='['&&script.currentChar()!='('&&script.currentChar()!='+'&&script.currentChar()!='-'&&script.currentChar()!=')';++script.index)
	{
		index*=10;
		if(script.currentChar()=='1')index+=1;
		else if(script.currentChar()=='2')index+=2;
		else if(script.currentChar()=='3')index+=3;
		else if(script.currentChar()=='4')index+=4;
		else if(script.currentChar()=='5')index+=5;
		else if(script.currentChar()=='6')index+=6;
		else if(script.currentChar()=='7')index+=7;
		else if(script.currentChar()=='8')index+=8;
		else if(script.currentChar()=='9')index+=9;
		else if(script.currentChar()!='0')index/=10;
	}
	return index;
}

#pragma mark Argument Parsing

bool* XPINSParser::ParseBoolArg(XPINSScriptSpace& script,char expectedEnd)
{
	bool* retVal=NULL;
	while (script.index<script.instructions.length()&&script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	if (script.index>=script.instructions.length()) return NULL;
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (bool*)runScript(script);
	}
	else if(script.matchesString("$B"))//Variable
	{
		script.index+=2;
		size_t index=readInt(script, expectedEnd);
		retVal=&script.data->bVars[index];
	}
	else if(script.currentChar()=='~')//constant
	{
		retVal=new bool(XPINSBuiltIn::ParseBoolConst(script));

	}
	else if(script.matchesString("?B"))//Expression
	{
		retVal=new bool(XPINSBuiltIn::ParseBoolExp(script));
	}
	else if(script.matchesString("#B"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(bool*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//Array Value
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((bool*)arr->values[arrIndex]);
	}
	else if(script.matchesString("XB"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=new bool(XPINSBuiltIn::ParseBoolBIF(index,script));
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		++script.data->GarbageCost;
	}
	while(script.currentChar()!=expectedEnd&&(script.currentChar()!=')'))++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
double* XPINSParser::ParseNumArg(XPINSScriptSpace& script,char expectedEnd)
{
	double* retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (double*)runScript(script);
	}
	else if(script.matchesString("$N"))//Variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->nVars[index];
	}
	else if(script.currentChar()=='~')//Constant
	{
		retVal=new double(XPINSBuiltIn::ParseNumConst(script, expectedEnd));
	}
	else if(script.matchesString("?N"))//Expression
	{
		retVal=new double(XPINSBuiltIn::ParseNumExp(script));
		while(script.instructions[script.index]!='?')++script.index;
	}
	else if(script.matchesString("#NM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(double*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//Array Value
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=(double*)arr->values[arrIndex];
	}
	else if(script.matchesString("XN"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=new double(XPINSBuiltIn::ParseNumBIF(index,script));
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		++script.data->GarbageCost;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	return retVal;
}
Vector* XPINSParser::ParseVecArg(XPINSScriptSpace& script,char expectedEnd)
{
	Vector *retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (Vector*)runScript(script);
	}
	else if(script.matchesString("$V"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->vVars[index];
	}
	else if(script.currentChar()=='~')//Constant
	{
		retVal=new Vector(XPINSBuiltIn::ParseVecConst(script));
	}
	else if(script.matchesString("?V"))
	{
		retVal=new Vector(XPINSBuiltIn::ParseVecExp(script));
	}
	else if(script.matchesString("#VM")){//User-defined Function
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(Vector*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//Array Value
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((Vector*)arr->values[arrIndex]);
	}
	else if(script.matchesString("XV"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=new Vector(XPINSBuiltIn::ParseVecBIF(index,script));
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		script.data->GarbageCost+=3;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')'&&script.currentChar()!='\n')++script.index;
	if(script.instructions[script.index+1]=='?'&&script.currentChar()!='\n')script.index+=2;
	return retVal;
}
Matrix *XPINSParser::ParseMatArg(XPINSScriptSpace& script,char expectedEnd)
{
	Matrix* retVal=new Matrix();
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='2') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (Matrix*)runScript(script);
	}
	else  if(script.matchesString("$M"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->mVars[index];
	}
	else if(script.matchesString("~["))//constant (can contain varialbes, though)
	{
		retVal=new Matrix(XPINSBuiltIn::ParseMatConst(script));
	}
	else if(script.matchesString("?M"))
	{
		retVal=new Matrix(XPINSBuiltIn::ParseMatExp(script));
	}
	else if(script.matchesString("#MM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(Matrix*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("XM"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=new Matrix(XPINSBuiltIn::ParseMatBIF(index,script));
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//Array
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((Matrix*)arr->values[arrIndex]);
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		script.data->GarbageCost+=retVal->GetRows()*retVal->GetCols();
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
Polynomial* XPINSParser::ParsePolyArg(XPINSScriptSpace& script, char expectedEnd)
{
	Polynomial* retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (Polynomial*)runScript(script);
	}
	else if(script.matchesString("$P"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->pVars[index];
	}
	else if(script.matchesString("~("))//constant (can contain varialbes, though)
	{
		retVal=new Polynomial(XPINSBuiltIn::ParsePolyConst(script));
	}
	else if(script.matchesString("?P"))
	{
		retVal=new Polynomial(XPINSBuiltIn::ParsePolyExp(script));
	}
	else if(script.matchesString("#PM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(Polynomial*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("XP"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=new Polynomial(XPINSBuiltIn::ParsePolyBIF(index,script));
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((Polynomial*)arr->values[arrIndex]);
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		script.data->GarbageCost+=2*retVal->Size();
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
string *XPINSParser::ParseStrArg(XPINSScriptSpace& script,char expectedEnd)
{
	string *retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='#'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (string*)runScript(script);
	}
	else if(script.matchesString("$S"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->sVars[index];
	}
	else if(script.currentChar()=='~')//User-defined Function
	{
		retVal=new string(XPINSBuiltIn::ParseStrConst(script));
	}
	else if(script.matchesString("#SM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		retVal=(string*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		XPINSArray arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((string*)arr.values[arrIndex]);
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		script.data->GarbageCost+=retVal->length()/8;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
void** XPINSParser::ParsePointerArg(XPINSScriptSpace& script,char expectedEnd, char* type)
{
	void** retVal=NULL;
	if(!type)type=new char();
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	*type=readType(script);
	bool temporary=script.currentChar()!='$'||*type!='O';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		return new void*(runScript(script));
	}
	else switch(*type)
	{
		case 'O':
			if(script.matchesString("$O"))//variable
			{
				script.index+=2;
				int index=readInt(script, expectedEnd);
				retVal=&script.data->oVars[index];
				*type='O';
			}
			else if(script.matchesString("#OM"))//User-defined Function
			{
				int mNum=readInt(script,'F');
				int fNum=readInt(script,'(');
				retVal=(void**)script.bindings[mNum]->BindFunction(fNum, script);
				*type='O';
			}
			else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
			{
				XPINSArray* arr=ParseArrayArg(script, '[');
				++script.index;
				if(script.currentChar()==':')
				{
					*type='A';
					XPINSArray* array=new XPINSArray(*arr);
					retVal=new void*(array);
				}
				else
				{
					int arrIndex=readInt(script, ']');
					retVal=(void**)arr->values[arrIndex];
					*type=arr->types[arrIndex];
				}
			}
			break;
		case 'B':
		{
			bool* b=new bool(*ParseBoolArg(script, expectedEnd));
			retVal=new void*(b);
		}break;
		case 'N':
		{
			double* d=new double(*ParseNumArg(script, expectedEnd));
			retVal=new void*(d);
		}break;
		case 'V':
		{
			Vector* v=new Vector(*ParseVecArg(script, expectedEnd));
			retVal=new void*(v);
		}break;
		case 'M':
		{
			Matrix* m=new Matrix(*ParseMatArg(script, expectedEnd));
			retVal=new void*(m);
		}break;
		case 'P':
		{
			Polynomial* p=new Polynomial(*ParsePolyArg(script, expectedEnd));
			retVal=new void*(p);
		}break;
		case 'S':
		{
			string* s=new string(*ParseStrArg(script, expectedEnd));
			retVal=new void*(s);
		}break;
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		++script.data->GarbageCost;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
XPINSArray* XPINSParser::ParseArrayArg(XPINSScriptSpace& script,char expectedEnd)
{
	XPINSArray* retVal = nullptr;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='#'&&script.currentChar()!='@') ++script.index;//Get to Key Character
	bool temporary=script.currentChar()!='$';
	if(script.matchesString("@RUN["))//Run another Script
	{
		script.index+=5;
		retVal= (XPINSArray*)runScript(script);
	}
	else if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		XPINSArray* arr=&script.data->aVars[index];
		if(script.currentChar()=='[')
		{
			int temp=script.index;
			++script.index;
			int arrIndex=readInt(script, ']');
			if(expectedEnd=='['&&script.instructions[script.index+1]!='[')
			{
				retVal=arr;
				script.index=temp;
			}
			else retVal=(XPINSArray*)(arr->values[arrIndex]);
		}
		else retVal=arr;
	}
	else if(script.currentChar()=='~')//Constant
	{
		retVal=new XPINSArray(XPINSBuiltIn::ParseArrConst(script));
	}
	else if(script.matchesString("#AM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		XPINSArray* arr=(XPINSArray*)script.bindings[mNum]->BindFunction(fNum, script);
		while (script.currentChar()!=')')++script.index;
		if(script.instructions[script.index+1]=='[')
		{
			int temp=++script.index;
			++script.index;
			int arrIndex=readInt(script, ']');
			if(expectedEnd=='['&&script.instructions[script.index+1]!='[')
			{
				retVal=arr;
				script.index=temp;
			}
			else retVal=(XPINSArray*)(arr->values[arrIndex]);
		}
		else retVal=arr;
	}
	else if(script.matchesString("@PARAMS"))//Script Parameters
	{
		script.index+=7;
		XPINSArray* arr=script.scriptParams;
		if(script.currentChar()=='[')
		{
			int temp=script.index;
			++script.index;
			int arrIndex=readInt(script, ']');
			if(expectedEnd=='['&&script.instructions[script.index+1]!='[')
			{
				retVal=arr;
				script.index=temp;
			}
			else retVal=(XPINSArray*)(arr->values[arrIndex]);
		}
		else retVal=arr;
	}
	if(temporary)
	{
		script.data->Trash.push_back(retVal);
		script.data->GarbageCost+=arrSize(retVal);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}

#pragma mark Conditionals and Loops

void skipBlock(XPINSScriptSpace& script)
{
	while (script.index<script.instructions.length()&&script.currentChar()!='{')++script.index;
	int num=1;
	//skip if block
	while(num>0){
		if(++script.index>=script.instructions.length())break;
		if(script.currentChar()=='{')
			++num;
		else if(script.currentChar()=='}')
			num--;
	}
}
void ParseIf(XPINSScriptSpace& script)
{
	while (script.currentChar()!='$'&&script.currentChar()!='?'&&script.currentChar()!='~'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;
	if(!*ParseBoolArg(script, ']'))//IF was false
	{
		skipBlock(script);
		while (script.currentChar()!='\n')++script.index;
		int temp=script.index;
		++script.index;
		if(script.matchesString("@EL"))
		{
			script.index+=3;
			if (script.matchesString("IF")) ParseIf(script);//ELIF
			else while (script.currentChar()!='{')++script.index;//ELSE
		}
		else script.index=temp;
	}
	else while (script.currentChar()!='{')++script.index;//Execute if
}
void ParseLoop(XPINSScriptSpace & script)
{
	while (script.currentChar()!='$'&&script.currentChar()!='?'&&script.currentChar()!='~'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;
	int expressionIndex=script.index--;
	while (script.currentChar()!='{')++script.index;
	int loopStart=script.index;
	skipBlock(script);
	int loopStop=script.index;
	script.index=expressionIndex;
	int temp=script.index;
	while (*ParseBoolArg(script, ']')) {
		ParseCode(script, loopStart, loopStop);
		script.index=temp;
	}
	skipBlock(script);
}

#pragma mark Script Execution

bool checkVersion(XPINSScriptSpace& script)//Check to make sure script is compatible
{
	for(int i=0;i<script.instructions.length();++i)
	{
		if(script.matchesString("@PARSER["))
		{
			script.index+=8;
			int MAJOR=readInt(script, '.');
			int MINOR=readInt(script, ']');
			return (MAJOR==kPMajor&&MINOR<=kPMinor);
		}
	}
	return false;
}
void allocVars(XPINSScriptSpace& script)//Allocate Variable space
{
	script.index=0;
	script.data=new XPINSVarSpace();
	while(script.matchesString("@VARS"))++script.index;
	//BOOLS
	while(script.currentChar()!='B')++script.index;
	size_t bsize=readInt(script, ' ');
	script.data->bVars=new bool[bsize];
	for (int i=0; i<bsize; ++i)script.data->bVars[i]=false;
	//NUMS
	while(script.currentChar()!='N')++script.index;
	size_t nsize=readInt(script, ' ');
	script.data->nVars=new double[nsize];
	for (int i=0; i<nsize; ++i)script.data->nVars[i]=0.0;
	//VECs
	while(script.currentChar()!='V')++script.index;
	size_t vsize=readInt(script, ' ');
	script.data->vVars=new Vector[vsize];
	for (int i=0; i<vsize; ++i)script.data->vVars[i]=*new Vector();
	//MATs
	while(script.currentChar()!='M')++script.index;
	size_t msize=readInt(script, ' ');
	script.data->mVars=new Matrix[msize];
	for (int i=0; i<msize; ++i)script.data->mVars[i]=*new Matrix();
	//POLYs
	while(script.currentChar()!='P')++script.index;
	size_t psize=readInt(script, ' ');
	script.data->pVars=new Polynomial[psize];
	for (int i=0; i<psize; ++i)script.data->pVars[i]=*new Polynomial();
	//STRs
	while(script.currentChar()!='S')++script.index;
	size_t ssize=readInt(script, ' ');
	script.data->sVars=new string[ssize];
	for (int i=0; i<ssize; ++i)script.data->sVars[i]="";
	//Pointers
	while(script.currentChar()!='O')++script.index;
	size_t osize=readInt(script, ' ');
	script.data->oVars=new void*[osize];
	for (int i=0; i<osize; ++i)script.data->oVars[i]=NULL;
	//ARRs
	while(script.currentChar()!='A')++script.index;
	size_t asize=readInt(script, ' ');
	script.data->aVars=new XPINSArray[asize];
	for (int i=0; i<asize; ++i)script.data->aVars[i]=XPINSArray();
	//Garbage
	script.data->Garbage=list<void*>();
}
void XPINSParser::EmptyGarbage(XPINSParser::XPINSVarSpace &vars)
{
	while (vars.Garbage.size()>0)
	{
		delete vars.Garbage.back();
		vars.Garbage.pop_back();
	}
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
	XPINSScriptSpace script=XPINSScriptSpace(scriptText,bindings);
	if(!checkVersion(script))return;//Check Script Version
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
}
void ParseScriptCluster(string directory,vector<XPINSBindings*> bindings)
{
	directory+='/';
	XPINSScriptSpace script=XPINSScriptSpace(directory,"MAIN",bindings);
	if(!checkVersion(script))return;//Check Script Version
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
}
void* runScript(XPINSParser::XPINSScriptSpace& rootScript)
{
	//Parse Arguments
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
	return script.returnVal;
}
//This is the actual code parser (makes loops easier). Call ParseCode and it will call this.
void ParseCode(XPINSScriptSpace& script,int start,int stop)
{
	script.index=start;
	//Get to the start of the Script
	if(stop<0) while(!script.matchesString("@CODE"))++script.index;
	while(script.index<script.instructions.length())
	{
		while(script.index<script.instructions.length()&&script.currentChar()!='\n')++script.index;
		if (script.index>=script.instructions.length()||script.matchesString("@END")
			||(stop>0&&script.index>=stop))//END OF SCRIPT
			break;
		++script.index;
		switch (script.currentChar())//Determine Key Character
		{
			case '$'://Variable
			{
				++script.index;
				switch (script.currentChar())
				{
					case 'B'://Bool Variable
					{
						int index=readInt(script, '=');
						bool b=*ParseBoolArg(script, '\n');
						script.data->bVars[index]=b;
					}break;
					case 'N'://NUM variable
					{
						int index=readInt(script, '=');
						double d=*ParseNumArg(script, '\n');
						script.data->nVars[index]=d;
					}break;
					case 'V'://VEC variable
					{
						int index=readInt(script, '=');
						Vector v=*ParseVecArg(script, '\n');
						script.data->vVars[index]=v;
					}break;
					case 'M'://MAT variable
					{
						int index=readInt(script, '=');
						Matrix m=*ParseMatArg(script, '\n');
						script.data->mVars[index].Clear();
						script.data->mVars[index]=m;
					}break;
					case 'P'://POLY variable
					{
						int index=readInt(script, '=');
						Polynomial p=*ParsePolyArg(script, '\n');
						script.data->pVars[index].Clear();
						script.data->pVars[index]=p;
					}break;
					case 'S'://STR variable
					{
						int index=readInt(script, '=');
						string s=*ParseStrArg(script, '\n');
						script.data->sVars[index]=s;
					}break;
					case 'O'://Pointer variable
					{
						int index=readInt(script, '=');
						void* o=*ParsePointerArg(script, '\n');
						script.data->oVars[index]=o;
					}break;
					case 'A'://Array variable
					{
						int temp=script.index;
						bool arrayVal=false;
						while(script.instructions[++temp]!='=')
						{
							if(script.instructions[temp]=='[')
							{
								arrayVal=true;
								break;
							}
						}
						if(arrayVal)
						{
							XPINSArray*arr=ParseArrayArg(script, '[');
							++script.index;
							int arrIndex=readInt(script, ']');
							void* val=*ParsePointerArg(script, '\n');
							clearArr(arr,arrIndex);
							arr->values[arrIndex]=val;
						}
						else
						{
							int index=readInt(script, '=');
							XPINSArray a=*ParseArrayArg(script, '\n');
							clearArr(&script.data->aVars[index]);
							script.data->aVars[index]=a;
						}
					}break;
				}
			}break;
			case '#'://User-Defined Function
			{
				int mNum=readInt(script, 'F');
				int fNum=readInt(script, '(');
				script.bindings[mNum]->
					BindFunction(fNum, script);
			}break;
			case 'X'://Built In Function
			{
				XPINSBuiltIn::ParseVoidBIF(readInt(script, '('),script);
			}break;
			case '?'://Expression
			{
				XPINSBuiltIn::ParseVoidExp(script);
			}break;
			case '@':
			{
				if(script.matchesString("@IF"))//IF STATEMENT
					ParseIf(script);
				else if(script.matchesString("@WHILE"))//WHILE LOOP
					ParseLoop(script);
				else if(script.matchesString("@EL"))//ELSE/ELIF (bypass because IF was executed)
				{
					skipBlock(script);
					while (script.currentChar()!='\n')++script.index;
				}
				else if(script.clusterURL.length()>0&&script.matchesString("@RETURN"))//RETURN statement
				{
					while (script.currentChar()!='[') ++script.index;
					++script.index;
					script.returnVal=*ParsePointerArg(script, ']');
					return;
				}
			}break;
		}
		while (script.data->Trash.size()>0)
		{
			script.data->Garbage.push_back(script.data->Trash.front());
			script.data->Trash.pop_front();
		}
		if(script.data->Garbage.size()>garbageCapacity)EmptyGarbage(*script.data);
	}
}