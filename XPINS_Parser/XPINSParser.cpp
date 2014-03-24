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


//Helper functions

//Read an int constant
//Can be used to parse script as well (accessing variable indexes)
// scriptText: the script
// startIndex: the starting index of the INT
// expectedEnd: the character expected immediately after the INT
int readInt(string scriptText,int *startIndex,char expectedEnd)
{
	int i=*startIndex;
	int index=0;
	bool isNeg=scriptText[i]=='-';//Make negative if approrpriate
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
float readFloat(string scriptText,int *startIndex,char expectedEnd)
{
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
	int index=readInt(scriptText, &i, expectedEnd);
	*startIndex=i;
	return index;
}

//Expression Parsing
bool parseBoolExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			bool arg=ParseBoolArg(scriptText, params,vars, &i, ')');
			result=!arg;
			++i;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=ParseBoolArg(scriptText, params,vars, &i,'|');
				if(arg1){
					result=true;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=ParseBoolArg(scriptText,params,vars, &i,')');
					result=arg1||arg2;
					++i;
				}
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=ParseBoolArg(scriptText, params,vars, &i,'|');
				if(!arg1){
					result=false;
					while (scriptText[++i]!=')') {}
					++i;
				}
				else{
					i+=2;
					bool arg2=ParseBoolArg(scriptText, params,vars, &i,')');
					result=arg1&&arg2;
					++i;
				}
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'<');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '<');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'<');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '<');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'>');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '>');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'>');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '>');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'!');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '!');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, params,vars, &i,'=');
				else
					arg1=ParseIntArg(scriptText, params,vars, &i, '=');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					arg2=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=arg1==arg2;
			}
		}
	}
	*start=i;
	return result;
}
int parseIntExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
				result=ParseFloatArg(scriptText, params,vars, &i,'+');
			else
				result=ParseIntArg(scriptText, params,vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result+=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, params,vars, &i,'-');
			else
				result=ParseIntArg(scriptText, params,vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result-=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, params,vars, &i,'*');
			else
				result=ParseIntArg(scriptText, params,vars, &i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result*=ParseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, params,vars, &i,'/');
			else
				result=ParseIntArg(scriptText, params,vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result/=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='%') {//modulus
			result=ParseIntArg(scriptText, params,vars, &i, '%');
			++i;
			result%=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}
float parseFloatExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
				result=ParseFloatArg(scriptText,params, vars, &i,'+');
			else
				result=(float)ParseIntArg(scriptText,params, vars, &i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText,params, vars, &i,')');
			else
				result+=(float)ParseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, params,vars, &i,'-');
			else
				result=(float)ParseIntArg(scriptText, params,vars, &i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result-=(float)ParseIntArg(scriptText,params, vars, &i, ')');
			++i;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				++i;
				result=Vector::DotProduct(v1, v2);
			}
			else{
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseFloatArg(scriptText,params, vars, &i,'*');
				else
					result=(float)ParseIntArg(scriptText, params,vars, &i, '*');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					result*=(float)ParseIntArg(scriptText,params, vars, &i, ')');
				++i;
			}
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, params,vars, &i,'/');
			else
				result=(float)ParseIntArg(scriptText, params,vars, &i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, params,vars, &i,')');
			else
				result/=(float)ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
		}
	}
	*start=i;
	return result;
}
Vector parseVecExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
	Vector result=Vector();
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Vector v1=ParseVecArg(scriptText, params,vars, &i,'+');
			++i;
			Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
			++i;
			result=Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Vector v1=ParseVecArg(scriptText, params,vars, &i,'-');
			++i;
			Vector temp=ParseVecArg(scriptText, params,vars, &i,')');
			Vector v2=Vector::Scale(temp, -1);
			++i;
			result=Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				++i;
				result=Matrix::MultiplyMatrixVector(m1, v2);
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'){//Cross Produt
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				++i;
				result=Vector::CrossProduct(v1, v2);
			}
			else{
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					k=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=Vector::Scale(v1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Vector v1=ParseVecArg(scriptText, params, vars, &i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,params, vars, &i,')');
			else
				k=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
			result=Vector::Scale(v1, 1/k);
		}
	}
	*start=i;
	return result;
}
Matrix parseMatExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
	Matrix result=Matrix();
	int i=*start;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
			++i;
			Matrix m2=ParseMatArg(scriptText, params,vars, &i,')');
			++i;
			result=Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
			++i;
			Matrix temp=ParseMatArg(scriptText, params,vars, &i,')');
			++i;
			Matrix m2=Matrix::Scale(temp, -1);
			result=Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
				++i;
				Matrix m2=ParseMatArg(scriptText, params,vars, &i,')');
				++i;
				result=Matrix::Add(m1, m2);

			}
			else{
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					k=ParseIntArg(scriptText, params,vars, &i, ')');
				++i;
				result=Matrix::Scale(m1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Matrix m1=ParseMatArg(scriptText, params, vars, &i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,params, vars, &i,')');
			else
				k=ParseIntArg(scriptText, params,vars, &i, ')');
			++i;
			result=Matrix::Scale(m1, 1/k);
		}
	}
	*start=i;
	return result;
}
//parameter parsing
bool XPINSParser::ParseBoolArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	bool retVal=false;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if (i>=scriptText.length()) {
		return false;
	}
	if(scriptText[i]=='$'){//Variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
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
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText, params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='B'){
			retVal=false;
		}
		//XB_AND
		else if(scriptText[i+1]=='A'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D'){
			i+=4;
			bool param1=ParseBoolArg(scriptText, params,vars, &i, ',');
			bool param2=ParseBoolArg(scriptText, params,vars, &i, ')');
			retVal=param1&&param2;
		}
		//XB_OR
		else if(scriptText[i+1]=='O'&&scriptText[i+2]=='R'){
			i+=3;
			bool param1=ParseBoolArg(scriptText, params,vars, &i, ',');
			bool param2=ParseBoolArg(scriptText, params,vars, &i, ')');
			retVal=param1||param2;
		}
		//XB_NOT
		else if(scriptText[i+1]=='N'&&scriptText[i+2]=='O'&&scriptText[i+3]=='T'){
			i+=4;
			bool param1=ParseBoolArg(scriptText, params,vars, &i, ')');
			retVal=!param1;
		}
		//XB_ILESS
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
			i+=6;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1<param2;
		}
		//XB_FLESS
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
			i+=6;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1<param2;
			
		}
		//XB_IMORE
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
			i+=6;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1>param2;
		}
		//XB_FMORE
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
			i+=6;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1>param2;
		}
		//XB_IEQUAL
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
			i+=7;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1==param2;
		}
		//XB_FEQUAL
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
			i+=7;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
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
int XPINSParser::ParseIntArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	int retVal=0;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
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
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='I'){
			retVal=0;
		}
		//XI_IADD
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
			i+=5;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1+param2;
		}
		//XI_ISUB
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
			i+=5;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params, vars, &i, ')');
			retVal=param1-param2;
		}
		//XI_IMULT
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
			i+=6;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1*param2;
		}
		//XI_IDIV
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
			i+=5;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1/param2;
		}
		//XI_IMOD
		else if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='D'){
			i+=5;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1%param2;
		}
		//XI_RAND
		else if(scriptText[i+1]=='R'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+3]=='D'){
			i+=5;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=arc4random()%(param2-param1)+param1;
		}
		else retVal=0;
	}
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
float XPINSParser::ParseFloatArg(string scriptText,XPINSParams* params, varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
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
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='F'){
			retVal=0;
		}
		//XF_FADD
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1+param2;
		}
		//XF_FSUB
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1-param2;
		}
		//XF_FMULT
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
			i+=6;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1*param2;
		}
		//XF_FDIV
		else if(scriptText[i+1]=='F'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=param1/param2;
		}
		//XF_VMAG
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='G'){
			i+=5;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=param1.Magnitude();
		}
		//XF_VR
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'){
			i+=3;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1.PolarCoords(&f, NULL,NULL);
			retVal=f;
		}
		//XF_VTHETA
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='T'&&scriptText[i+3]=='H'&&scriptText[i+4]=='E'&&scriptText[i+5]=='T'&&scriptText[i+6]=='A'){
			i+=7;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=param1.Direction();
		}
		//XF_VPHI
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='H'&&scriptText[i+4]=='I'){
			i+=5;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=param1.Altitude();
		}
		//XF_VX
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='X'){
			i+=3;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1.RectCoords(&f, NULL,NULL);
			retVal=f;
		}
		//XF_VY
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='Y'){
			i+=3;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1.RectCoords(NULL, &f,NULL);
			retVal=f;
		}
		//XF_VZ
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='Z'){
			i+=3;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			float f=0;
			param1.RectCoords(NULL, NULL,&f);
			retVal=f;
		}

		//XF_VANG
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+4]=='G'){
			i+=5;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
			Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=Vector::AngleBetweenVectors(param1, param2);
		}
		//XF_VDOT
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
			i+=5;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
			Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=Vector::DotProduct(param1, param2);
		}
		//XF_VADDPOLAR
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'&&scriptText[i+5]=='P'&&scriptText[i+6]=='O'&&scriptText[i+7]=='L'&&scriptText[i+8]=='A'&&scriptText[i+9]=='R'){
			i+=10;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=addPolar(param1, param2);
		}
		//XF_VDIST
		else if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='S'&&scriptText[i+5]=='T'){
			i+=6;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=dist(param1, param2);
		}
		//XF_TSIN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='S'&&scriptText[i+3]=='I'&&scriptText[i+4]=='N'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=sinf(param1);
		}
		//XF_TCOS
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='C'&&scriptText[i+3]=='O'&&scriptText[i+4]=='S'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=cosf(param1);
		}
		//XF_TTAN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='T'&&scriptText[i+3]=='A'&&scriptText[i+4]=='N'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=tanf(param1);
		}
		//XF_TATAN
		else if(scriptText[i+1]=='T'&&scriptText[i+2]=='A'&&scriptText[i+3]=='T'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'){
			i+=6;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=atan2f(param1, param2);
		}
		//XF_POW
		else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
			i+=4;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=powf(param1, param2);
		}
		//XF_POW
		else if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
			i+=4;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=powf(param1, param2);
		}
		//XF_MGET
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='G'&&scriptText[i+3]=='E'&&scriptText[i+4]=='T'){
			i+=5;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ',');
			int param3=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=param1.ValueAtPosition(param2, param3);
			
		}
		//XF_MDET
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='D'&&scriptText[i+3]=='E'&&scriptText[i+4]=='T'){
			i+=5;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Determinant(param1);
		}
		else retVal=0;
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
Vector XPINSParser::ParseVecArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){

	int i=*start;
	Vector retVal=Vector();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->vVars[index];
	}
	else if(scriptText[i]=='^'){//constant (can contain varialbes, though)
		++i;
		if(scriptText[i]=='P'&&scriptText[i+1]=='<'){//Polar Vector
				i+=2;
				float r=ParseFloatArg(scriptText, params,vars, &i, ',');
				float t=ParseFloatArg(scriptText, params,vars, &i, ',');
				float z=ParseFloatArg(scriptText, params,vars, &i, '>');
				retVal=Vector::PolarVector(r, t, z);
		}
		else if(scriptText[i]=='S'&&scriptText[i+1]=='<'){//Polar Vector
			i+=2;
			float r=ParseFloatArg(scriptText, params,vars, &i, ',');
			float t=ParseFloatArg(scriptText, params,vars, &i, ',');
			float p=ParseFloatArg(scriptText, params,vars, &i, '>');
			retVal=Vector::PolarVector(r, t ,p);
		}
		else if(scriptText[i]=='<'){//rectangular vector
			++i;
			float x=ParseFloatArg(scriptText, params,vars, &i, ',');
			float y=ParseFloatArg(scriptText, params,vars, &i, ',');
			float z=ParseFloatArg(scriptText, params,vars, &i, '>');
			retVal=Vector(x, y,z);
		}
		else{
			retVal=Vector();
		}
	}
	else if(scriptText[i]=='?'){
		retVal=parseVecExp(scriptText, params, vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]=='V'&&scriptText[i+2]=='F')
		{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='V'){
			retVal=Vector();
		}
		else //XV_VREC
			if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
				i+=5;
				float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
				retVal=Vector(param1,param2,param3);
			}
		//XV_VPOL
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
				i+=5;
				float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
				retVal=Vector::PolarVector(param1,param2,param3);
			}
		//XV_VSPHERE
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='H'&&scriptText[i+4]=='E'&&scriptText[i+5]=='R'&&scriptText[i+6]=='E'){
				i+=7;
				float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
				retVal=Vector::SphericalVector(param1,param2,param3);
			}
		//XV_VADD
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
				i+=5;
				Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
				Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
				retVal=Vector::Add(param1,param2);
			}
		//X_VSUB
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
				i+=5;
				Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
				Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
				Vector temp=Vector::Scale(param2, -1);
				retVal=Vector::Add(param1,temp);
			}
		//X_VSCALE
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+5]=='L'&&scriptText[i+6]=='E'){
				i+=7;
				Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
				float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
				retVal=Vector::Scale(param1,param2);
			}
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='C'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+5]=='S'&&scriptText[i+6]=='S'){
				i+=7;
				Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
				Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
				retVal=Vector::CrossProduct(param1,param2);
			}
		//X_VPROJ
			else if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+4]=='J'){
				i+=6;
				Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
				float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
				float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
				retVal=Vector::ProjectionInDirection(param1,param2,param3);
			}
		//XV_MMTV
			else if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='T'&&scriptText[i+4]=='V'){
				i+=5;
				Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
				retVal=Matrix::VectorForMatrix(param1);
			}
			else retVal=Vector();
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}

Matrix XPINSParser::ParseMatArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	
	int i=*start;
	Matrix retVal=Matrix();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->mVars[index];
	}
	else if(scriptText[i]=='^'){//constant (can contain varialbes, though)
		while (scriptText[i]!='[') ++i;
		size_t cols=1;
		for (int j=i; scriptText[j]!=';'&&scriptText[j]!=']'; ++j) {
			if(scriptText[j]==',')++cols;
		}
		size_t rows=1;
		for (int j=i; scriptText[j]!=']'; ++j) {
			if(scriptText[j]==';')++cols;
		}
		Matrix m=Matrix(rows,cols);
		int r=0,c=0;
		while (scriptText[i]!=']')
		{
			if(c==cols-1){
				if(r==rows-1)m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ']'),r,c);
				else m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ';'),r,c);
				++r;
				c=-1;
			}
			else m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ','),r,c);
			++c;
			++i;
		}
		return m;
	}
	else if(scriptText[i]=='?'){
		return Matrix();//retVal=parseVecExp(scriptText, params, vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]=='M'&&scriptText[i+2]=='F')
		{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='M'){
			retVal=Matrix();
		}
		//XM_MMAKE
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='K'&&scriptText[i+5]=='E'){
			i+=6;
			int param1=ParseIntArg(scriptText, params,vars, &i, ',');
			int param2=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=Matrix(param1,param2);
		}
		//XM_MID
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='I'&&scriptText[i+3]=='D'){
			i+=4;
			int param1=ParseIntArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::IdentityMatrixOfSize(param1);
		}
		//XM_MROT
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='R'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
			i+=5;
			float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
			Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::RotationMatrixWithAngleAroundVector(param2,param1);
		}
		//XM_MVTM
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='V'&&scriptText[i+3]=='T'&&scriptText[i+4]=='M'){
			i+=5;
			Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::MatrixForVector(param1);
		}
		//XM_MADD
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
			i+=5;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
			Matrix param2=ParseMatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Add(param1,param2);

		}
		//XM_MSUB
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
			i+=5;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
			Matrix temp=ParseMatArg(scriptText, params,vars, &i, ')');
			Matrix param2=Matrix::Scale(temp, -1);
			retVal=Matrix::Add(param1,param2);
			
		}
		//XM_MSCALE
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+4]=='L'&&scriptText[i+4]=='E'){
			i+=6;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
			float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Scale(param1,param2);
			
		}
		//XM_MMULT
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+5]=='T'){
			i+=6;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
			Matrix param2=ParseMatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Multiply(param1,param2);
		}
		//XM_MINV
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='I'&&scriptText[i+3]=='N'&&scriptText[i+4]=='V'){
			i+=5;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Invert(param1);
			
		}
		//XM_MTRANS
		else if(scriptText[i+1]=='M'&&scriptText[i+2]=='T'&&scriptText[i+3]=='R'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'&&scriptText[i+6]=='S'){
			i+=7;
			Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
			retVal=Matrix::Transpose(param1);
		}
		else retVal=Matrix();
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}

void* XPINSParser::ParsePointerArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	void* retVal=NULL;
	while (scriptText[i]!='$'&&scriptText[i]!='#') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->pVars[index];
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='P'||scriptText[i+2]!='F'){
		}
		else{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
void BuiltInVoidFunction(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	//XN_MSET
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='S'&&scriptText[i+3]=='E'&&scriptText[i+4]=='T'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
		int param3=ParseIntArg(scriptText, params,vars, &i, ',');
		int param4=ParseIntArg(scriptText, params,vars, &i, ')');
		param1.SetValueAtPosition(param2, param3, param4);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
}
//Check to make sure script is compatible
// script: the script
bool checkVersion(string script){
	for(int i=0;i<script.length();i++){
		if(script[i]=='@'&&script[i+1]=='P'&&script[i+2]=='A'&&script[i+3]=='R'&&script[i+4]=='S'&&script[i+5]=='E'&&script[i+6]=='R'){
			while(script[i]!='[')i++;
			int MAJOR=readInt(script, &i, '.');
			int MINOR=readInt(script, &i, ']');
			if(MAJOR!=kPMajor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			if(MINOR>kPMinor){
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
void XPINSParser::ParseScript(string scriptText,XPINSParams *parameters,varSpace *vars,bool isRECURSIVE,int start,int stop){
//	cout<<scriptText.length()<<endl<<endl;
	//SET UP VAR SPACE
	bool initialized_varSpace=false;
	if(vars==NULL){
		vars=new varSpace;
		vars->bVars=vector<bool>();
		vars->iVars=vector<int>();
		vars->fVars=vector<float>();
		vars->vVars=vector<Vector>();
		vars->pVars=vector<void*>();
		initialized_varSpace=true;
	}
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
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->bVars[index]=ParseBoolArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='I'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->iVars[index]=ParseIntArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='F'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->fVars[index]=ParseFloatArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='V'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->vVars[index]=ParseVecArg(scriptText, parameters,vars, &i,'\n');
			}
			else if(scriptText[i]=='P'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->pVars[index]=ParsePointerArg(scriptText, parameters, vars, &i,'\n');
			}
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='#'){
			if (scriptText[i+1]!='F') {
				printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
			}
			i+=2;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,parameters, vars,  &i, 0);
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='X'){
			i+=3;
			if(scriptText[i-1]!='_'||scriptText[i-2]!='N')
				BuiltInVoidFunction(scriptText, parameters, vars, &i,'\n');
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
					if(!ParseBoolArg(scriptText, parameters, vars, &i, ']')){
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
					while (ParseBoolArg(scriptText, parameters, vars, &expIndex, ']')) {
						expIndex=temp;
						ParseScript(scriptText, parameters, vars, true, loopStart, loopStop);
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
		vars->vVars.resize(0);
		vars->pVars.resize(0);
		delete vars;
	}
}