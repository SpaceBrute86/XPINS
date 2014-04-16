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

void ParseCode(string,XPINSVarSpace*,XPINSBindings* localBindings,int,int);

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

#pragma mark Argument Parsing

bool XPINSParser::ParseBoolArg(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	bool retVal=false;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if (i>=scriptText.length()) return false;

	if(scriptText[i]=='$')//Variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		retVal=scriptVars->bVars[*varIndex];
	}
	else if(scriptText[i]=='^')//constant
		retVal=(scriptText[i+1]=='T');
	else if(scriptText[i]=='?')//expression
		retVal=XPINSBuiltIn::ParseBoolExp(scriptText, scriptVars,localBindings, i);
	else if(scriptText[i]=='#'&&(scriptText[i+1]=='B'&&scriptText[i+2]!='F'))//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		localBindings->BindFunction(fNum, scriptText,scriptVars, i, &retVal);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='B')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=XPINSBuiltIn::ParseBoolBIF(index,scriptText, scriptVars, localBindings, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}
int XPINSParser::ParseIntArg(string scriptText,XPINSVarSpace* scriptVars, XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	return ParseFloatArg(scriptText, scriptVars, localBindings, i, expectedEnd,varIndex);
}
float XPINSParser::ParseFloatArg(string scriptText, XPINSVarSpace* scriptVars, XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//Variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		retVal=scriptText[i-1]=='F'?scriptVars->fVars[*varIndex]:scriptVars->iVars[*varIndex];
	}
	else if(scriptText[i]=='^')//Constant
	{
		retVal=0;
		int fpartDig=0;
		bool fpart=false;
		bool isNeg=scriptText[i+1]=='-';
		while(++i<scriptText.length()&&scriptText[i]!=expectedEnd){
			if(fpart)fpartDig++;//record decimal place
			retVal*=10;
			if(scriptText[i]=='1')retVal+=1;
			else if(scriptText[i]=='2')retVal+=2;
			else if(scriptText[i]=='3')retVal+=3;
			else if(scriptText[i]=='4')retVal+=4;
			else if(scriptText[i]=='5')retVal+=5;
			else if(scriptText[i]=='6')retVal+=6;
			else if(scriptText[i]=='7')retVal+=7;
			else if(scriptText[i]=='8')retVal+=8;
			else if(scriptText[i]=='9')retVal+=9;
			else if(scriptText[i]=='.')fpart=true;//Start recording decimal places
			else if(scriptText[i]!='0')retVal/=10;
		}
		while (fpartDig-->0) {//put decimal point in correct place
			retVal/=10;
		}
		if(isNeg)retVal*=-1;
	}
	else if(scriptText[i]=='?')//Expression
		retVal=scriptText[i+1]=='I'?
			XPINSBuiltIn::ParseIntExp(scriptText, scriptVars, localBindings, i):
			XPINSBuiltIn::ParseFloatExp(scriptText, scriptVars, localBindings, i);
	else if(scriptText[i]=='#'&&(scriptText[i+1]=='F'||scriptText[i+1]=='I')&&scriptText[i+2]=='F')
		//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		localBindings->BindFunction(fNum, scriptText,scriptVars, i, &retVal);
	}
	else if(scriptText[i]=='X'&&(scriptText[i+1]=='I'||scriptText[i+1]=='F'))
	//Built-in Function
	{
		bool isFloat=scriptText[i+1]=='F';
		i+=2;
		int index=readInt(scriptText, i, '(');
		retVal=isFloat?
			XPINSBuiltIn::ParseFloatBIF(index,scriptText, scriptVars, localBindings, i):
			XPINSBuiltIn::ParseIntBIF(index,scriptText, scriptVars, localBindings, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}
Vector XPINSParser::ParseVecArg(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	Vector *retVal=new Vector();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		*retVal=scriptVars->vVars[*varIndex];
	}
	else if(scriptText[i]=='^')//constant (can contain varialbes, though)
	{
		++i;
		if(scriptText[i]=='P'&&scriptText[i+1]=='<')//Polar Vector
		{
			i+=2;
			float r=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float t=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float z=ParseFloatArg(scriptText, scriptVars,localBindings, i, '>',NULL);
			*retVal=Vector::PolarVector(r, t, z);
		}
		else if(scriptText[i]=='S'&&scriptText[i+1]=='<')//Spherical Vector
		{
			i+=2;
			float r=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float t=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float p=ParseFloatArg(scriptText, scriptVars,localBindings, i, '>',NULL);
			*retVal=Vector::PolarVector(r, t ,p);
		}
		else if(scriptText[i]=='<')//rectangular vector
		{
			++i;
			float x=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float y=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
			float z=ParseFloatArg(scriptText, scriptVars,localBindings, i, '>',NULL);
			retVal=new Vector(x, y,z);
		}
	}
	else if(scriptText[i]=='?')
		*retVal=XPINSBuiltIn::ParseVecExp(scriptText, scriptVars,localBindings, i);
	else if(scriptText[i]=='#'&&scriptText[i+1]=='V'&&scriptText[i+2]=='F'){//User-defined Function
			i+=3;
			int fNum=readInt(scriptText, i, '(');
			localBindings->BindFunction(fNum, scriptText,scriptVars,  i, &retVal);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='V')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		*retVal=XPINSBuiltIn::ParseVecBIF(index,scriptText, scriptVars,localBindings, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return *retVal;
}

Matrix XPINSParser::ParseMatArg(string scriptText,XPINSVarSpace* scriptVars, XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	Matrix* mat=new Matrix();
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		mat=&scriptVars->mVars[*varIndex];
	}
	else if(scriptText[i]=='^')//constant (can contain varialbes, though)
	{
		while (scriptText[++i]!='[');
		size_t rows=1,cols=1;
		for (int j=i; scriptText[j]!='|'&&scriptText[j]!=']'; ++j)//Find Col Count
			if(scriptText[j]==',')++cols;
		for (int j=i; scriptText[j]!=']'; ++j)//Find Row Count
			if(scriptText[j]=='|')++rows;
		mat=new Matrix(rows,cols);//Create Matrix
		for(int r=0;r<rows;++r)
		{
			for(int c=0;c<cols;++c)
			{
				float val;
				if(c==cols-1)
				{
					if(r==rows-1)val=ParseFloatArg(scriptText, scriptVars,localBindings, i, ']',NULL);
					else val=ParseFloatArg(scriptText, scriptVars,localBindings, i, '|',NULL);
				}
				else val=ParseFloatArg(scriptText, scriptVars,localBindings, i, ',',NULL);
				mat->SetValueAtPosition(val,r,c);
			}
		}
	}
	else if(scriptText[i]=='?')
		*mat=XPINSBuiltIn::ParseMatExp(scriptText, scriptVars,localBindings, i);
	else if(scriptText[i]=='#'&&scriptText[i+1]=='M'&&scriptText[i+2]=='F')//User-defined Function
	{
			i+=3;
			int fNum=readInt(scriptText, i, '(');
			localBindings->BindFunction(fNum, scriptText,scriptVars,  i, mat);
	}
	else if(scriptText[i]=='X'&&scriptText[i+1]=='M')//Built-in Function
	{
		i+=2;
		int index=readInt(scriptText, i, '(');
		*mat=XPINSBuiltIn::ParseMatBIF(index,scriptText, scriptVars,localBindings, i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return *mat;
}
string XPINSParser::ParseStrArg(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	if(!varIndex)varIndex=(int*)malloc(sizeof(int));
	string retVal="";
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='#') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		retVal=scriptVars->sVars[*varIndex];
	}
	else if(scriptText[i]=='^')//User-defined Function
	{
		++i;
		while (++i<scriptText.length())
		{
			if(scriptText[i]=='\\')
				switch (scriptText[++i]) {
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
			else if(scriptText[i]=='\"')break;
			else retVal+=scriptText[i];
		}
	}
	else if(scriptText[i]=='#'&&scriptText[i+1]=='S'&&scriptText[i+2]=='F')//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		localBindings->BindFunction(fNum, scriptText,scriptVars,  i, &retVal);
	}
	while(scriptText[i]!=expectedEnd)++i;
	return retVal;
}
void* XPINSParser::ParsePointerArg(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int& i,char expectedEnd,int*varIndex)
{
	void* retVal=NULL;
	while (scriptText[i]!='$'&&scriptText[i]!='#') ++i;//Get to Key Character
	if(scriptText[i]=='$')//variable
	{
		i+=2;
		*varIndex=readInt(scriptText, i, expectedEnd);
		retVal=scriptVars->pVars[*varIndex];
	}
	else if(scriptText[i]=='#'&&scriptText[i+1]=='P'&&scriptText[i+2]=='F')//User-defined Function
	{
		i+=3;
		int fNum=readInt(scriptText, i, '(');
		localBindings->BindFunction(fNum, scriptText,scriptVars,  i, &retVal);
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
void ParseIf(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int &i)
{
	while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i+1]!='B'&&scriptText[i]!='^')//Bad Script
	{
		printf("WARNING:VARIABLE NOT BOOL, Assuming False");
		skipBlock(scriptText, i);
		while (scriptText[++i]!='\n');
	}
	else if(!ParseBoolArg(scriptText,scriptVars,localBindings, i, ']',NULL))//If was false
	{
		skipBlock(scriptText, i);
		while (scriptText[++i]!='\n');
		++i;
		if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L')
		{
			if (scriptText[i+3]=='I'&&scriptText[i+4]=='F') ParseIf(scriptText, scriptVars,localBindings, i);//ELIF
			else while (scriptText[++i]!='{');//ELSE
		}
	}
	else while (scriptText[++i]!='{');//Execute if
}
void ParseLoop(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings*localBindings, int &i)
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
		while (ParseBoolArg(scriptText, scriptVars,localBindings, expressionIndex, ']',NULL)) {
			expressionIndex=temp;
			ParseCode(scriptText, scriptVars,localBindings, loopStart, loopStop);
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
	while(scriptText[i++]!='S');
	space->sVars.resize(readInt(scriptText, i, ' '));
	while(scriptText[i++]!='P');
	space->pVars.resize(readInt(scriptText, i, ' '));
	return space;
}
void XPINSParser::ParseScript(string script,XPINSBindings* localBindings)
{
	if(!checkVersion(script))return;//Check Script Version
	//Set up scriptVars Space
	XPINSVarSpace* scriptVars=allocVars(script);
	//Run Script
	ParseCode(script, scriptVars, localBindings,0,-1);
	//Clean up
	cout<<"\nSCRIPT FINISHED\n";
	delete scriptVars;
	while (localBindings->toDelete.size()>0)
	{
		delete localBindings->toDelete.front();
		localBindings->toDelete.pop_front();
	}
}

//This is the actual code parser (makes loops easier). Call ParseCode and it will call this.
void ParseCode(string scriptText,XPINSVarSpace* scriptVars,XPINSBindings* localBindings,int start,int stop)
{
	int i=start;
	//Get to the start of the Script
	if(stop<0) while(scriptText[i]!='@'||scriptText[i+1]!='C')++i;
	while(i<scriptText.length()){
		//get to next line
		while(i<scriptText.length()&&scriptText[i++]!='\n'){}
		if (i>=scriptText.length()
			||(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D')
			||(stop>0&&i>=stop))
			break;
		//Determine Key Character
		switch (scriptText[i]) {
			case '$':{//Variable
				i+=2;
				switch (scriptText[i-1]) {
					case 'B':{//Bool Variable
						int index=readInt(scriptText, i, '=');
						i++;
						scriptVars->bVars[index]=ParseBoolArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'I':{//INT variable
						int index=readInt(scriptText, i, '=');
						i++;
						scriptVars->iVars[index]=ParseIntArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'F':{//FLOAT variable
						int index=readInt(scriptText, i, '=');
						++i;
						scriptVars->fVars[index]=ParseFloatArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'V':{//VEC variable
						int index=readInt(scriptText, i, '=');
						++i;
						scriptVars->vVars[index]=ParseVecArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'M':{//MAT variable
						int index=readInt(scriptText, i, '=');
						++i;
						scriptVars->mVars[index]=ParseMatArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'S':{//MAT variable
						int index=readInt(scriptText, i, '=');
						++i;
						scriptVars->sVars[index]=ParseStrArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
					case 'P':{//Pointer variable
						int index=readInt(scriptText, i, '=');
						++i;
						scriptVars->pVars[index]=ParsePointerArg(scriptText, scriptVars,localBindings, i, '\n',NULL);
					}break;
				}
			}break;
			case '#':{
				i+=2;
				int fNum=readInt(scriptText, i, '(');
				localBindings->BindFunction(fNum, scriptText,scriptVars, i, 0);
			}break;
			case 'X':{
				i+=2;
				int index=readInt(scriptText, i, '(');
				XPINSBuiltIn::ParseVoidBIF(index,scriptText, scriptVars,localBindings, i);
			}break;
			case '?':{//Expression
				XPINSBuiltIn::ParseVoidExp(scriptText, scriptVars, localBindings, i);
			}break;
			case '@':{
				++i;
				//IF STATEMENT
				if(scriptText[i]=='I'&&scriptText[i+1]=='F')
					ParseIf(scriptText, scriptVars,localBindings, i);
				//WHILE LOOP
				else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E')
					ParseLoop(scriptText,scriptVars,localBindings,i);
				//ELSE/ELIF (BYPASS because IF was executed)
				else if(scriptText[i]=='E'&&scriptText[i+1]=='L')
				{
					skipBlock(scriptText, i);
					while (scriptText[i]!='\n')i++;
				}
			}break;
		}
	}
}