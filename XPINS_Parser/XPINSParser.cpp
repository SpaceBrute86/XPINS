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

void ParseCode(XPINSScriptSpace& script,int,int);

XPINSScriptSpace::XPINSScriptSpace(string script,vector<XPINSBindings*> bind)
{
	instructions=script;
	index=0;
	bindings=bind;
}

bool XPINSScriptSpace::matchesString(string testString)
{
	for(int i=0;i<testString.length();++i)
		if(i+index>=instructions.length()||
		   instructions[index+i]!=testString[i])
			return false;
	return true;
}

char readType(XPINSScriptSpace& script){
	if(script.instructions[script.index]!='~')
		return script.instructions[script.index+1]=='A'?'P':script.instructions[script.index+1];
	else switch(script.instructions[script.index+1]){
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
		case '{':
			return'A';
		default:return'N';
	}
}


#pragma mark Text to Number


//Read an int constant
int readInt(XPINSScriptSpace& script,char expectedEnd)
{
	int index=0;
	while(script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!='['&&script.currentChar()!='(')
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

bool* XPINSParser::ParseBoolArg(XPINSScriptSpace& script,char expectedEnd)
{
	bool* retVal=NULL;
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
		bool val=(script.currentChar()=='T');
		retVal=&val;
	}
	else if(script.currentChar()=='?')//expression
	{
		bool val=XPINSBuiltIn::ParseBoolExp(script);
		retVal=&val;
	}
	else if(script.matchesString("#B"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		script.bindings[mNum]->BindFunction(fNum, script,(void**) &retVal);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		vector<void*> arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((bool*)arr[arrIndex]);
		
	}
	else if(script.matchesString("XB"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		bool val=XPINSBuiltIn::ParseBoolBIF(index,script);
		retVal=&val;
	}
	while(script.currentChar()!=expectedEnd&&(script.currentChar()!=')'))++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}

double* XPINSParser::ParseNumArg(XPINSScriptSpace& script,char expectedEnd)
{
	double* retVal=0;
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
		while(++script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!=')'&&script.currentChar()!='e'&&script.currentChar()!='E'){
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
		while(++script.index<script.instructions.length()&&script.currentChar()!=expectedEnd&&script.currentChar()!=')'){
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
		exp*=isENeg?-1:1;
		if(isNeg)val*=-1;
		val*=pow(10, exp);
		retVal=&val;
	}
	else if(script.currentChar()=='?')//Expression
	{
		double val=XPINSBuiltIn::ParseNumExp(script);
		retVal=&val;
	}
	else if(script.matchesString("#NM"))
		//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		script.bindings[mNum]->BindFunction(fNum, script, (void**)&retVal);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		vector<void*> arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=(double*)arr[arrIndex];
	}
	else if(script.matchesString("XN"))
	//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		double val=XPINSBuiltIn::ParseNumBIF(index,script);
		retVal=&val;
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
	else if(script.currentChar()=='~')//constant (can contain varialbes, though)
	{
		++script.index;
		if(script.matchesString("P<"))//Polar Vector
		{
			script.index+=2;
			double r=*ParseNumArg(script, ',');
			double t=*ParseNumArg(script, ',');
			double z=*ParseNumArg(script, '>');
			Vector val=Vector::PolarVector(r, t, z);
			retVal=&val;
		}
		else if(script.matchesString("S<"))//Spherical Vector
		{
			script.index+=2;
			double r=*ParseNumArg(script, ',');
			double t=*ParseNumArg(script, ',');
			double p=*ParseNumArg(script, '>');
			Vector val=Vector::PolarVector(r, t ,p);
			retVal=&val;
		}
		else if(script.currentChar()=='<')//rectangular vector
		{
			++script.index;
			double x=*ParseNumArg(script, ',');
			double y=*ParseNumArg(script, ',');
			double z=*ParseNumArg(script, '>');
			retVal=new Vector(x, y,z);
		}
	}
	else if(script.currentChar()=='?')
		*retVal=XPINSBuiltIn::ParseVecExp(script);
	else if(script.matchesString("#VM")){//User-defined Function
			script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		script.bindings[mNum]->BindFunction(fNum, script, (void**)&retVal);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		vector<void*> arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((Vector*)arr[arrIndex]);
	}
	else if(script.matchesString("XV"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		*retVal=XPINSBuiltIn::ParseVecBIF(index,script);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}

Matrix *XPINSParser::ParseMatArg(XPINSScriptSpace& script,char expectedEnd)
{
	Matrix* mat=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	if(script.matchesString("$M"))//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		mat=&script.data->mVars[index];
	}
	else if(script.currentChar()=='~')//constant (can contain varialbes, though)
	{
		while (script.currentChar()!='[')++script.index;
		size_t rows=1,cols=1;
		int temp=script.index;
		for (; script.currentChar()!='|'&&script.currentChar()!=']'; ++script.index)//Find Col Count
		{
			if(script.currentChar()=='(')
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
			if(script.currentChar()=='(')
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
	else if(script.currentChar()=='?')
	{
		Matrix val=XPINSBuiltIn::ParseMatExp(script);
		mat=&val;
	}
	else if(script.matchesString("#MM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		script.bindings[mNum]->BindFunction(fNum, script, (void**)&mat);
	}
	else if(script.matchesString("XM"))//Built-in Function
	{
		script.index+=2;
		int index=readInt(script, '(');
		*mat=XPINSBuiltIn::ParseMatBIF(index,script);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		vector<void*> arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		mat=((Matrix*)arr[arrIndex]);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return mat;
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
		string str="";
		retVal=&str;
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
		script.bindings[mNum]->BindFunction(fNum, script, (void**)&retVal);
	}
	else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
	{
		vector<void*> arr= *ParseArrayArg(script, '[');
		++script.index;
		int arrIndex=readInt(script, ']');
		retVal=((string*)arr[arrIndex]);
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
void** XPINSParser::ParsePointerArg(XPINSScriptSpace& script,char expectedEnd)
{
	void** retVal=NULL;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
	switch(readType(script))
	{
		case 'P':{
			if(script.matchesString("$P"))//variable
			{
				script.index+=2;
				int index=readInt(script, expectedEnd);
				retVal=&script.data->pVars[index];
			}
			else if(script.matchesString("#PM"))//User-defined Function
			{
				int mNum=readInt(script,'F');
				int fNum=readInt(script,'(');
				script.bindings[mNum]->BindFunction(fNum, script, (void**)&retVal);
			}
			else if(script.matchesString("#A")||script.matchesString("$A"))//User-defined Function
			{
				vector<void*>* arr= ParseArrayArg(script, '[');
				++script.index;
				if(script.currentChar()==':')retVal=(void**)&arr;
				else
				{
					int arrIndex=readInt(script, ']');
					retVal=(void**)((*arr)[arrIndex]);
				}
			}
		}break;
		case 'B':{
			bool* val=ParseBoolArg(script, expectedEnd);
			retVal=(void**)&val;
		}break;
		case 'N':{
			double* val=ParseNumArg(script, expectedEnd);
			retVal=(void**)&val;
		}break;
		case 'V':{
			Vector* val=ParseVecArg(script, expectedEnd);
			retVal=(void**)&val;
		}break;
		case 'M':{
			Matrix* val=ParseMatArg(script, expectedEnd);
			retVal=(void**)&val;
		}break;
		case 'S':{
			string* val=ParseStrArg(script, expectedEnd);
			retVal=(void**)&val;
		}break;
	}
	while(script.currentChar()!=expectedEnd&&script.currentChar()!=')')++script.index;
	if(script.instructions[script.index+1]=='?')script.index+=2;
	return retVal;
}
vector<void*>* XPINSParser::ParseArrayArg(XPINSScriptSpace& script,char expectedEnd)
{
	vector<void*>* retVal=new vector<void*>;
	while (script.currentChar()!='$'&&script.currentChar()!='~'&&script.currentChar()!='#') ++script.index;//Get to Key Character
	if(script.currentChar()=='$')//variable
	{
		script.index+=2;
		int index=readInt(script, expectedEnd);
		vector<void*>* arr=&script.data->aVars[index];
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
			else retVal=(vector<void*>*)((*arr)[arrIndex]);
		}
		else retVal=arr;
	}
	else if(script.currentChar()=='~')//User-defined Function
	{
		while (script.currentChar()!='{')++script.index;
		size_t size=1;
		int temp=script.index;
		for (;script.currentChar()!='}'; ++script.index)//Find Col Count
			if(script.currentChar()==',')++size;
		retVal->resize(size);
		script.index=temp;
		for(int i=0;i<size;++i)
		{
			if(i==size-1)
				(*retVal)[i]=ParsePointerArg(script, '}');
			else
				(*retVal)[i]=ParsePointerArg(script, ',');
		}
	}
	else if(script.matchesString("#AM"))//User-defined Function
	{
		script.index+=3;
		int mNum=readInt(script,'F');
		int fNum=readInt(script,'(');
		vector<void*>* arr=new vector<void*>();
		script.bindings[mNum]->BindFunction(fNum, script, (void**)&arr);
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
			else retVal=(vector<void*>*)((*arr)[arrIndex]);
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
	if(script.instructions[script.index+1]!='B'&&script.currentChar()!='~')//Bad Script
	{
		printf("WARNING:VARIABLE NOT BOOL, Assuming False");
		skipBlock(script);
		while (script.currentChar()!='\n')++script.index;
	}
	else if(!*ParseBoolArg(script, ']'))//If was false
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
	if(script.instructions[script.index+1]!='B'&&script.currentChar()!='~')
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
		while (*ParseBoolArg(script, ']')) {
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
	//BOOLS
	while(script.currentChar()!='B')++script.index;
	size_t bsize=readInt(script, ' ');
	script.data->bVars=(bool*)malloc(sizeof(bool)*bsize);
	for (int i=0; i<bsize; ++i)script.data->bVars[i]=false;
	//NUMS
	while(script.currentChar()!='N')++script.index;
	size_t nsize=readInt(script, ' ');
	script.data->nVars=(double*)malloc(sizeof(double)*nsize);
	for (int i=0; i<nsize; ++i)script.data->nVars[i]=0.0;
	//VECs
	while(script.currentChar()!='V')++script.index;
	size_t vsize=readInt(script, ' ');
	script.data->vVars=(Vector*)malloc(sizeof(Vector)*vsize);
	for (int i=0; i<vsize; ++i)script.data->vVars[i]=*new Vector();
	//MATs
	while(script.currentChar()!='M')++script.index;
	size_t msize=readInt(script, ' ');
	script.data->mVars=(Matrix*)malloc(sizeof(Matrix)*msize);
	for (int i=0; i<msize; ++i)script.data->mVars[i]=*new Matrix();
	//STRs
	while(script.currentChar()!='S')++script.index;
	size_t ssize=readInt(script, ' ');
	script.data->sVars=(string*)malloc(sizeof(string)*ssize);
	for (int i=0; i<ssize; ++i)script.data->sVars[i]="";
	//Pointers
	while(script.currentChar()!='P')++script.index;
	size_t psize=readInt(script, ' ');
	script.data->pVars=(void**)malloc(sizeof(void*)*psize);
	for (int i=0; i<psize; ++i)script.data->pVars[i]=NULL;
	//ARRs
	while(script.currentChar()!='A')++script.index;
	size_t asize=readInt(script, ' ');
	script.data->aVars=(vector<void*>*)malloc(sizeof(vector<void*>)*asize);
	for (int i=0; i<asize; ++i)script.data->aVars[i]=vector<void*>();
	script.index=0;
}
void XPINSParser::ParseScript(string scriptText,vector<XPINSBindings*> bindings)
{
	XPINSScriptSpace script=*(new XPINSScriptSpace(scriptText,bindings));
	if(!checkVersion(script))return;//Check Script Version
	//Set up scriptVars Space
	allocVars(script);
	//Run Script
	ParseCode(script,0,-1);
	//Clean up
	delete script.data;
	for (int i=0; i<bindings.size(); ++i) {
		while (bindings[i]->toDelete.size()>0)
		{
			delete bindings[i]->toDelete.front();
			bindings[i]->toDelete.pop_front();
		}

	}
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
						script.data->bVars[index]=*ParseBoolArg(script, '\n');
					}break;
					case 'N':{//NUM variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->nVars[index]=*ParseNumArg(script, '\n');
					}break;
					case 'V':{//VEC variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->vVars[index]=*ParseVecArg(script, '\n');
					}break;
					case 'M':{//MAT variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						Matrix mat=*ParseMatArg(script, '\n');
					//	script.data->mVars[index].clean();
						script.data->mVars[index]=mat;
					}break;
					case 'S':{//STR variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->sVars[index]=*ParseStrArg(script, '\n');
					}break;
					case 'P':{//Pointer variable
						++script.index;
						int index=readInt(script, '=');
						++script.index;
						script.data->pVars[index]=*ParsePointerArg(script, '\n');
					}break;
					case 'A':{//Array variable
						++script.index;
						int temp=script.index;
						bool assign;
						while(script.instructions[++temp]!='=')
							if(script.instructions[++temp]=='[')assign=true;
						if(assign){
							vector<void*>*arr=ParseArrayArg(script, '[');
							++script.index;
							int arrIndex=readInt(script, ']');
							(*arr)[arrIndex]=*ParsePointerArg(script, '\n');
						}
						else{
							int index=readInt(script, '=');
							++script.index;
							script.data->aVars[index]=*ParseArrayArg(script, '\n');
						}
					}break;
				}
			}break;
			case '#':{
				script.index+=2;
				int mNum=readInt(script, 'F');
				int fNum=readInt(script, '(');
				script.bindings[mNum]->BindFunction(fNum, script, NULL);
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