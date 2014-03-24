//
//  XPINSBuiltIn.cpp
//  XPINS
//
//  Created by Robbie Markwick on 3/23/14.
//
//

#include "XPINSParser.h"
#include "XPINSBridge.h"
#include <math.h>


using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;


#pragma mark Expression Processing
//Expression Parsing
bool XPINSBuiltIn::ParseBoolExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			*start=++i;
			return !arg;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=ParseBoolArg(scriptText, params,vars, &i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText,params,vars, &i,')');
				*start=++i;
				return arg1||arg2;
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=ParseBoolArg(scriptText, params,vars, &i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText, params,vars, &i,')');
				*start=++i;
				return arg1&&arg2;
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
				*start=++i;
				return arg1<arg2;
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
				*start=++i;
				return arg1<=arg2;
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
				*start=++i;
				return arg1>arg2;
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
				*start=++i;
				return arg1>=arg2;
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
				*start=++i;
				return arg1!=arg2;
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
				*start=++i;
				return arg1==arg2;
			}
		}
	}
	return false;
}
int XPINSBuiltIn::ParseIntExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			*start=++i;
			return result;
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
			*start=++i;
			return result;
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
			*start=++i;
			return result;
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
			*start=++i;
			return result;
		}
		else if (scriptText[j]=='%') {//modulus
			result=ParseIntArg(scriptText, params,vars, &i, '%');
			++i;
			result%=ParseIntArg(scriptText, params,vars, &i, ')');
			*start=++i;
			return result;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			*start=++i;
			return result;
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
			*start=++i;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				result=Vector::DotProduct(v1, v2);
				*start=++i;
				return result;
			}
			else if(scriptText[j+2]=='V'){
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseFloatArg(scriptText,params, vars, &i,'*');
				else
					result=(float)ParseIntArg(scriptText, params,vars, &i, '*');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					result*=(float)ParseIntArg(scriptText,params, vars, &i, ')');
				*start=++i;
				return result;
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
			*start=++i;
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			*start=++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Vector v1=ParseVecArg(scriptText, params,vars, &i,'-');
			++i;
			Vector temp=ParseVecArg(scriptText, params,vars, &i,')');
			Vector v2=Vector::Scale(temp, -1);
			*start=++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				*start=++i;
				return Matrix::MultiplyMatrixVector(m1, v2);
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'){//Cross Produt
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, params,vars, &i,')');
				*start=++i;
				return Vector::CrossProduct(v1, v2);
			}
			else{
				Vector v1=ParseVecArg(scriptText, params,vars, &i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					k=ParseIntArg(scriptText, params,vars, &i, ')');
				*start=++i;
				return Vector::Scale(v1, k);
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
			*start=++i;
			return Vector::Scale(v1, 1/k);
		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(string scriptText,XPINSParams* params,varSpace* vars, int* start){
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
			*start=++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
			++i;
			Matrix temp=ParseMatArg(scriptText, params,vars, &i,')');
			++i;
			Matrix m2=Matrix::Scale(temp, -1);
			*start=++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'+');
				++i;
				Matrix m2=ParseMatArg(scriptText, params,vars, &i,')');
				++i;
				return Matrix::Add(m1, m2);
				
			}
			else{
				Matrix m1=ParseMatArg(scriptText, params,vars, &i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, params,vars, &i,')');
				else
					k=ParseIntArg(scriptText, params,vars, &i, ')');
				*start=++i;
				return Matrix::Scale(m1, k);
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
			*start=++i;
			return Matrix::Scale(m1, 1/k);
		}
	}
	return Matrix();
}

#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
	int i=*start;
	//XB_AND
	if(scriptText[i+1]=='A'&&scriptText[i+2]=='N'&&scriptText[i+3]=='D'){
		i+=4;
		bool param1=ParseBoolArg(scriptText, params,vars, &i, ',');
		bool param2=ParseBoolArg(scriptText, params,vars, &i, ')');
		return param1&&param2;
	}
	//XB_OR
	if(scriptText[i+1]=='O'&&scriptText[i+2]=='R'){
		i+=3;
		bool param1=ParseBoolArg(scriptText, params,vars, &i, ',');
		bool param2=ParseBoolArg(scriptText, params,vars, &i, ')');
		return param1||param2;
	}
	//XB_NOT
	if(scriptText[i+1]=='N'&&scriptText[i+2]=='O'&&scriptText[i+3]=='T'){
		i+=4;
		bool param1=ParseBoolArg(scriptText, params,vars, &i, ')');
		return !param1;
	}
	//XB_ILESS
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
		i+=6;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1<param2;
	}
	//XB_FLESS
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='L'&&scriptText[i+3]=='E'&&scriptText[i+4]=='S'&&scriptText[i+5]=='S'){
		i+=6;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1<param2;
		
	}
	//XB_IMORE
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
		i+=6;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1>param2;
	}
	//XB_FMORE
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='R'&&scriptText[i+5]=='E'){
		i+=6;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1>param2;
	}
	//XB_IEQUAL
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
		i+=7;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1==param2;
	}
	//XB_FEQUAL
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='E'&&scriptText[i+3]=='Q'&&scriptText[i+4]=='U'&&scriptText[i+5]=='A'&&scriptText[i+5]=='L'){
		i+=7;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1==param2;
	}
	return false;
}
int XPINSBuiltIn::ParseIntBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
	int i=*start;
	//XI_IADD
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
		i+=5;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1+param2;
	}
	//XI_ISUB
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
		i+=5;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params, vars, &i, ')');
		return param1-param2;
	}
	//XI_IMULT
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
		i+=6;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1*param2;
	}
	//XI_IDIV
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
		i+=5;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1/param2;
	}
	//XI_IMOD
	if(scriptText[i+1]=='I'&&scriptText[i+2]=='M'&&scriptText[i+3]=='O'&&scriptText[i+4]=='D'){
		i+=5;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1%param2;
	}
	//XI_RAND
	if(scriptText[i+1]=='R'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+3]=='D'){
		i+=5;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return arc4random()%(param2-param1)+param1;
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
	int i=*start;
	//XF_FADD
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1+param2;
	}
	//XF_FSUB
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1-param2;
	}
	//XF_FMULT
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+4]=='T'){
		i+=6;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1*param2;
	}
	//XF_FDIV
	if(scriptText[i+1]=='F'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='V'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return param1/param2;
	}
	//XF_VMAG
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='G'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		return param1.Magnitude();
	}
	//XF_VR
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'){
		i+=3;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		float f=0;
		param1.PolarCoords(&f, NULL,NULL);
		return f;
	}
	//XF_VTHETA
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='T'&&scriptText[i+3]=='H'&&scriptText[i+4]=='E'&&scriptText[i+5]=='T'&&scriptText[i+6]=='A'){
		i+=7;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		return param1.Direction();
	}
	//XF_VPHI
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='H'&&scriptText[i+4]=='I'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		return param1.Altitude();
	}
	//XF_VX
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='X'){
		i+=3;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		float f=0;
		param1.RectCoords(&f, NULL,NULL);
		return f;
	}
	//XF_VY
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='Y'){
		i+=3;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		float f=0;
		param1.RectCoords(NULL, &f,NULL);
		return f;
	}
	//XF_VZ
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='Z'){
		i+=3;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		float f=0;
		param1.RectCoords(NULL, NULL,&f);
		return f;
	}
	
	//XF_VANG
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='N'&&scriptText[i+4]=='G'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		return Vector::AngleBetweenVectors(param1, param2);
	}
	//XF_VDOT
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		return Vector::DotProduct(param1, param2);
	}
	//XF_VADDPOLAR
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'&&scriptText[i+5]=='P'&&scriptText[i+6]=='O'&&scriptText[i+7]=='L'&&scriptText[i+8]=='A'&&scriptText[i+9]=='R'){
		i+=10;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return addPolar(param1, param2);
	}
	//XF_VDIST
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='D'&&scriptText[i+3]=='I'&&scriptText[i+4]=='S'&&scriptText[i+5]=='T'){
		i+=6;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return dist(param1, param2);
	}
	//XF_TSIN
	if(scriptText[i+1]=='T'&&scriptText[i+2]=='S'&&scriptText[i+3]=='I'&&scriptText[i+4]=='N'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
		return sinf(param1);
	}
	//XF_TCOS
	if(scriptText[i+1]=='T'&&scriptText[i+2]=='C'&&scriptText[i+3]=='O'&&scriptText[i+4]=='S'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
		return cosf(param1);
	}
	//XF_TTAN
	if(scriptText[i+1]=='T'&&scriptText[i+2]=='T'&&scriptText[i+3]=='A'&&scriptText[i+4]=='N'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ')');
		return tanf(param1);
	}
	//XF_TATAN
	if(scriptText[i+1]=='T'&&scriptText[i+2]=='A'&&scriptText[i+3]=='T'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'){
		i+=6;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return atan2f(param1, param2);
	}
	//XF_POW
	if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
		i+=4;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return powf(param1, param2);
	}
	//XF_POW
	if(scriptText[i+1]=='P'&&scriptText[i+2]=='O'&&scriptText[i+3]=='W'){
		i+=4;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return powf(param1, param2);
	}
	//XF_MGET
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='G'&&scriptText[i+3]=='E'&&scriptText[i+4]=='T'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ',');
		int param3=ParseIntArg(scriptText, params,vars, &i, ')');
		return param1.ValueAtPosition(param2, param3);
		
	}
	//XF_MDET
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='D'&&scriptText[i+3]=='E'&&scriptText[i+4]=='T'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::Determinant(param1);
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
	int i=*start;
	
	//XV_VREC
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Vector(param1,param2,param3);
	}
	//XV_VPOL
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='R'&&scriptText[i+3]=='E'&&scriptText[i+4]=='C'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Vector::PolarVector(param1,param2,param3);
	}
	//XV_VSPHERE
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='H'&&scriptText[i+4]=='E'&&scriptText[i+5]=='R'&&scriptText[i+6]=='E'){
		i+=7;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Vector::SphericalVector(param1,param2,param3);
	}
	//XV_VADD
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		return Vector::Add(param1,param2);
	}
	//X_VSUB
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		Vector temp=Vector::Scale(param2, -1);
		return Vector::Add(param1,temp);
	}
	//X_VSCALE
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+5]=='L'&&scriptText[i+6]=='E'){
		i+=7;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Vector::Scale(param1,param2);
	}
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='C'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+5]=='S'&&scriptText[i+6]=='S'){
		i+=7;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		return Vector::CrossProduct(param1,param2);
	}
	//X_VPROJ
	if(scriptText[i+1]=='V'&&scriptText[i+2]=='P'&&scriptText[i+3]=='R'&&scriptText[i+4]=='O'&&scriptText[i+4]=='J'){
		i+=6;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ',');
		float param3=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Vector::ProjectionInDirection(param1,param2,param3);
	}
	//XV_MMTV
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='T'&&scriptText[i+4]=='V'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::VectorForMatrix(param1);
	}
	return Vector();
}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
	int i=*start;
	
	//XM_MMAKE
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='A'&&scriptText[i+4]=='K'&&scriptText[i+5]=='E'){
		i+=6;
		int param1=ParseIntArg(scriptText, params,vars, &i, ',');
		int param2=ParseIntArg(scriptText, params,vars, &i, ')');
		return Matrix(param1,param2);
	}
	//XM_MID
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='I'&&scriptText[i+3]=='D'){
		i+=4;
		int param1=ParseIntArg(scriptText, params,vars, &i, ')');
		return Matrix::IdentityMatrixOfSize(param1);
	}
	//XM_MROT
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='R'&&scriptText[i+3]=='O'&&scriptText[i+4]=='T'){
		i+=5;
		float param1=ParseFloatArg(scriptText, params,vars, &i, ',');
		Vector param2=ParseVecArg(scriptText, params,vars, &i, ')');
		return Matrix::RotationMatrixWithAngleAroundVector(param2,param1);
	}
	//XM_MVTM
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='V'&&scriptText[i+3]=='T'&&scriptText[i+4]=='M'){
		i+=5;
		Vector param1=ParseVecArg(scriptText, params,vars, &i, ')');
		return Matrix::MatrixForVector(param1);
	}
	//XM_MADD
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='A'&&scriptText[i+3]=='D'&&scriptText[i+4]=='D'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		Matrix param2=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::Add(param1,param2);
		
	}
	//XM_MSUB
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='S'&&scriptText[i+3]=='U'&&scriptText[i+4]=='B'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		Matrix temp=ParseMatArg(scriptText, params,vars, &i, ')');
		Matrix param2=Matrix::Scale(temp, -1);
		return Matrix::Add(param1,param2);
		
	}
	//XM_MSCALE
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='S'&&scriptText[i+3]=='C'&&scriptText[i+4]=='A'&&scriptText[i+4]=='L'&&scriptText[i+4]=='E'){
		i+=6;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		float param2=ParseFloatArg(scriptText, params,vars, &i, ')');
		return Matrix::Scale(param1,param2);
		
	}
	//XM_MMULT
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='M'&&scriptText[i+3]=='U'&&scriptText[i+4]=='L'&&scriptText[i+5]=='T'){
		i+=6;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ',');
		Matrix param2=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::Multiply(param1,param2);
	}
	//XM_MINV
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='I'&&scriptText[i+3]=='N'&&scriptText[i+4]=='V'){
		i+=5;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::Invert(param1);
		
	}
	//XM_MTRANS
	if(scriptText[i+1]=='M'&&scriptText[i+2]=='T'&&scriptText[i+3]=='R'&&scriptText[i+4]=='A'&&scriptText[i+5]=='N'&&scriptText[i+6]=='S'){
		i+=7;
		Matrix param1=ParseMatArg(scriptText, params,vars, &i, ')');
		return Matrix::Transpose(param1);
	}
	return Matrix();

}
void ParseVoidBIF(string scriptText,XPINSParams* params,varSpace* vars,int* start)
{
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
}








