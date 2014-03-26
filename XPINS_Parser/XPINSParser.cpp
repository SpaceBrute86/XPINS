//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSParser.h"
#include "XPINSBridge.h"
#include <math.h>


using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;
const int kPMajor=0;
const int kPMinor=9;

void ParseCode(string,XPINSDataRef,int,int);

#pragma mark Text to Number

//Read an int constant
int readInt(string scriptText,int& i,char expectedEnd)
{
	int index=0;
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd)
	{
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]=='.')
		{
			index/=10;
			break;
		}
		else if(scriptText[i]!='0')index/=10;
		++i;
	}
	return index;
}

//read a float constant
float readFloat(string scriptText,int& i,char expectedEnd)
{
	int index=0;
	int fpartDig=0;
	bool fpart=false;
	bool isNeg=scriptText[i]=='-';
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd){
		if(fpart)fpartDig++;//record decimal place
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]=='.')fpart=true;//Start recording decimal places
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	while (fpartDig) {//put decimal point in correct place
		index/=10;
		fpartDig--;
	}
	if(isNeg)index*=-1;
	return index;
}

#pragma mark Argument Parsing

bool XPINSParser::ParseBoolArg(string scriptText,XPINSDataRef data,int& i,char expectedEnd)
{
	bool retVal=false;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if (i>=scriptText.length()) return false;

	if(scriptText[i]=='$')//Variable
	{
		i+=2;
		int index=readInt(scriptText, i, expectedEnd);
		retVal=data.scriptVars->bVars[index];
	}
	else if(scriptText[i]=='^')//constant
		retVal=(scriptText[i+1]=='T');
	else if(scriptText[i]=='?')//expression
		retVal=XPINSBuiltIn::ParseBoolExp(scriptText, data, i);
	else if(scriptText[i]=='#'&&(scriptText[i+1]=='B'&&scriptText[i+2]!='F'))//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		XPINSBridge::BridgeFunction(fNum, scriptText,data, i, &retVal);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='B')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=XPINSBuiltIn::ParseBoolBIF(index,scriptText, data, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}
int XPINSParser::ParseIntArg(string scriptText,XPINSDataRef data,int& i,char expectedEnd)
{
	return ParseFloatArg(scriptText, data, i, expectedEnd);
}
float XPINSParser::ParseFloatArg(string scriptText, XPINSDataRef data,int& i,char expectedEnd)
{
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//Variable
	{
		i+=2;
		int index=readInt(scriptText, i, expectedEnd);
		retVal=scriptText[i-1]=='F'?data.scriptVars->fVars[index]:data.scriptVars->iVars[index];
	}
	else if(scriptText[i]=='^')//Constant
	{
		++i;
		retVal=readFloat(scriptText, i, expectedEnd);
	}
	else if(scriptText[i]=='?')//Expression
		retVal=scriptText[i+1]=='I'?
			XPINSBuiltIn::ParseIntExp(scriptText, data, i):
			XPINSBuiltIn::ParseFloatExp(scriptText, data, i);
	else if(scriptText[i]=='#'&&(scriptText[i+1]=='F'||scriptText[i+1]=='I')&&scriptText[i+2]=='F')
		//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		XPINSBridge::BridgeFunction(fNum, scriptText,data,  i, &retVal);
	}
	else if(scriptText[i]=='X'&&(scriptText[i+1]=='I'||scriptText[i+1]=='F'))
	//Built-in Function
	{
		bool isFloat=scriptText[i+1]=='F';
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=isFloat?
			XPINSBuiltIn::ParseFloatBIF(index,scriptText, data, i):
			XPINSBuiltIn::ParseIntBIF(index,scriptText, data, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}
Vector XPINSParser::ParseVecArg(string scriptText,XPINSDataRef data,int& i,char expectedEnd)
{
	Vector retVal=Vector();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		int index=readInt(scriptText, i, expectedEnd);
		retVal=data.scriptVars->vVars[index];
	}
	else if(scriptText[i]=='^')//constant (can contain varialbes, though)
	{
		++i;
		if(scriptText[i]=='P'&&scriptText[i+1]=='<')//Polar Vector
		{
			i+=2;
			float r=ParseFloatArg(scriptText, data, i, ',');
			float t=ParseFloatArg(scriptText, data, i, ',');
			float z=ParseFloatArg(scriptText, data, i, '>');
			retVal=Vector::PolarVector(r, t, z);
		}
		else if(scriptText[i]=='S'&&scriptText[i+1]=='<')//Spherical Vector
		{
			i+=2;
			float r=ParseFloatArg(scriptText, data, i, ',');
			float t=ParseFloatArg(scriptText, data, i, ',');
			float p=ParseFloatArg(scriptText, data, i, '>');
			retVal=Vector::PolarVector(r, t ,p);
		}
		else if(scriptText[i]=='<')//rectangular vector
		{
			++i;
			float x=ParseFloatArg(scriptText, data, i, ',');
			float y=ParseFloatArg(scriptText, data, i, ',');
			float z=ParseFloatArg(scriptText, data, i, '>');
			retVal=Vector(x, y,z);
		}
	}
	else if(scriptText[i]=='?')
		retVal=XPINSBuiltIn::ParseVecExp(scriptText, data, i);
	else if(scriptText[i]=='#'&&scriptText[i+1]=='V'&&scriptText[i+2]=='F'){//User-defined Function
			i+=3;
			int fNum=readInt(scriptText, i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,data,  i, &retVal);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='V')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=XPINSBuiltIn::ParseVecBIF(index,scriptText, data, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}

Matrix XPINSParser::ParseMatArg(string scriptText,XPINSDataRef data,int& i,char expectedEnd)
{
	Matrix retVal=Matrix();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		int index=readInt(scriptText, i, expectedEnd);
		retVal=data.scriptVars->mVars[index];
	}
	else if(scriptText[i]=='^')//constant (can contain varialbes, though)
	{
		while (scriptText[++i]!='[');
		size_t rows=1,cols=1;
		for (int j=i; scriptText[j]!=';'&&scriptText[j]!=']'; ++j)//Find Col Count
			if(scriptText[j]==',')++cols;
		for (int j=i; scriptText[j]!=']'; ++j)//Find Row Count
			if(scriptText[j]==';')++rows;
		retVal=Matrix(rows,cols);//Create Matrix
		int r=0,c=0;
		while (scriptText[i]!=']')//Find Values
		{
			if(c==cols-1)
			{
				if(r==rows-1)retVal.SetValueAtPosition(ParseFloatArg(scriptText, data, i, ']'),r,c);
				else retVal.SetValueAtPosition(ParseFloatArg(scriptText, data, i, ';'),r,c);
				++r;
				c=-1;
			}
			else retVal.SetValueAtPosition(ParseFloatArg(scriptText, data, i, ','),r,c);
			++c;
			++i;
		}
	}
	else if(scriptText[i]=='?')
		retVal=XPINSBuiltIn::ParseMatExp(scriptText, data, i);
	else if(scriptText[i]=='#'&&scriptText[i+1]=='M'&&scriptText[i+2]=='F')//User-defined Function
	{
			i+=3;
			int fNum=readInt(scriptText, i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,data,  i, &retVal);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='M')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=XPINSBuiltIn::ParseMatBIF(index,scriptText, data, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}

void* XPINSParser::ParsePointerArg(string scriptText,XPINSDataRef data,int& i,char expectedEnd){
	void* retVal=NULL;
	while (scriptText[i]!='$'&&scriptText[i]!='#') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		int index=readInt(scriptText, i, expectedEnd);
		retVal=data.scriptVars->pVars[index];
	}
	else if(scriptText[i]=='#'&&scriptText[i+1]=='P'&&scriptText[i+2]=='F')//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		XPINSBridge::BridgeFunction(fNum, scriptText,data,  i, &retVal);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}

#pragma mark Conditionals and Loops
void skipBlock(string scriptText, int &i)
{
	while (scriptText[i]!='{')++i;
	int num=1;
	//skip if block
	while(num>0){
		if(++i>=scriptText.length())break;
		if(scriptText[i]=='{')
			++num;
		else if(scriptText[i]=='}')
			num--;
	}
}
void ParseIf(string scriptText,XPINSDataRef data,int &i)
{
	while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i+1]!='B'&&scriptText[i]!='^')//Bad Script
	{
		printf("WARNING:VARIABLE NOT BOOL, Assuming False");
		skipBlock(scriptText, i);
		while (scriptText[++i]!='\n');
	}
	else if(!ParseBoolArg(scriptText,data, i, ']'))//If was false
	{
		skipBlock(scriptText, i);
		while (scriptText[++i]!='\n');
		++i;
		if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L')
		{
			if (scriptText[i+3]=='I'&&scriptText[i+4]=='F') ParseIf(scriptText, data, i);//ELIF
			else while (scriptText[++i]!='{');//ELSE
		}
	}
	else while (scriptText[++i]!='{');//Execute if
}
void ParseLoop(string scriptText,XPINSDataRef data, int &i)
{
	while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') i++;
	if(scriptText[i+1]!='B'&&scriptText[i]!='^')
	{
		printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
		skipBlock(scriptText, i);
		while (scriptText[++i]!='\n');
	}
	else
	{
		int expressionIndex=i--;
		while (scriptText[++i]!='{');
		int loopStart=i;
		skipBlock(scriptText, i);
		int loopStop=i;
		int temp=expressionIndex;
		while (ParseBoolArg(scriptText, data, expressionIndex, ']')) {
			expressionIndex=temp;
			ParseCode(scriptText, data, loopStart, loopStop);
		}
	}
}

#pragma mark Script Execution

//Check to make sure script is compatible
bool checkVersion(string script)
{
	for(int i=0;i<script.length();i++)
	{
		if(script[i]=='@'&&script[i+1]=='P'&&script[i+2]=='A'&&script[i+3]=='R'&&script[i+4]=='S'&&script[i+5]=='E'&&script[i+6]=='R')
		{
			while(script[++i]!='[');
			int MAJOR=readInt(script, i, '.');
			int MINOR=readInt(script, i, ']');
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
XPINSVarSpace* allocVars(string scriptText)
{
	XPINSVarSpace* space=new XPINSVarSpace();
	int i=0;
	 while(scriptText[i]!='@'||scriptText[i+1]!='V')++i;
	while(scriptText[i++]!='B');
	space->bVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='I');
	space->iVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='F');
	space->fVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='V');
	space->vVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='M');
	space->mVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='P');
	space->pVars.resize(readInt(scriptText, i, ' '));
	return space;
}
void XPINSParser::ParseScript(string script,XPINSParams* params)
{
	if(!checkVersion(script))return;//Check Script Version
	//Set up Data Space
	XPINSParser::XPINSDataRef data=XPINSParser::XPINSDataRef();
	data.scriptParams=params;
	data.scriptVars=allocVars(script);
	//Run Script
	ParseCode(script, data,0,-1);
	//Clean up
	delete data.scriptVars;
}

//This is the actual code parser (makes loops easier). Call ParseCode and it will call this.
void ParseCode(string scriptText,XPINSDataRef data,int start,int stop)
{
	int i=start;
	//Get to the start of the Script
	if(stop<0) while(scriptText[i]!='@'||scriptText[i+1]!='C')++i;
	while(true){
		//get to next line
		while(i<scriptText.length()&&scriptText[i++]!='\n'){}
		if (i>=scriptText.length())break;
		if((scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D')||
		   (stop>0&&i>=stop))break;
		//Determine Key Character
		switch (scriptText[i]) {
			case '$':{//Variable
				i+=2;
				switch (scriptText[i-1]) {
					case 'B':{//Bool Variable
						int index=readInt(scriptText, i, '=');
						i++;
						data.scriptVars->bVars[index]=ParseBoolArg(scriptText, data, i, '\n');
					}break;
					case 'I':{//INT variable
						int index=readInt(scriptText, i, '=');
						i++;
						data.scriptVars->iVars[index]=ParseIntArg(scriptText, data, i, '\n');
					}break;
					case 'F':{//FLOAT variable
						int index=readInt(scriptText, i, '=');
						++i;
						data.scriptVars->fVars[index]=ParseFloatArg(scriptText, data, i, '\n');
					}break;
					case 'V':{//VEC variable
						int index=readInt(scriptText, i, '=');
						++i;
						data.scriptVars->vVars[index]=ParseVecArg(scriptText, data, i, '\n');
					}break;
					case 'M':{//MAT variable
						int index=readInt(scriptText, i, '=');
						++i;
						data.scriptVars->mVars[index]=ParseMatArg(scriptText, data, i, '\n');
					}break;
					case 'P':{//Pointer variable
						int index=readInt(scriptText, i, '=');
						++i;
						data.scriptVars->pVars[index]=ParsePointerArg(scriptText, data, i, '\n');
					}break;
				}
			}break;
			case '#':{
				i+=2;
				int fNum=readInt(scriptText, i, '(');
				XPINSBridge::BridgeFunction(fNum, scriptText,data,  i, 0);
			}break;
			case 'X':{
				i+=2;
				int index=readInt(scriptText, i, '(');
				XPINSBuiltIn::ParseVoidBIF(index,scriptText, data, i);
			}break;
			case '@':{
				++i;
				//IF STATEMENT
				if(scriptText[i]=='I'&&scriptText[i+1]=='F'){
					ParseIf(scriptText, data, i);
				}
				//WHILE LOOP
				else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E'){
					ParseLoop(scriptText,data,i);
				}
				//ELSE/ELIF (BYPASS because IF was executed)
				else if(scriptText[i]=='E'&&scriptText[i+1]=='L'){
					skipBlock(scriptText, i);
					while (scriptText[i]!='\n')i++;
				}
			}break;
		}
		if(i>=scriptText.length())break;
	}
}