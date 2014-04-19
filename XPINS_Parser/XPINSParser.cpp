//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINS.h"
#include <math.h>
#include <fstream>

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;

const int kPMajor=0;
const int kPMinor=9;

void ParseCode(XPINSScriptSpace& script,int,int);

XPINSScriptSpace::XPINSScriptSpace(string script,XPINSBindings* bind)
{
	instructions=script;
	index=0;
	bindings=bind;
}

bool XPINSScriptSpace::matchesString(string testString)
{
	for(int i=0;i<testString.length();++i)
		if(i+index>=instructions.length()||instructions[index+i]!=testString[i])return false;
	return true;
}

#pragma mark Text to Number

//Read an int constant
int readInt(XPINSScriptSpace& script,char expectedEnd)
{
	int index=0;
	while(script.index<script.instructions.length()&&script.currentChar()!=expectedEnd)
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
		else if(script.currentChar()=='.')
		{
			index/=10;
			break;
		}
		else if(script.currentChar()!='0')index/=10;
		++script.index;
	}
	return index;
}

#pragma mark Argument Parsing

bool XPINSParser::ParseBoolArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	bool retVal=false;
	while (script.index<script.instructions.length()&&script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if (script.index>=script.instructions.length()) return false;

	if(script.currentChar()=='$')//Variable
	{
		script.index+=2;
		*varIndex=readInt(script, expectedEnd);
		retVal=script.data->bVars[*varIndex];
	}
	else if(script.currentChar()=='^')//constant
	{
		++script.index;
		retVal=(script.currentChar()=='T');
	}
	else if(script.currentChar()=='?')//expression
		retVal=XPINSBuiltIn::ParseBoolExp(script);
	else if(script.matchesString("#BF"))//User-defined Function
	{
		script.index+=3;
		int fNum=readInt(script,'(');
		script.bindings->BindFunction(fNum, script, &retVal);
	}
	else if(script.matchesString("XB"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		retVal=XPINSBuiltIn::ParseBoolBIF(index,script);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
	return retVal;
}
int XPINSParser::ParseIntArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	return ParseFloatArg(script, expectedEnd,varIndex);
}
float XPINSParser::ParseFloatArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	float retVal=0;
	while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//Variable
	{
		++script.index;
		char c=script.currentChar();
		++script.index;
		*varIndex=readInt(script, expectedEnd);
		retVal=c=='F'?script.data->fVars[*varIndex]:script.data->iVars[*varIndex];
	}
	else if(script.currentChar()=='^')//Constant
	{
		retVal=0;
		int fpartDig=0;
		bool fpart=false;
		bool isNeg=false;
		while(++script.index<script.instructions.length()&&script.currentChar()!=expectedEnd){
			if(fpart)fpartDig++;//record decimal place
			retVal*=10;
			if(script.currentChar()=='1')retVal+=1;
			else if(script.currentChar()=='2')retVal+=2;
			else if(script.currentChar()=='3')retVal+=3;
			else if(script.currentChar()=='4')retVal+=4;
			else if(script.currentChar()=='5')retVal+=5;
			else if(script.currentChar()=='6')retVal+=6;
			else if(script.currentChar()=='7')retVal+=7;
			else if(script.currentChar()=='8')retVal+=8;
			else if(script.currentChar()=='9')retVal+=9;
			else if(script.currentChar()=='.')fpart=true;//Start recording decimal places
			else if(script.currentChar()!='0')retVal/=10;
			if(script.currentChar()=='-')isNeg=true;
		}
		while (fpartDig-->0) {//put decimal point in correct place
			retVal/=10;
		}
		if(isNeg)retVal*=-1;
	}
	else if(script.currentChar()=='?')//Expression
	{
		retVal=script.instructions[script.index+1]=='I'?
			XPINSBuiltIn::ParseIntExp(script):
			XPINSBuiltIn::ParseFloatExp(script);
	}
	else if(script.matchesString("#IF")||script.matchesString("#FF"))
		//User-defined Function
	{
		script.index+=3;
		int fNum=readInt(script, '(');
		script.bindings->BindFunction(fNum, script, &retVal);
	}
	else if(script.matchesString("XI")||script.matchesString("XF"))
	//Built-in Function
	{
		++script.index;
		bool isFloat=script.currentChar()=='F';
		++script.index;
		int index=readInt(script, '(');
		retVal=isFloat?
			XPINSBuiltIn::ParseFloatBIF(index,script):
			XPINSBuiltIn::ParseIntBIF(index,script);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
	return retVal;
}
Vector XPINSParser::ParseVecArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	Vector *retVal=new Vector();
	while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		*varIndex=readInt(script, expectedEnd);
		*retVal=script.data->vVars[*varIndex];
	}
	else if(script.currentChar()=='^')//constant (can contain varialbes, though)
	{
		++script.index;
		if(script.matchesString("P<"))//Polar Vector
		{
			script.index+=2;
			float r=ParseFloatArg(script, ',',NULL);
			float t=ParseFloatArg(script, ',',NULL);
			float z=ParseFloatArg(script, '>',NULL);
			*retVal=Vector::PolarVector(r, t, z);
		}
		else if(script.matchesString("S<"))//Spherical Vector
		{
			script.index+=2;
			float r=ParseFloatArg(script, ',',NULL);
			float t=ParseFloatArg(script, ',',NULL);
			float p=ParseFloatArg(script, '>',NULL);
			*retVal=Vector::PolarVector(r, t ,p);
		}
		else if(script.currentChar()=='<')//rectangular vector
		{
			++script.index;
			float x=ParseFloatArg(script, ',',NULL);
			float y=ParseFloatArg(script, ',',NULL);
			float z=ParseFloatArg(script, '>',NULL);
			retVal=new Vector(x, y,z);
		}
	}
	else if(script.currentChar()=='?')
		*retVal=XPINSBuiltIn::ParseVecExp(script);
	else if(script.matchesString("#VF")){//User-defined Function
			script.index+=3;
			int fNum=readInt(script, '(');
			script.bindings->BindFunction(fNum, script, &retVal);
	}
	else if(script.matchesString("XV"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		*retVal=XPINSBuiltIn::ParseVecBIF(index,script);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
	return *retVal;
}

Matrix XPINSParser::ParseMatArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	Matrix* mat=new Matrix();
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		*varIndex=readInt(script, expectedEnd);
		mat=&script.data->mVars[*varIndex];
	}
	else if(script.currentChar()=='^')//constant (can contain varialbes, though)
	{
		while (script.currentChar()!='[')++script.index;
		size_t rows=1,cols=1;
		int temp=script.index;
		for (; script.currentChar()!='|'&&script.currentChar()!=']'; ++script.index)//Find Col Count
			if(script.currentChar()==',')++cols;
		for (script.index=temp; script.currentChar()!=']'; ++script.index)//Find Row Count
			if(script.currentChar()=='|')++rows;
		mat=new Matrix(rows,cols);//Create Matrix
		script.index=temp;
		for(int r=0;r<rows;++r)
		{
			for(int c=0;c<cols;++c)
			{
				float val;
				if(c==cols-1)
				{
					if(r==rows-1)val=ParseFloatArg(script, ']',NULL);
					else val=ParseFloatArg(script, '|',NULL);
				}
				else val=ParseFloatArg(script, ',',NULL);
				mat->SetValueAtPosition(val,r,c);
			}
		}
	}
	else if(script.currentChar()=='?')
		*mat=XPINSBuiltIn::ParseMatExp(script);
	else if(script.matchesString("#MF"))//User-defined Function
	{
			script.index+=3;
			int fNum=readInt(script, '(');
			script.bindings->BindFunction(fNum, script, mat);
	}
	else if(script.matchesString("XM"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		*mat=XPINSBuiltIn::ParseMatBIF(index,script);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
	return *mat;
}
string XPINSParser::ParseStrArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	string retVal="";
	while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='#') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		*varIndex=readInt(script, expectedEnd);
		retVal=script.data->sVars[*varIndex];
	}
	else if(script.currentChar()=='^')//User-defined Function
	{
		++script.index;
		while (++script.index<script.instructions.length())
		{
			if(script.currentChar()=='\\')
			{
				++script.index;
				switch (script.currentChar()) {
					case 'n':retVal+='\n';
						break;
					case '\t':retVal+='\t';
						break;
					case '\\':retVal+='\\';
						break;
					case '\'':retVal+='\'';
						break;
					case '\"':retVal+='\"';
						break;
					case '\r':retVal+='\r';
						break;
					case 'a':retVal+='\a';
						break;
					case 'b':retVal+='\b';
						break;
					case 'f':retVal+='\f';
						break;
					case 'v':retVal+='\v';
						break;
					case 'e':retVal+='\e';
						break;
					default:
						break;
				}
			}
			else if(script.currentChar()=='\"')break;
			else retVal+=script.currentChar();
		}
	}
	else if(script.matchesString("#SF"))//User-defined Function
	{
		script.index+=3;
		int fNum=readInt(script, '(');
		script.bindings->BindFunction(fNum, script, &retVal);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
	return retVal;
}
void* XPINSParser::ParsePointerArg(XPINSScriptSpace& script,char expectedEnd,int*varIndex)
{
	void* retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='#') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		*varIndex=readInt(script, expectedEnd);
		retVal=script.data->pVars[*varIndex];
	}
	else if(script.matchesString("#PF"))//User-defined Function
	{
		script.index+=3;
		int fNum=readInt(script, '(');
		script.bindings->BindFunction(fNum, script, &retVal);
	}
	while(script.currentChar()!=expectedEnd)++script.index;
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
	while (script.currentChar()!='$'&&script.currentChar()!='?'&&script.currentChar()!='^'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;
	if(script.instructions[script.index+1]!='B'&&script.currentChar()!='^')//Bad Script
	{
		printf("WARNING:VARIABLE NOT BOOL, Assuming False");
		skipBlock(script);
		while (script.currentChar()!='\n')++script.index;
	}
	else if(!ParseBoolArg(script, ']',NULL))//If was false
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
	while (script.currentChar()!='$'&&script.currentChar()!='?'&&script.currentChar()!='^'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;
	if(script.instructions[script.index+1]!='B'&&script.currentChar()!='^')
	{
		printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
		skipBlock(script);
		while (script.index<script.instructions.length()&&script.currentChar()!='{')++script.index;
	}
	else
	{
		int expressionIndex=script.index--;
		while (script.currentChar()!='{')++script.index;
		int loopStart=script.index;
		skipBlock(script);
		int loopStop=script.index;
		script.index=expressionIndex;
		int temp=script.index;
		while (ParseBoolArg(script, ']',NULL)) {
			ParseCode(script, loopStart, loopStop);
			script.index=temp;
		}
		skipBlock(script);
	}
}

#pragma mark Script Execution
//Check to make sure script is compatible
bool checkVersion(XPINSScriptSpace& script)
{
	for(int i=0;i<script.instructions.length();++i)
	{
		if(script.matchesString("@PARSER"))
		{
			while(script.currentChar()!='[')++script.index;
			int MAJOR=readInt(script, '.');
			int MINOR=readInt(script, ']');
			if(MAJOR!=kPMajor||MINOR>kPMinor)
			{
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			return true;
		}
	}
	cout<<"SCRIPT MISSING VERSION. FAILING";
	return false;
}
//Allocate Variable space
void allocVars(XPINSScriptSpace& script)
{
	script.index=0;
	script.data=new XPINSVarSpace();
	while(script.matchesString("@VARS"))++script.index;
	while(script.currentChar()!='B')++script.index;
	script.data->bVars.resize(readInt(script, ' '));
	while(script.currentChar()!='I')++script.index;
	script.data->iVars.resize(readInt(script, ' '));
	while(script.currentChar()!='F')++script.index;
	script.data->fVars.resize(readInt(script, ' '));
	while(script.currentChar()!='V')++script.index;
	script.data->vVars.resize(readInt(script, ' '));
	while(script.currentChar()!='M')++script.index;
	script.data->mVars.resize(readInt(script, ' '));
	while(script.currentChar()!='S')++script.index;
	script.data->sVars.resize(readInt(script, ' '));
	while(script.currentChar()!='P')++script.index;
	script.data->pVars.resize(readInt(script, ' '));
	script.index=0;
}
void XPINSParser::ParseScript(string scriptText,XPINSBindings* localBindings)
{
	XPINSScriptSpace script=*(new XPINSScriptSpace(scriptText,localBindings));
	if(!checkVersion(script))return;//Check Script Version
	//Set up scriptVars Space
	allocVars(script);
	//Run Script
	ParseCode(script,0,-1);
	//Clean up
	cout<<"\nSCRIPT FINISHED\n";
	while (localBindings->toDelete.size()>0)
	{
		delete localBindings->toDelete.front();
		localBindings->toDelete.pop_front();
	}
	delete script.data;
}

//This is the actual code parser (makes loops easier). Call ParseCode and it will call this.
void ParseCode(XPINSScriptSpace& script,int start,int stop)
{
	script.index=start;
	//Get to the start of the Script
	if(stop<0) while(!script.matchesString("@CODE"))++script.index;
	while(script.index<script.instructions.length()){
		//get to next line
		while(script.index<script.instructions.length()&&script.currentChar()!='\n')++script.index;
		if (script.index>=script.instructions.length()
			||script.matchesString("@END")
			||(stop>0&&script.index>=stop))
			break;
		++script.index;
		//Determine Key Character
		switch (script.currentChar()) {
			case '$':{//Variable
				++script.index;
				switch (script.currentChar()) {
					case 'B':{//Bool Variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->bVars[index]=ParseBoolArg(script, '\n',NULL);
					}break;
					case 'I':{//INT variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->iVars[index]=ParseIntArg(script, '\n',NULL);
					}break;
					case 'F':{//FLOAT variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->fVars[index]=ParseFloatArg(script, '\n',NULL);
					}break;
					case 'V':{//VEC variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->vVars[index]=ParseVecArg(script, '\n',NULL);
					}break;
					case 'M':{//MAT variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->mVars[index]=ParseMatArg(script, '\n',NULL);
					}break;
					case 'S':{//STR variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->sVars[index]=ParseStrArg(script, '\n',NULL);
					}break;
					case 'P':{//Pointer variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->pVars[index]=ParsePointerArg(script, '\n',NULL);
					}break;
				}
			}break;
			case '#':{
				script.index+=2;
				int fNum=readInt(script, '(');
				script.bindings->BindFunction(fNum, script, NULL);
			}break;
			case 'X':{
				script.index+=2;
				int index=readInt(script, '(');
				XPINSBuiltIn::ParseVoidBIF(index,script);
			}break;
			case '?':{//Expression
				XPINSBuiltIn::ParseVoidExp(script);
			}break;
			case '@':{
				//IF STATEMENT
				if(script.matchesString("@IF"))
					ParseIf(script);
				//WHILE LOOP
				else if(script.matchesString("@WHILE"))
					ParseLoop(script);
				//ELSE/ELIF (BYPASS because IF was executed)
				else if(script.matchesString("@EL"))
				{
					skipBlock(script);
					while (script.currentChar()!='\n')++script.index;
				}
			}break;
		}
	}
}