//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;

const int kPMajor=0;
const int kPMinor=10;

void ParseCode(XPINSScriptSpace& script, int, int);

#pragma mark Variable Space and Script Space

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
char XPINSScriptSpace::currentChar()
{
	return instructions[index];
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
int readInt(XPINSScriptSpace& script,char expectedEnd)//Read an Integer from the script
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
	bool* retVal;
	while (script.index<script.instructions.length()&&script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if (script.index>=script.instructions.length()) return NULL;
	if(script.matchesString("$B"))//Variable
	{
		script.index+=2;
		size_t index=readInt(script, expectedEnd);
		retVal=&script.data->bVars[index];
	}
	else if(script.currentChar()=='~')//constant
	{
		++script.index;
		retVal=new bool(script.currentChar()=='T');
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
	while(script.currentChar()!=expectedEnd&&(script.currentChar()!=')'))++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
double* XPINSParser::ParseNumArg(XPINSScriptSpace& script,char expectedEnd)
{
	double* retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.matchesString("$N"))//Variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->nVars[index];
	}
	else if(script.currentChar()=='~')//Constant
	{
		double val=0;
		int exp=0;
		bool fpart=false;
		bool isNeg=false;
		while(++script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!=')'&&script.currentChar()!='e'&&script.currentChar()!='E')
		{
			if(fpart)--exp;//record decimal place
			val*=10;
			if(script.currentChar()=='1')val+=1;
			else if(script.currentChar()=='2')val+=2;
			else if(script.currentChar()=='3')val+=3;
			else if(script.currentChar()=='4')val+=4;
			else if(script.currentChar()=='5')val+=5;
			else if(script.currentChar()=='6')val+=6;
			else if(script.currentChar()=='7')val+=7;
			else if(script.currentChar()=='8')val+=8;
			else if(script.currentChar()=='9')val+=9;
			else if(script.currentChar()!='0')val/=10;
			if(script.currentChar()=='-')isNeg=true;
			if(script.currentChar()=='.')fpart=true;//Start recording decimal places
		}
		bool isENeg=false;
		--script.index;
		while(++script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!=')')
		{
			exp*=10;
			if(script.currentChar()=='1')exp+=1;
			else if(script.currentChar()=='2')exp+=2;
			else if(script.currentChar()=='3')exp+=3;
			else if(script.currentChar()=='4')exp+=4;
			else if(script.currentChar()=='5')exp+=5;
			else if(script.currentChar()=='6')exp+=6;
			else if(script.currentChar()=='7')exp+=7;
			else if(script.currentChar()=='8')exp+=8;
			else if(script.currentChar()=='9')exp+=9;
			else if(script.currentChar()!='0')exp/=10;
			if(script.currentChar()=='-')isENeg=true;
		}
		val=(isNeg?-1:1) * val * pow(10, (isENeg?-1:1)*exp);
		retVal=&val;
	}
	else if(script.matchesString("?N"))//Expression
	{
		retVal=new double(XPINSBuiltIn::ParseNumExp(script));
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
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
Vector* XPINSParser::ParseVecArg(XPINSScriptSpace& script,char expectedEnd)
{
	Vector *retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.matchesString("$V"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->vVars[index];
	}
	else if(script.currentChar()=='~')//Constant
	{
		if(script.matchesString("~<"))//Cartesian vector
		{
			script.index+=2;
			double x=*ParseNumArg(script, ',');
			double y=*ParseNumArg(script, ',');
			double z=*ParseNumArg(script, '>');
			retVal=new Vector(x,y ,z);
		}
		else if(script.matchesString("~P<"))//Polar Vector
		{
			script.index+=3;
			double r=*ParseNumArg(script, ',');
			double t=*ParseNumArg(script, ',');
			double z=*ParseNumArg(script, '>');
			Vector val=Vector::PolarVector(r, t, z);
			retVal=&val;
		}
		else if(script.matchesString("~S<"))//Spherical Vector
		{
			script.index+=3;
			double r=*ParseNumArg(script, ',');
			double t=*ParseNumArg(script, ',');
			double p=*ParseNumArg(script, '>');
			Vector val=Vector::SphericalVector(r, t ,p);
			retVal=&val;
		}
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
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')'&&script.currentChar()!='\n')++script.index;
	if(script.instructions[script.index+1]=='?'&&script.currentChar()!='\n')script.index+=2;
	return retVal;
}
Matrix *XPINSParser::ParseMatArg(XPINSScriptSpace& script,char expectedEnd)
{
	Matrix* mat=new Matrix();
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.matchesString("$M"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		mat=&script.data->mVars[index];
	}
	else if(script.matchesString("~["))//constant (can contain varialbes, though)
	{
		size_t rows=1,cols=1;
		script.index+=2;
		int temp=script.index;
		for (;script.currentChar()!='|'&&script.currentChar()!=']'; ++script.index)//Find Column Count
		{
			if(script.currentChar()=='(')//Skip Parenthesis blocks
			{
				++script.index;
				for (int count=1; count>0; ++script.index) {
					if(script.currentChar()=='(')++count;
					else if(script.currentChar()==')')--count;
				}
			}
			if(script.currentChar()==',')++cols;
		}
		for (script.index=temp; script.currentChar()!=']'; ++script.index)//Find Row Count
		{
			if(script.currentChar()=='(')//Skip Parenthesis blocks
			{
				++script.index;
				for (int count=1; count>0; ++script.index) {
					if(script.currentChar()=='(')++count;
					else if(script.currentChar()==')')--count;
				}
			}
			if(script.currentChar()=='|')++rows;
		}
		mat=new Matrix(rows,cols);//Create Matrix
		script.index=temp;
		for(int r=0;r<rows;++r)
		{
			for(int c=0;c<cols;++c)
			{
				double val;
				if(c==cols-1)
				{
					if(r==rows-1)val=*ParseNumArg(script, ']');
					else val=*ParseNumArg(script, '|');
				}
				else val=*ParseNumArg(script, ',');
				mat->SetValueAtPosition(val,r,c);
			}
		}
	}
	else if(script.matchesString("?M"))
	{
		mat=new Matrix(XPINSBuiltIn::ParseMatExp(script));
	}
	else if(script.matchesString("#MM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		mat=(Matrix*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("XM"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		mat=new Matrix(XPINSBuiltIn::ParseMatBIF(index,script));
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//Array
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		mat=((Matrix*)arr->values[arrIndex]);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return mat;
}
Polynomial* XPINSParser::ParsePolyArg(XPINSScriptSpace& script, char expectedEnd)
{
	Polynomial* poly=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.matchesString("$P"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		poly=&script.data->pVars[index];
	}
	else if(script.matchesString("~("))//constant (can contain varialbes, though)
	{
		while (script.currentChar()!='(')++script.index;
		++script.index;
		vector<Polynomial::Monomial> mons=vector<Polynomial::Monomial>();
		for (int i=0;true;++i)
		{
			mons.resize(i+1);
			mons[i].coeff=*ParseNumArg(script, '_');
			mons[i].exponents=vector<unsigned int>();
			while (script.currentChar()=='_')
			{
				++script.index;
				unsigned int expIndex=0;
				switch (script.currentChar())
				{
					case 'X':
					case 'x':
						expIndex=1;
						break;
					case 'Y':
					case 'y':
						expIndex=2;
						break;
					case 'Z':
					case 'z':
						expIndex=3;
						break;
					case 'T':
					case 't':
						expIndex=4;
						break;
				}
				++script.index;
				if(script.currentChar()=='+'||script.currentChar()=='-'||script.currentChar()==')')
				{
					if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
					mons[i].exponents[expIndex-1]+=1;
					break;
				}
				if(script.currentChar()=='_')
				{
					if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
					mons[i].exponents[expIndex-1]+=1;
				}
				if(expIndex!=0)
				{
					if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
					mons[i].exponents[expIndex-1]+=readInt(script, '_');
				}
			}
			if(script.currentChar()==')')break;
		}
		poly=new Polynomial(mons);
		++script.index;
	}
	else if(script.matchesString("?P"))
	{
		poly=new Polynomial(XPINSBuiltIn::ParsePolyExp(script));
	}
	else if(script.matchesString("#PM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		poly=(Polynomial*)script.bindings[mNum]->BindFunction(fNum, script);
	}
	else if(script.matchesString("XP"))//Built-in Function NO POLY BIFs YET
	{
		script.index+=2;
		int index=readInt(script, '(');
		poly=new Polynomial(XPINSBuiltIn::ParsePolyBIF(index,script));
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		XPINSArray* arr= ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		poly=((Polynomial*)arr->values[arrIndex]);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return poly;
}
string *XPINSParser::ParseStrArg(XPINSScriptSpace& script,char expectedEnd)
{
	string *retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='#') ++script.index;//Get to Key Character
	if(script.matchesString("$S"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		retVal=&script.data->sVars[index];
	}
	else if(script.currentChar()=='~')//User-defined Function
	{
		retVal=new string();
		++script.index;
		while (++script.index<script.instructions.length())
		{
			if(script.currentChar()=='\\')
			{
				++script.index;
				switch (script.currentChar()) {
					case 'n':*retVal+='\n';
						break;
					case '\t':*retVal+='\t';
						break;
					case '\\':*retVal+='\\';
						break;
					case '\'':*retVal+='\'';
						break;
					case '\"':*retVal+='\"';
						break;
					case '\r':*retVal+='\r';
						break;
					case 'a':*retVal+='\a';
						break;
					case 'b':*retVal+='\b';
						break;
					case 'f':*retVal+='\f';
						break;
					case 'v':*retVal+='\v';
						break;
					case 'e':*retVal+='\e';
						break;
					default:
						break;
				}
			}
			else if(script.currentChar()=='\"')break;
			else *retVal+=script.currentChar();
		}
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
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
void** XPINSParser::ParsePointerArg(XPINSScriptSpace& script,char expectedEnd, char* type)
{
	void** retVal=NULL;
	if(!type)type=new char();
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	*type=readType(script);
	switch(*type)
	{
		case 'O':
			if(script.matchesString("$O"))//variable
			{
				script.index+=2;
				int index=readInt(script, expectedEnd);
				retVal=&script.data->oVars[index];
				*type='O';
			}
			else if(script.matchesString("#PM"))//User-defined Function
			{
				int mNum=readInt(script,'F');
				int fNum=readInt(script,'(');
				retVal=(void**)script.bindings[mNum]->BindFunction(fNum, script);
				*type='O';
			}
			else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
			{
				XPINSArray* arr= ParseArrayArg(script, '[');
				++script.index;
				*type='A';
				if(script.currentChar()==':')retVal=(void**)&arr;
				else
				{
					int arrIndex=readInt(script, ']');
					retVal=(void**)&(arr->values[arrIndex]);
					*type=arr->types[arrIndex];
				}
			}
			break;
		case 'B':
			retVal=new void*(ParseBoolArg(script, expectedEnd));
			break;
		case 'N':
			retVal=new void*(ParseNumArg(script, expectedEnd));
			break;
		case 'V':
			retVal=new void*(ParseVecArg(script, expectedEnd));
			break;
		case 'M':
			retVal=new void*(ParseMatArg(script, expectedEnd));
			break;
		case 'P':
			retVal=new void*(ParsePolyArg(script, expectedEnd));
			break;
		case 'S':
			retVal=new void*(ParseStrArg(script, expectedEnd));
			break;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
XPINSArray* XPINSParser::ParseArrayArg(XPINSScriptSpace& script,char expectedEnd)
{
	XPINSArray* retVal = nullptr;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='#') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
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
		while (script.currentChar()!='{')++script.index;
		size_t size=1;
		int temp=script.index;
		for (;script.currentChar()!='}'; ++script.index)//Find Col Count
			if(script.currentChar()==',')++size;
		retVal=new XPINSArray();
		retVal->values.resize(size);
		script.index=temp;
		for(int i=0;i<size;++i)
		{
			char type='O';
			if(i==size-1)
				retVal->values[i]=ParsePointerArg(script, '}',&type);
			else
				retVal->values[i]=ParsePointerArg(script, ',',&type);
			retVal->types[i]=type;
		}
	}
	else if(script.matchesString("#AM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		XPINSArray* arr=new XPINSArray();
		retVal=(XPINSArray*)script.bindings[mNum]->BindFunction(fNum, script);
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
		++script.index;
		if(script.matchesString("@EL"))
		{
			script.index+=3;
			if (script.matchesString("IF")) ParseIf(script);//ELIF
			else while (script.currentChar()!='{')++script.index;//ELSE
		}
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
}
void XPINSParser::ParseScript(string scriptText,vector<XPINSBindings*> bindings)
{
	XPINSScriptSpace script=XPINSScriptSpace(scriptText,bindings);
	if(!checkVersion(script))return;//Check Script Version
	allocVars(script);//Set up scriptVars Space
	ParseCode(script,0,-1);//Run Script
	//Clean up
	delete script.data;
	while (script.toDelete.size()>0)
	{
		delete script.toDelete.front();
		script.toDelete.pop_front();
	}
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
						script.data->bVars[index]=*ParseBoolArg(script, '\n');
					}break;
					case 'N'://NUM variable
					{
						int index=readInt(script, '=');
						script.data->nVars[index]=*ParseNumArg(script, '\n');
					}break;
					case 'V'://VEC variable
					{
						int index=readInt(script, '=');
						script.data->vVars[index]=*ParseVecArg(script, '\n');
					}break;
					case 'M'://MAT variable
					{
						int index=readInt(script, '=');
						script.data->mVars[index]=*ParseMatArg(script, '\n');
					}break;
					case 'P'://POLY variable
					{
						int index=readInt(script, '=');
						Polynomial poly=*ParsePolyArg(script, '\n');
						script.data->pVars[index]=poly;
					}break;
					case 'S'://STR variable
					{
						int index=readInt(script, '=');
						script.data->sVars[index]=*ParseStrArg(script, '\n');
					}break;
					case 'O'://Pointer variable
					{
						int index=readInt(script, '=');
						script.data->oVars[index]=*ParsePointerArg(script, '\n');
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
							arr->values[arrIndex]=*ParsePointerArg(script, '\n');
						}
						else
						{
							int index=readInt(script, '=');
							script.data->aVars[index]=*ParseArrayArg(script, '\n');
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
			}break;
		}
	}
}