//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSParser.h"
#include "XPINSBridge.h"
#include <math.h>


using namespace std;
const int kPMajor=0;
const int kPMinor=6;

//Helper functions

//read index of a variable:
// scriptText: the script
// startIndex: the starting index of the variable
// expectedEnd: the character expected immediately after the variable
int readVarIndex(string scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd){
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
int readInt(string scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	bool isNeg=scriptText[i]=='-';//Make negative if approrpriate
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd){
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
float readFloat(string scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
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
	*startIndex=i;
	return index;
}
//Read Variable Index for Function Parameters
int readFuncParameter(string scriptText,int *startIndex,char varType,char expectedEnd){
	int i=*startIndex;
	++i;
	if (scriptText[i]!='$'||scriptText[i+1]!=varType) {
		// printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
		return 0;
	}
	i+=2;
	int index=readVarIndex(scriptText, &i, expectedEnd);
	*startIndex=i;
	return index;
}

//Expression Parsing
bool parseBoolExp(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars, int* start){
	bool result=false;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='B'){
		//cerr<<"Could not parse XPINS expression, returning false\n";
		return false;
	}
	else{
		i+=2;
		if (scriptText[i]=='!'&&scriptText[i+1]=='=') {//not
			++i;
			bool arg=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ')');
			result=!arg;
			++i;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=XPINSParser::parseBoolArg(scriptText, params,vars, &i,'|');
				if(arg1){
					result=true;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=XPINSParser::parseBoolArg(scriptText,params,vars, &i,')');
					result=arg1||arg2;
					++i;
				}
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=XPINSParser::parseBoolArg(scriptText, params,vars, &i,'|');
				if(!arg1){
					result=false;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=parseBoolArg(scriptText, params,vars, &i,')');
					result=arg1&&arg2;
					++i;
				}
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=XPINSParser::parseFloatArg(scriptText, params,vars, &i,'<');
				else
					arg1=XPINSParser::parseIntArg(scriptText, params,vars, &i, '<');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=XPINSParser::parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, params,vars, &i,'<');
				else
					arg1=parseIntArg(scriptText, params,vars, &i, '<');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, params,vars, &i,'>');
				else
					arg1=parseIntArg(scriptText, params,vars, &i, '>');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, params,vars, &i,'>');
				else
					arg1=parseIntArg(scriptText, params,vars, &i, '>');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, params,vars, &i,'!');
				else
					arg1=parseIntArg(scriptText, params,vars, &i, '!');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=parseFloatArg(scriptText, params,vars, &i,'=');
				else
					arg1=parseIntArg(scriptText, params,vars, &i, '=');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=parseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=parseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1==arg2;
			}
		}
	}
	*start=i;
	return result;
}
int parseIntExp(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars, int* start){
	int result=0;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='I'){
		cerr<<"Could not parse XPINS expression, returning 0\n";
		return 0;
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/'&&scriptText[j]!='%') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'+');
			else
				result=parseIntArg(scriptText, params,vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result+=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'-');
			else
				result=parseIntArg(scriptText, params,vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result-=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'*');
			else
				result=parseIntArg(scriptText, params,vars, &i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result*=parseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'/');
			else
				result=parseIntArg(scriptText, params,vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result/=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='%') {//modulus
			result=parseIntArg(scriptText, params,vars, &i, '%');
			++i;
			result%=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}
float parseFloatExp(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars, int* start){
	float result=0.0;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='F'){
		cerr<<"Could not parse XPINS expression, returning 0.0\n";
		return 0.0;
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText,params, vars, &i,'+');
			else
				result=(float)parseIntArg(scriptText,params, vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=parseFloatArg(scriptText,params, vars, &i,')');
			else
				result+=(float)parseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'-');
			else
				result=(float)parseIntArg(scriptText, params,vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result-=(float)parseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				XPINSScriptableMath::Vector *v1=XPINSParser::parseVecArg(scriptText, params,vars, &i,'+');
				++i;
				XPINSScriptableMath::Vector *v2=XPINSParser::parseVecArg(scriptText, params,vars, &i,')');
				++i;
				result=XPINSScriptableMath::Vector::dotProduct(v1, v2);
				delete v1;
				delete v2;
			}
			else{
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=parseFloatArg(scriptText,params, vars, &i,'*');
				else
					result=(float)parseIntArg(scriptText, params,vars, &i, '*');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=parseFloatArg(scriptText, params,vars, &i,')');
				else
					result*=(float)parseIntArg(scriptText,params, vars, &i, ')');
				++i;
			}
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=parseFloatArg(scriptText, params,vars, &i,'/');
			else
				result=(float)parseIntArg(scriptText, params,vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=parseFloatArg(scriptText, params,vars, &i,')');
			else
				result/=(float)parseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}
XPINSScriptableMath::Vector* parseVecExp(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars, int* start){
	XPINSScriptableMath::Vector *result=NULL;
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
		result=new XPINSScriptableMath::Vector(0,0);
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			XPINSScriptableMath::Vector *v1=XPINSParser::parseVecArg(scriptText, params,vars, &i,'+');
			++i;
			XPINSScriptableMath::Vector *v2=XPINSParser::parseVecArg(scriptText, params,vars, &i,')');
			++i;
			result=XPINSScriptableMath::Vector::addVectors(v1, v2);
			delete v1;
			delete v2;
		}
		else if (scriptText[j]=='-') {//subtraction
			XPINSScriptableMath::Vector *v1=XPINSParser::parseVecArg(scriptText, params,vars, &i,'-');
			++i;
			XPINSScriptableMath::Vector *temp=XPINSParser::parseVecArg(scriptText, params,vars, &i,')');
			XPINSScriptableMath::Vector *v2=XPINSScriptableMath::Vector::scaledVector(temp, -1);
			delete temp;
			++i;
			result=XPINSScriptableMath::Vector::addVectors(v1, v2);
			delete v1;
			delete v2;
		}
		else if (scriptText[j]=='*') {//multiplication
			XPINSScriptableMath::Vector *v1=XPINSParser::parseVecArg(scriptText, params,vars, &i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=parseFloatArg(scriptText, params,vars, &i,')');
			else
				k=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
			result=XPINSScriptableMath::Vector::scaledVector(v1, k);
			delete v1;
		}
		else if (scriptText[j]=='/') {//division
			XPINSScriptableMath::Vector *v1=XPINSParser::parseVecArg(scriptText, params, vars, &i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=parseFloatArg(scriptText,params, vars, &i,')');
			else
				k=parseIntArg(scriptText, params,vars, &i, ')');
			++i;
			result=XPINSScriptableMath::Vector::scaledVector(v1, 1/k);
			delete v1;
		}
	}
	*start=i;
	return result;
}

//parameter parsing
bool XPINSParser::parseBoolArg(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	bool retVal=false;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if (i>=scriptText.length()) {
		return false;
	}
	if(scriptText[i]=='$'){//Variable
		i+=2;
		int index=readVarIndex(scriptText, &i, expectedEnd);
		retVal=vars->bVars[index];
	}
	else if(scriptText[i]=='^'){//constant
		retVal=(scriptText[i+1]=='T');
	}
	else if(scriptText[i]=='?'){//expression
		retVal=parseBoolExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='B'||scriptText[i+2]!='F'){
			retVal=false;
		}
		else{
			i+=3;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText, params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='B'){
			retVal=0;
		}
		//X_AND
		else if(scriptText[i+1]=='A'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D'){
			i+=4;
			bool param1=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ',');
			bool param2=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ')');
			retVal=param1&&param2;
		}
		//X_OR
		else if(scriptText[i+1]=='O'&&scriptText[i+2]=='R'){
			i+=3;
			bool param1=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ',');
			bool param2=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ')');
			retVal=param1||param2;
		}
		//X_NOT
		else if(scriptText[i+1]=='N'&&scriptText[i+2]=='O'&&scriptText[i+3]=='T'){
			i+=4;
			bool param1=XPINSParser::parseBoolArg(scriptText, params,vars, &i, ')');
			retVal=!param1;
		}
		//X_ILESS
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
			i+=6;
			int param1=parseIntArg(scriptText, params,vars, &i, ',');
			int param2=parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1<param2;
		}
		//X_FLESS
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
			i+=6;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1<param2;
			
		}
		//X_IMORE
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
			i+=6;
			int param1=parseIntArg(scriptText, params,vars, &i, ',');
			int param2=parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1>param2;
		}
		//X_FMORE
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
			i+=6;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1>param2;
		}
		//X_IEQUAL
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
			i+=7;
			int param1=parseIntArg(scriptText, params,vars, &i, ',');
			int param2=parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1==param2;
		}
		//X_FEQUAL
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
			i+=7;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1==param2;
		}
		else{
			retVal=false;
		}
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
int XPINSParser::parseIntArg(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	int retVal=0;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readVarIndex(scriptText, &i, expectedEnd);
		retVal=vars->iVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readInt(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=parseIntExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='I'||scriptText[i+2]!='F'){
			retVal=NULL;
		}
		else{
			i+=3;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='I'){
			retVal=0;
		}
		//X_IADD
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
			i+=5;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1+param2;
		}
		//X_ISUB
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
			i+=5;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params, vars, &i, ')');
			retVal=param1-param2;
		}
		//X_IMULT
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
			i+=6;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1*param2;
		}
		//X_IDIV
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
			i+=5;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1/param2;
		}
		//X_IMOD
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='D'){
			i+=5;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1%param2;
		}
		//X_RAND
		else if(scriptText[i+1]=='R'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+3]=='D'){
			i+=5;
			int param1=XPINSParser::parseIntArg(scriptText, params,vars, &i, ',');
			int param2=XPINSParser::parseIntArg(scriptText, params,vars, &i, ')');
			retVal=arc4random()%(param2-param1)+param1;
		}
		else retVal=0;
	}
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
float XPINSParser::parseFloatArg(string scriptText,XPINSParams* params, varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readVarIndex(scriptText, &i, expectedEnd);
		retVal=vars->fVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readFloat(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=parseFloatExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='F'||scriptText[i+2]!='F'){
			retVal=NULL;
		}
		else{
			i+=3;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='F'){
			retVal=0;
		}
		//X_FADD
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1+param2;
		}
		//X_FSUB
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1-param2;
		}
		//X_FMULT
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
			i+=6;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1*param2;
		}
		//X_FDIV
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1/param2;
		}
		//X_VMAG
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='G'){
			i+=5;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			retVal=param1->magnitude();
		}
		//X_VDIR
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='R'){
			i+=5;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			retVal=param1->direction();
		}
		//X_VX
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='X'){
			i+=3;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1->RectCoords(&f, NULL);
			retVal=f;
		}
		//X_VY
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='Y'){
			i+=3;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1->RectCoords(NULL, &f);
			retVal=f;
		}
		//X_VANG
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+4]=='G'){
			i+=5;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
			XPINSScriptableMath::Vector* param2=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			retVal=XPINSScriptableMath::Vector::angleBetweenVectors(param1, param2);
		}
		//X_VDOT
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
			i+=5;
			XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
			XPINSScriptableMath::Vector* param2=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
			retVal=XPINSScriptableMath::Vector::dotProduct(param1, param2);
		}
		//X_VADDPOLAR
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'&&scriptText[i+5]=='P'&&scriptText[i+6]=='O'&&scriptText[i+7]=='L'&&scriptText[i+8]=='A'&&scriptText[i+9]=='R'){
			i+=10;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=XPINSScriptableMath::addPolar(param1, param2);
		}
		//X_VDIST
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='S'&&scriptText[i+5]=='T'){
			i+=6;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=XPINSScriptableMath::dist(param1, param2);
		}
		//X_TSIN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='S'&&scriptText[i+3]=='I'&&scriptText[i+4]=='N'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=sinf(param1);
		}
		//X_TCOS
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='C'&&scriptText[i+3]=='O'&&scriptText[i+4]=='S'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=cosf(param1);
		}
		//X_TTAN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='T'&&scriptText[i+3]=='A'&&scriptText[i+4]=='N'){
			i+=5;
			float param1=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=tanf(param1);
		}
		//X_TATAN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='A'&&scriptText[i+3]=='T'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'){
			i+=6;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=atan2f(param1, param2);
		}
		//X_POW
		else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
			i+=4;
			float param1=parseFloatArg(scriptText, params,vars, &i, ',');
			float param2=parseFloatArg(scriptText, params,vars, &i, ')');
			retVal=powf(param1, param2);
		}
		else retVal=0;
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
XPINSScriptableMath::Vector* XPINSParser::parseVecArg(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars,int* start,char expectedEnd){

	int i=*start;
	XPINSScriptableMath::Vector* retVal=NULL;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readVarIndex(scriptText, &i, expectedEnd);
		retVal=vars->vVars[index]->copy();
	}
	else if(scriptText[i]=='^'){//constant (can contain varialbes, though)
		++i;
		if(scriptText[i]=='P'){//Polar Vector
			if(scriptText[++i]!='<')retVal=new XPINSScriptableMath::Vector(0,0);
			else{
				++i;
				float r=parseFloatArg(scriptText, params,vars, &i, ',');
				float t=parseFloatArg(scriptText, params,vars, &i, '>');
				retVal=XPINSScriptableMath::Vector::PolarVector(r, t);
			}
		}
		else if(scriptText[++i]=='<'){//rectangular vector
			++i;
			float x=parseFloatArg(scriptText, params,vars, &i, ',');
			float y=parseFloatArg(scriptText, params,vars, &i, '>');
			retVal=new XPINSScriptableMath::Vector(x, y);
		}
		else{
			retVal=new XPINSScriptableMath::Vector(0,0);
		}
	}
	else if(scriptText[i]=='?'){
		retVal=parseVecExp(scriptText, params, vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='V'||scriptText[i+2]!='F'){
			retVal=NULL;
		}
		else{
			i+=3;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='V'){
			retVal=0;
		}
		else //X_VREC
			if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
				i+=5;
				float param1=parseFloatArg(scriptText, params,vars, &i, ',');
				float param2=parseFloatArg(scriptText, params,vars, &i, ')');
				retVal=new XPINSScriptableMath::Vector::Vector(param1,param2);
				
			}
		//X_VPOL
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
				i+=5;
				float param1=parseFloatArg(scriptText, params,vars, &i, ',');
				float param2=parseFloatArg(scriptText, params,vars, &i, ')');
				retVal=XPINSScriptableMath::Vector::PolarVector(param1,param2);
			}
		//X_VADD
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
				i+=5;
				XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
				XPINSScriptableMath::Vector* param2=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
				retVal=XPINSScriptableMath::Vector::addVectors(param1,param2);
				delete param1;
				delete param2;
			}
		//X_VSUB
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
				i+=5;
				XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
				XPINSScriptableMath::Vector* param2=XPINSParser::parseVecArg(scriptText, params,vars, &i, ')');
				XPINSScriptableMath::Vector *temp=XPINSScriptableMath::Vector::scaledVector(param2, -1);
				retVal=XPINSScriptableMath::Vector::addVectors(param1,temp);
				delete temp;
				delete param1;
				delete param2;
			}
		//X_VSCALE
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+5]=='L'&&scriptText[i+6]=='E'){
				i+=7;
				XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
				float param2=parseFloatArg(scriptText, params,vars, &i, ')');
				retVal=XPINSScriptableMath::Vector::scaledVector(param1,param2);
				delete param1;
			}
		//X_VPROJ
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+4]=='J'){
				i+=6;
				XPINSScriptableMath::Vector* param1=XPINSParser::parseVecArg(scriptText, params,vars, &i, ',');
				float param2=parseFloatArg(scriptText, params,vars, &i, ')');
				retVal=XPINSScriptableMath::Vector::projectionInDirection(param1,param2);
				delete param1;
			}
			else retVal=new XPINSScriptableMath::Vector(0,0);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
XPINSCustomStruct XPINSParser::parsePointerArg(string scriptText,XPINSParams* params,XPINSParser::varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	XPINSCustomStruct retVal=XPINSCustomStruct();
	while (scriptText[i]!='$'&&scriptText[i]!='#') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readVarIndex(scriptText, &i, expectedEnd);
		retVal=vars->pVars[index];
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='P'||scriptText[i+2]!='F'){
		}
		else{
			i+=3;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	return retVal;
}
//Check to make sure script is compatible
// script: the script
bool checkVersion(string script){
	for(int i=0;i<script.length();i++){
		if(script[i]=='@'&&script[i+1]=='P'&&script[i+2]=='A'&&script[i+3]=='R'&&script[i+4]=='S'&&script[i+5]=='E'&&script[i+6]=='R'){
			while(script[i]!='[')i++;
			int MAJOR=readVarIndex(script, &i, '.');
			int MINOR=readVarIndex(script, &i, ']');
			if(MAJOR!=kPMajor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			if(MINOR<kPMinor){
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
void XPINSParser::parseScript(string scriptText,XPINSParams *parameters,XPINSParser::varSpace *vars,bool isRECURSIVE,int start,int stop){
//	cout<<scriptText.length()<<endl<<endl;
	//SET UP VAR SPACE
	bool initialized_varSpace=false;
	if(vars==NULL){
		vars=new varSpace;
		vars->bVars=vector<bool>();
		vars->iVars=vector<int>();
		vars->fVars=vector<float>();
		vars->vVars=vector<XPINSScriptableMath::Vector*>();
		vars->pVars=vector<XPINSCustomStruct>();
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
		++i;
	}
	while(true){
		//get to next line
		while(i<scriptText.length()&&scriptText[i++]!='\n'){}
		if (i>=scriptText.length())break;
		//cout<<scriptText[i];
		//reaching the end of the Script
		if(!(scriptText[i]!='@'||
			scriptText[i+1]!='E'||
			scriptText[i+2]!='N'||
			 scriptText[i+3]!='D')||
		   (isRECURSIVE&&i>=stop)){
			break;
		}
		//Declaring new vars
		if(scriptText[i]=='B'){
			vars->bVars.resize(vars->bVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if(scriptText[i]=='I'){
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
				i++;
				vars->bVars[index]=parseBoolArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='I'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				i++;
				vars->iVars[index]=parseIntArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='F'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				i++;
				vars->fVars[index]=parseFloatArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='V'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				i++;
				if(vars->vVars.size()>index+1) vars->vVars.at(index);
				vars->vVars[index]=XPINSParser::parseVecArg(scriptText, parameters,vars, &i,'\n');
			}
			else if(scriptText[i]=='P'){
				i++;
				int index=readVarIndex(scriptText, &i, '=');
				i++;
				if(vars->pVars.size()>index+1||vars->pVars[index].shouldDelete) delete vars->pVars[index].obj;
				vars->pVars[index]=XPINSParser::parsePointerArg(scriptText, parameters, vars, &i,'\n');
			}
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='#'){
			if (scriptText[i+1]!='F') {
				printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
			}
			i+=2;
			int fNum=readVarIndex(scriptText, &i, '(');
			XPINSBridge::bridgeFunction(fNum, scriptText,parameters, vars,  &i, 0);
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='@'){
			i++;
			//IF STATEMENT
			if(scriptText[i]=='I'&&scriptText[i+1]=='F'){
				while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') i++;
				if(scriptText[i+1]!='B'&&scriptText[i]!='^'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING IF/ELSE\n");
					while (scriptText[i]!='{')i++;
					--i;
					int num=0;
					bool found=false;
					//skip if block
					while(!found||num>0){
						if(i>=scriptText.length())break;
						if(scriptText[i]=='{'){
							num++;found=true;}
						else if(scriptText[i]=='}')
							num--;
						i++;
					}
					while (scriptText[i]!='\n')i++;
					//skiip Else
					if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
						while (scriptText[i]!='{')i++;
						--i;
						int num=0;
						bool found=false;
						//skip if block
						while(!found||num>0){
							if(i>=scriptText.length())break;
							if(scriptText[i]=='{'){
								num++;found=true;}
							else if(scriptText[i]=='}')
								num--;
							i++;
						}
					}
				}
				else{
					if(!XPINSParser::parseBoolArg(scriptText, parameters, vars, &i, ']')){
						while (scriptText[i]!='{')i++;
						--i;
						int num=0;
						bool found=false;
						//skip if block
						while(!found||num>0){
							if(i>=scriptText.length())break;
							if(scriptText[i]=='{'){
								num++;found=true;}
							else if(scriptText[i]=='}')
								num--;
							i++;
						}
						while (scriptText[i]!='\n')i++;
						++i;
						//execute else if applicable
						if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
							while (scriptText[i]!='{')i++;
						}
					}
					else{
						while (scriptText[i]!='{')i++;
					}
				}
			}
			//WHILE LOOP
			else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E'){
				while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') i++;
				if(scriptText[i+1]!='B'&&scriptText[i]!='^'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
				}
				else{
					int expIndex=i--;
					while (scriptText[++i]!='{'){};
					int loopStart=i;
					int num=0;
					bool found=false;
					//skip if block
					while(!found||num>0){
						if(i>=scriptText.length())break;
						if(scriptText[i]=='{'){
							num++;found=true;}
						else if(scriptText[i]=='}')
							num--;
						i++;
					}
					int loopStop=i;
				//	for(int k=loopStart;k<=loopStop;++k)cout<<scriptText[k];
					int temp=expIndex;
					while (XPINSParser::parseBoolArg(scriptText, parameters, vars, &expIndex, ']')) {
						expIndex=temp;
						XPINSParser::parseScript(scriptText, parameters, vars, true, loopStart, loopStop);
					}
				}
			}
			//ELSE (BYPASS because IF was executed)
			else if(scriptText[i]=='E'&&scriptText[i+1]=='L'&&scriptText[i+2]=='S'&&scriptText[i+3]=='E'){
				while (scriptText[i]!='{')i++;
				--i;
				int num=0;
				bool found=false;
				//skip if block
				while(!found||num>0){
					if(i>=scriptText.length())break;
					if(scriptText[i]=='{'){
						num++;found=true;}
					else if(scriptText[i]=='}')
						num--;
					i++;
				}
				
			}
			if(i>=scriptText.length())break;
		}
	}
	//wipe declared variables
	if(initialized_varSpace) {
		vars->bVars.resize(0);
		vars->iVars.resize(0);
		vars->fVars.resize(0);
		for (int j=vars->vVars.size()-1; j>=0; ++j) {
			delete vars->vVars[j];
		}
		vars->vVars.resize(0);
		for (int j=vars->pVars.size()-1; j>=0; ++j) {
			if(vars->pVars[j].shouldDelete)delete vars->pVars[j].obj;
		}
		vars->pVars.resize(0);
		delete vars;
	}
	else{
		vars->bVars.resize(bSize);
		vars->iVars.resize(iSize);
		vars->fVars.resize(fSize);
		for (int j=vars->vVars.size()-1; j>=vSize; ++j) {
			delete vars->vVars[j];
		}
		vars->vVars.resize(vSize);
		for (int j=vars->pVars.size()-1; j>=pSize; ++j) {
			if(vars->pVars[j].shouldDelete)delete vars->pVars[j].obj;
		}
		vars->pVars.resize(pSize);
	}


}