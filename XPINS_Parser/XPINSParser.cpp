//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSParser.h"
#include <math.h>


using namespace std;
const kMajor=0;
const kMinor=4;


//Expression Parsing
bool parseBoolExp(char* scriptText,varSpace* vars, int* start){
	bool result=false;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]=='B'){
		cerr<<"Could not parse XPINS expression, returning false\n";
	}
	else{
		i+=2;
		if (scriptText[i]=='!'&&scriptText[i+1]=='=') {//not
			++i;
			bool arg=parseBoolArg(scriptText, vars, &i, ')');
			result=!arg;
			++i;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=parseBoolArg(scriptText, vars, &i,'|');
				if(arg1){
					result=true;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=parseBoolArg(scriptText, vars, &i,')');
					result=arg1||arg2;
					++i;
				}
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=parseBoolArg(scriptText, vars, &i,'|');
				if(!arg1){
					result=false;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=parseBoolArg(scriptText, vars, &i,')');
					result=arg1&&arg2;
					++i;
				}
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'<');
				else
					arg1=parseIntArg(scriptText, vars, &i, '<');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'<');
				else
					arg1=parseIntArg(scriptText, vars, &i, '<');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'>');
				else
					arg1=parseIntArg(scriptText, vars, &i, '>');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'>');
				else
					arg1=parseIntArg(scriptText, vars, &i, '>');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'!');
				else
					arg1=parseIntArg(scriptText, vars, &i, '!');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, vars, &i,'=');
				else
					arg1=parseIntArg(scriptText, vars, &i, '=');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, vars, &i,')');
				else
					arg2=parseIntArg(scriptText, vars, &i, ')');
				++i;
				result=arg1==arg2;
			}
		}
	}
	*start=i;
	return result;
}
int parseIntExp(char* scriptText,varSpace* vars, int* start){
	int result=0;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]=='I'){
		cerr<<"Could not parse XPINS expression, returning 0\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/'&&scriptText[j]!='%') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'+');
			else
				result=parseIntArg(scriptText, vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=parseFloatArg(scriptText, vars, &i,')');
			else
				result+=parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'-');
			else
				result=parseIntArg(scriptText, vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=parseFloatArg(scriptText, vars, &i,')');
			else
				result-=parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'*');
			else
				result=parseIntArg(scriptText, vars, &i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=parseFloatArg(scriptText, vars, &i,')');
			else
				result*=parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'/');
			else
				result=parseIntArg(scriptText, vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=parseFloatArg(scriptText, vars, &i,')');
			else
				result/=parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='%') {//modulus
			result=parseIntArg(scriptText, vars, &i, '%');
			++i;
			result%=parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}
float parseFloatExp(char* scriptText,varSpace* vars, int* start){
	float result=0.0;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]=='F'){
		cerr<<"Could not parse XPINS expression, returning 0.0\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'+');
			else
				result=(float)parseIntArg(scriptText, vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=parseFloatArg(scriptText, vars, &i,')');
			else
				result+=(float)parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'-');
			else
				result=(float)parseIntArg(scriptText, vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=parseFloatArg(scriptText, vars, &i,')');
			else
				result-=(float)parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'*');
			else
				result=(float)parseIntArg(scriptText, vars, &i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=parseFloatArg(scriptText, vars, &i,')');
			else
				result*=(float)parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, vars, &i,'/');
			else
				result=(float)parseIntArg(scriptText, vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=parseFloatArg(scriptText, vars, &i,')');
			else
				result/=(float)parseIntArg(scriptText, vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}

//parameter parsing
bool parseBoolArg(char * scriptText,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	bool retVal=false;
	while (scriptText[i]!='$'&&scriptText[i]!='^') ++i;
	if(scriptText[i]=='$'){
		int index=readFuncParameter(scriptText, &i, 'B', expectedEnd);
		retVal=vars->bVars[index];
	}
	else if(scriptText[i]=='^'){
		retVal=(scriptText[i+1]=='T');
	}
	else if(scriptText[i]=='?'){
		retVal=parseBoolExp(scriptText, vars, start);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
int parseIntArg(char * scriptText,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	int retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^') ++i;
	if(scriptText[i]=='$'){
		int index=readFuncParameter(scriptText, &i, 'I', expectedEnd);
		retVal=vars->iVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readInt(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=parseIntExp(scriptText, vars, start);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
float parseFloatArg(char * scriptText,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^') ++i;
	if(scriptText[i]=='$'){
		int index=readFuncParameter(scriptText, &i, 'F', expectedEnd);
		retVal=vars->fVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readFloat(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=parseFloatExp(scriptText, vars, start);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
XPINSScriptableMath::Vector parseVecArg(char * scriptText,varSpace* vars,int* start,char expectedEnd)float parseFloatArg(char * scriptText,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	XPINSScriptableMath::Vector* retVal=NULL;
	while (scriptText[i]!='$') ++i;
	int index=readFuncParameter(scriptText, &i, 'V', expectedEnd);
	retVal=vars->vVars[index];
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
void* parsePointerArg(char * scriptText,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	void* retVal=NULL;
	while (scriptText[i]!='$') ++i;
	int index=readFuncParameter(scriptText, &i, 'P', expectedEnd);
	retVal=vars->pVars[index];
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
//Helper functions

//read index of a variable:
// scriptText: the script
// startIndex: the starting index of the variable
// expectedEnd: the character expected immediately after the variable
int readVarIndex(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	while(scriptText[i]!=expectedEnd){
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
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	*startIndex=i;
	return index;
}
//Read an int constant
// scriptText: the script
// startIndex: the starting index of the INT
// expectedEnd: the character expected immediately after the INT
int readInt(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	bool isNeg=scriptText[i]=='-';//Make negative if approrpriate
	while(scriptText[i]!=expectedEnd){
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
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	if(isNeg)index*=-1;
	*startIndex=i;
	return index;
}
//read a float constant
// scriptText: the script
// startIndex: the starting index of the FLOAT
// expectedEnd: the character expected immediately after the FLOAT
float readFloat(char* scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	int fpartDig=0;
	bool fpart=false;
	bool isNeg=scriptText[i]=='-';
	while(scriptText[i]!=expectedEnd){
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
	*startIndex=i;
	return index;
}
//Read Variable Index for Function Parameters
int readFuncParameter(char* scriptText,int *startIndex,char varType,char expectedEnd){
	++i;
	if (scriptText[i]!='$'||scriptText[i+1]!=varType) {
		printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
		return;
	}
	i+=2;
	int index=readVarIndex(scriptText, &i, expectedEnd);
	*startIndex=i;
	return index;
}
//Check to make sure script is compatible
// script: the script
bool checkVersion(char* script){
	for(int i=0;true;i++){
		if(script[i]=='@'&&script[i+1]=='P'&&script[i+2]=='A'&&script[i+3]=='R'&&script[i+4]=='S'&&script[i+5]=='E'&&script[i+6]=='R'){
			while(script[i]!='[')i++;
			int MAJOR=readVarIndex(script, &i, '.');
			int MINOR=readVarIndex(script, &i, ']');
			if(MAJOR!=kMajor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			if(MINOR<kMinor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			return true;
		}
	}
	cout<<"SCRIPT MISSING VERSION. FAILING";
	return false;
}
//Primary Function
//See header for parameter descriptions
void XPINSParser::parseScript(char* scriptText,varSpace *vars,params *parameters,bool isRECURSIVE,int start,int stop){
	
	//SET UP VAR SPACE
	bool initialized_varSpace=false;
	if(vars==NULL){
		vars=new varSpace;
		vars->bVars=vector<bool>();
		vars->iVars=vector<int>();
		vars->fVars=vector<float>();
		vars->vVars=vector<XPINSScriptableMath::Vector*>();
		vars->pVars=vector<void*>();
		initialized_varSpace=true;
	}
	int bSize=vars->bVars.size();
	int iSize=vars->iVars.size();
	int fSize=vars->fVars.size();
	int vSize=vars->vVars.size();
	int pSize=vars->pVars.size();
	
	//RUN SCRIPT
	int i=0;//index of char in script
	if(isRECURSIVE)i=start;
	//Validate start of script
	if(!isRECURSIVE){
		if(!checkVersion(scriptText))return;
		while (scriptText[i]!='\n')i++;
		while(scriptText[i]!='@')i++;
		if(scriptText[i+1]!='C'||
		   scriptText[i+2]!='O'||
		   scriptText[i+3]!='D'||
		   scriptText[i+4]!='E'){
			printf("\nERROR:INVALID SCRIPT:MISSING @CODE!\n");
			return;
		}
	}
	while(true){
		//get to next line
		while(scriptText[i]!='\n')i++;
		i++;
		//reaching the end of the Script
		if((scriptText[i]!='@'&&
			scriptText[i+1]!='E'&&
			scriptText[i+2]!='N'&&
			scriptText[i+3]!='D')||
		   (isRECURSIVE&&i==stop)){
			break;
		}
		//Declaring new vars
		if(scriptText[i]=='B'){
			vars->bVars.resize(vars->bVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='I'){
			vars->iVars.resize(vars->iVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='F'){
			vars->fVars.resize(vars->fVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='V'){
			vars->vVars.resize(vars->vVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='*'){
			vars->pVars.resize(vars->pVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		if(scriptText[i]=='$'){
			i++;
			if(scriptText[i]=='B'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='B') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPE DOESN'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->bVars[index]=vars->bVars[index2];
				}
				else if(scriptText[i]=='^'){
					if(scriptText[i+1]=='T')
						vars->bVars[index]=true;
					else if(scriptText[i+1]=='F')
						vars->bVars[index]=false;
					else{
						printf("\nERROR:INVALID SCRIPT:INVALID BOOL CONSTANT!\n");
						return;
					}
				}
				else if(scriptText[i]=='?'){
					++i;
					vars->bVars[index]=parseBoolExp(scriptText, vars, &i);
				}
				else if(scriptText[i]=='#'||scriptText[i]=='X'){
					if ((scriptText[i]!='#'||scriptText[i+1]!='F')&&(scriptText[i]!='X'||scriptText[i+1]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					if(scriptText[i]=='#'&&scriptText[i+1]=='F'){
						i+=2;
						int fNum=readVarIndex(scriptText, &i, '(');
						XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i,index);
					}
					else{
						i+=2;
						//BUILT IN FUNCTIONS
						//X_AND
						if(scriptText[i+1]=='A'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D'){
							i+=4;
							bool param1=parseBoolArg(scriptText, vars, &i, ',');
							bool param2=parseBoolArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1&&param2;
						}
						//X_OR
						else if(scriptText[i+1]=='O'&&scriptText[i+2]=='R'){
							i+=3;
							bool param1=parseBoolArg(scriptText, vars, &i, ',');
							bool param2=parseBoolArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1||param2;
						}
						//X_NOT
						else if(scriptText[i+1]=='N'&&scriptText[i+2]=='O'&&scriptText[i+3]=='T'){
							i+=4;
							bool param1=parseBoolArg(scriptText, vars, &i, ')');
							vars->bVars[index]=!param1;
						}
						//X_ILESS
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
							i+=6;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1<param2;
						}
						//X_FLESS
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
							i+=6;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1<param2;

						}
						//X_IMORE
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
							i+=6;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1>param2;
						}
						//X_FMORE
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
							i+=6;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1>param2;
						}
						//X_IEQUAL
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
							i+=7;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1==param2;
						}
						//X_FEQUAL
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
							i+=7;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->bVars[index]=param1==param2;
						}
						else{
							printf("\nERROR:INVALID SCRIPT:UNDEFINED FUNCTION!\n");
							return;
						}
					}
				}
			}
			else if(scriptText[i]=='I'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='I') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->iVars[index]=vars->iVars[index2];
				}
				else if(scriptText[i]=='^'){
					i++;
					int n=readInt(scriptText,&i,'\n');
					vars->iVars[index]=n;
				}
				else if(scriptText[i]=='?'){
					++i;
					vars->iVars[index]=parseIntExp(scriptText, vars, &i);
				}
				else if(scriptText[i]=='#'||scriptText[i]=='X'){
					if ((scriptText[i]!='#'||scriptText[i+1]!='F')&&(scriptText[i]!='X'||scriptText[i+1]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					if(scriptText[i]=='#'&&scriptText[i+1]=='F'){
						i+=2;
						int fNum=readVarIndex(scriptText, &i, '(');
						XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i);
					}
					else{
						i+=2;
						//BUILT IN FUNCTIONS
						//X_IADD
						if(scriptText[i+1]=='I'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
							i+=5;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=param1+param2;
						}
						//X_ISUB
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
							i+=5;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=param1-param2;
						}
						//X_IMULT
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
							i+=6;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=param1*param2;
						}
						//X_IDIV
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
							i+=5;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=param1/param2;
						}
						//X_IMOD
						else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='D'){
							i+=5;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=param1%param2;
						}
						//X_RAND
						else if(scriptText[i+1]=='R'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+3]=='D'){
							i+=5;
							int param1=parseIntArg(scriptText, vars, &i, ',');
							int param2=parseIntArg(scriptText, vars, &i, ')');
							vars->iVars[index]=arc4random()%(param2-param1)+param1;
						}
						else{
							printf("\nERROR:INVALID SCRIPT:NONEXISTENT FUNCTION!\n");
							return;
						}
					}
				}
			}
			else if(scriptText[i]=='F'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='F') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->fVars[index]=vars->fVars[index2];
				}
				else if(scriptText[i]=='^'){
					i++;
					float n=readFloat(scriptText,&i,'\n');
					vars->fVars[index]=n;
				}
				else if(scriptText[i]=='?'){
					++i;
					vars->fVars[index]=parseFloatExp(scriptText, vars, &i);
				}
				else if(scriptText[i]=='#'||scriptText[i]=='X'){
					if ((scriptText[i]!='#'||scriptText[i+1]!='F')&&(scriptText[i]!='X'||scriptText[i+1]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					if(scriptText[i]=='#'&&scriptText[i+1]=='F'){
						i+=2;
						int fNum=readVarIndex(scriptText, &i, '(');
						XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i,index);
					}
					else{
						i+=2;
						//BUILT IN FUNCTIONS
						//X_FADD
						if(scriptText[i+1]=='F'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=param1+param2;
						}
						//X_FSUB
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=param1-param2;
						}
						//X_FMULT
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
							i+=6;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=param1*param2;
						}
						//X_FDIV
						else if(scriptText[i+1]=='F'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=param1/param2;
						}
						//X_VMAG
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='G'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ')');
							vars->fVars[index]=vars->param1->magnitude();
						}
						//X_VDIR
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='R'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ')');
							vars->fVars[index]=param1->direction();
						}
						//X_VX
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='X'){
							i+=3;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ')');
							float f=0;
							vars->param1->RectCoords(&f, NULL);
							vars->fVars[index]=f;
						}
						//X_VY
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='Y'){
							i+=3;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ')');
							float f=0;
							vars->param1->RectCoords(&f, NULL);
							vars->fVars[index]=f;
						}
						//X_VANG
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+4]=='G'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							XPINSScriptableMath::Vector* param2=parseVecArg(scriptText, vars, &i, ')');
							vars->fVars[index]=XPINSScriptableMath::Vector::angleBetweenVectors(param1, param2);
						}
						//X_VDOT
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							XPINSScriptableMath::Vector* param2=parseVecArg(scriptText, vars, &i, ')');
							vars->fVars[index]=XPINSScriptableMath::Vector::dotProduct(param1, param2);
						}
						//X_VADDPOLAR
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'&&scriptText[i+5]=='P'&&scriptText[i+6]=='O'&&scriptText[i+7]=='L'&&scriptText[i+8]=='A'&&scriptText[i+9]=='R'){
							i+=10;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=XPINSScriptableMath::addPolar(param1, param2);
						}
						//X_VDIST
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='S'&&scriptText[i+5]=='T'){
							i+=6;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=XPINSScriptableMath::dist(param1, param2);						}
						//X_TSIN
						else if(scriptText[i+1]=='T'&&scriptText[i+2]=='S'&&scriptText[i+3]=='I'&&scriptText[i+4]=='N'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=sinf(param1);
						}
						//X_TCOS
						else if(scriptText[i+1]=='T'&&scriptText[i+2]=='C'&&scriptText[i+3]=='O'&&scriptText[i+4]=='S'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=cosf(param1);
						}
						//X_TTAN
						else if(scriptText[i+1]=='T'&&scriptText[i+2]=='T'&&scriptText[i+3]=='A'&&scriptText[i+4]=='N'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=tanf(param1);						}
						//X_TATAN
						else if(scriptText[i+1]=='T'&&scriptText[i+2]=='A'&&scriptText[i+3]=='T'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'){
							i+=6;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=atan2f(param1, param2);
						}
						//X_POW
						else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
							i+=4;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->fVars[index]=powf(param1, param2);
						}
						else{
							printf("\nERROR:INVALID SCRIPT!\n");
							return;
						}
					}
				}
			}
			else if(scriptText[i]=='V'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='V') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->vVars[index]=vars->vVars[index2]->copy();
				}
				else if(scriptText[i]=='#'||scriptText[i]=='X'){
					if ((scriptText[i]!='#'||scriptText[i+1]!='F')&&(scriptText[i]!='X'||scriptText[i+1]!='_')) {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					if(scriptText[i]=='#'&&scriptText[i+1]=='F'){
						i+=2;
						int fNum=readVarIndex(scriptText, &i, '(');
						XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i,index);
					}
					else{
						i+=2;
						//X_VREC
						if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->vVars[index]=new XPINSScriptableMath::Vector::Vector(param1,param2);

						}
						//X_VPOL
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
							i+=5;
							float param1=parseFloatArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->vVars[index]=XPINSScriptableMath::Vector::PolarVector(param1,param2);
						}
						//X_VADD
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							XPINSScriptableMath::Vector* param2=parseVecArg(scriptText, vars, &i, ')');
							vars->vVars[index]=XPINSScriptableMath::Vector::addVectors(param1,param2);
						}
						//X_VSUB
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
							i+=5;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							XPINSScriptableMath::Vector* param2=parseVecArg(scriptText, vars, &i, ')');
							XPINSScriptableMath::Vector *temp=XPINSScriptableMath::Vector::scaledVector(param2, -1);
							vars->vVars[index]=XPINSScriptableMath::Vector::addVectors(param1,temp);
							delete temp;
						}
						//X_VSCALE
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+5]=='L'&&scriptText[i+6]=='E'){
							i+=7;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->vVars[index]=XPINSScriptableMath::Vector::scaledVector(param1,param2);
						}
						//X_VPROJ
						else if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+4]=='J'){
							i+=6;
							XPINSScriptableMath::Vector* param1=parseVecArg(scriptText, vars, &i, ',');
							float param2=parseFloatArg(scriptText, vars, &i, ')');
							vars->vVars[index]=XPINSScriptableMath::Vector::projectionInDirection(param1,param2);
						}
						else{
							printf("\nERROR:INVALID SCRIPT:UNDECLARED FUNCTION!\n");
							return;
						}
					}
				}
			}
			else if(scriptText[i]=='P'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				while(scriptText[i]!='#'&&scriptText[i]!='$'&&scriptText[i]!='^')i++;
				if(scriptText[i]=='$'){
					if (scriptText[i+1]!='P') {
						printf("\nERROR:INVALID SCRIPT:DECLARED VARIABLE TYPES DON'T MATCH!\n");
						return;
					}
					i+=2;
					int index2=readVarIndex(scriptText, &i, '\n');
					vars->pVars[index]=vars->pVars[index2];
				}
				else if(scriptText[i]=='#'){
					if (scriptText[i+1]!='F') {
						printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
						return;
					}
					i+=2;
					int fNum=readVarIndex(scriptText, &i, '(');
					XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i,index);
				}
			}
		}
		else if(scriptText[i]=='#'){
			if (scriptText[i+1]!='F') {
				printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
			}
			i+=2;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, parameters, vars, scriptText, &i,0);
		}
		else if(scriptText[i]=='@'){
			i++;
			//IF STATEMENT
			if(scriptText[i]=='I'&&scriptText[i+1]=='F'){
				while (scriptText[i]!='$') i++;
				if(scriptText[i+1]!='B'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING IF BLOCK");
				}
				else{
					i+=2;
					int index=readVarIndex(scriptText, &i, ']');
					if(!vars->bVars[index]){
						while (scriptText[i]!='{')i++;
						int num=1;
						//skip if block
						while(num>0){
							if(scriptText[i]=='{')num++;
							else if(scriptText[i]=='}')num--;
							i++;
						}
						while (scriptText[i]!='\n')i++;
						//execute else if applicable
						if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
							while (scriptText[i]!='{')i++;
							i++;
						}
					}
					else{
						while (scriptText[i]!='{')i++;
						i++;
					}
				}
			}
			//WHILE LOOP
			else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E'){
				while (scriptText[i]!='$') i++;
				if(scriptText[i+1]!='B'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
				}
				else{
					i+=2;
					int index=readVarIndex(scriptText, &i, ']');
					while (scriptText[i]!='{')i++;
					int loopStart=i;
					int num=1;
					//skip if block
					while(num>0){
						if(scriptText[i]=='{')num++;
						else if(scriptText[i]=='}')num--;
						i++;
					}
					int loopStop=i-1;
					while (vars->bVars[index]) {
						ScriptParser::parseScript(scriptText, vars, parameters, true, loopStart, loopStop);
					}
				}
			}
			//ELSE (BYPASS because IF was executed)
			else if(scriptText[i]=='E'&&scriptText[i+1]=='L'&&scriptText[i+2]=='S'&&scriptText[i+3]=='E'){
				while (scriptText[i]!='{')i++;
				int num=1;
				//skip if block
				while(num>0){
					if(scriptText[i]=='{')num++;
					else if(scriptText[i]=='}')num--;
					i++;
				}
				
			}
		}
	}
	if(initialized_varSpace) delete vars;
	else{//wipe declared variables
		int bSize2=vars->bVars.size();
		int iSize2=vars->iVars.size();
		int fSize2=vars->fVars.size();
		int vSize2=vars->vVars.size();
		int pSize2=vars->pVars.size();
		vars->bVars.resize(bSize);
		vars->iVars.resize(iSize);
		vars->fVars.resize(fSize);
		vars->vVars.resize(vSize);
		vars->pVars.resize(pSize);
		vars->bVars.resize(bSize2);
		vars->iVars.resize(iSize2);
		vars->fVars.resize(fSize2);
		vars->vVars.resize(vSize2);
		vars->pVars.resize(pSize2);

	}
}











