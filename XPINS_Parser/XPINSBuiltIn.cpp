//
//  XPINSBuiltIn.cpp
//  XPINS
//
//  Created by Robbie Markwick on 3/23/14.
//
//

#include "XPINSParser.h"
#include "XPINSBindings.h"
#include <math.h>


using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;


#pragma mark Expression Processing

void XPINSBuiltIn::ParseVoidExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i)
{
	switch (scriptText[i+1]) {
		case 'B':
			ParseBoolExp(scriptText, data, localBindings, i);
			break;
		case 'I':
			ParseIntExp(scriptText, data, localBindings, i);
			break;
		case 'F':
			ParseFloatExp(scriptText, data, localBindings, i);
			break;
		case 'V':
			ParseVecExp(scriptText, data, localBindings, i);
			break;
		case 'M':
			ParseMatExp(scriptText, data, localBindings, i);
			break;
	}
}

bool XPINSBuiltIn::ParseBoolExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='B'){
		//cerr<<"Could not parse XPINS expression, returning false\n";
		return false;
	}
	else{
		i+=2;
		if (scriptText[i]=='!'&&scriptText[i+1]=='=') {//not
			++i;
			bool arg=ParseBoolArg(scriptText, data, localBindings, i, ')');
			++i;
			return !arg;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=ParseBoolArg(scriptText, data, localBindings, i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText,data, localBindings, i,')');
				++i;
				return arg1||arg2;
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=ParseBoolArg(scriptText, data, localBindings, i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText, data, localBindings, i,')');
				++i;
				return arg1&&arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, localBindings, i,'<');
				else
					arg1=ParseIntArg(scriptText, data, localBindings, i, '<');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=arg1=ParseFloatArg(scriptText, data, localBindings, i,'<');
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				++i;
				return arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, localBindings, i,'>');
				else
					arg1=ParseIntArg(scriptText, data, localBindings, i, '>');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, localBindings, i,'>');
				else
					arg1=ParseIntArg(scriptText, data, localBindings, i, '>');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, localBindings, i,'!');
				else
					arg1=ParseIntArg(scriptText, data, localBindings, i, '!');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, localBindings, i,'=');
				else
					arg1=ParseIntArg(scriptText, data, localBindings, i, '=');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return arg1==arg2;
			}
		}
	}
	return false;
}
int XPINSBuiltIn::ParseIntExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	int result=0;
	while(scriptText[i++]!='?');
	if(scriptText[i]!='I'){
		cerr<<"Could not parse XPINS expression, returning 0\n";
		return 0;
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/'&&scriptText[j]!='%') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[j+1]=='+'){
				if(scriptText[j+2]=='$')
				{
					int k=j+1;
					result=ParseIntArg(scriptText, data, localBindings, k,')');
					k=j+2;
					SetNumVar(result+1, scriptText, data, k, ')');
					return result;
				}
				else
				{
					int k=i;
					result=ParseIntArg(scriptText, data, localBindings, k,'+');
					SetNumVar(++result, scriptText, data, i, '+');
					return result;
				}
			}
			else
			{
				int k=i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseIntArg(scriptText, data, localBindings, i,'+');
				else
					result=ParseIntArg(scriptText, data, localBindings, i, '+');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result+=ParseIntArg(scriptText, data, localBindings, i,')');
				else
					result+=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '+');
				return result;
			}
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[j+1]=='-'){
				if(scriptText[j+2]=='$')
				{
					int k=j+2;
					result=ParseIntArg(scriptText, data, localBindings, k,')');
					k=j+2;
					SetNumVar(result-1, scriptText, data, k, ')');
					return result;
				}
				else
				{
					int k=i;
					result=ParseIntArg(scriptText, data, localBindings, k,'+');
					SetNumVar(--result, scriptText, data, i, '+');
					return result;
				}
			}
			else
			{
				int k=i;
				result=ParseIntArg(scriptText, data, localBindings, i,'-');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				result-=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '-');
				return result;
			}
		}
		else if (scriptText[j]=='*') {//multiplication
			int k=i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'*');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '*');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result*=ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '*');
			return result;
		}
		else if (scriptText[j]=='/') {//division
			int k=i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'/');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '/');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result/=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '/');
			return result;
		}
		else if (scriptText[j]=='%') {//modulus
			int k=i;
			result=ParseIntArg(scriptText, data, localBindings, i, '%');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result%=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '%');
			return result;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	float result=0.0;
	
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='F'){
		cerr<<"Could not parse XPINS expression, returning 0.0\n";
		return ParseIntExp(scriptText, data, localBindings, i);
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[j+1]=='+'){
				if(scriptText[j+2]=='$')
				{
					int k=j+1;
					result=ParseIntArg(scriptText, data, localBindings, k,')');
					k=j+2;
					SetNumVar(result+1, scriptText, data, k, ')');
					return result;
				}
				else
				{
					int k=i;
					result=ParseIntArg(scriptText, data, localBindings, k,'+');
					SetNumVar(++result, scriptText, data, i, '+');
					return result;
				}
			}
			else
			{
			int k=i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText,data,localBindings, i,'+');
			else
				result=(float)ParseIntArg(scriptText,data,localBindings, i, '+');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				result+=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '+');
			return result;
			}
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[j+1]=='-'){
				if(scriptText[j+2]=='$')
				{
					int k=j+1;
					result=ParseIntArg(scriptText, data, localBindings, k,')');
					k=j+2;
					SetNumVar(result-1, scriptText, data, k, ')');
					return result;
				}
				else
				{
					int k=i;
					result=ParseIntArg(scriptText, data, localBindings, k,'+');
					SetNumVar(--result, scriptText, data, i, '+');
					return result;
				}
			}
			else
			{
			int k=i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'-');
			else
				result=(float)ParseIntArg(scriptText, data, localBindings, i, '-');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result-=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '-');
			return result;
			}
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'+');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				result=Vector::DotProduct(v1, v2);
				++i;
				return result;
			}
			else if(scriptText[j+2]=='F'){
				int k=i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseFloatArg(scriptText,data,localBindings, i,'*');
				else
					result=(float)ParseIntArg(scriptText, data, localBindings, i, '*');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					result*=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
				++i;
				if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '*');
				return result;
			}
		}
		else if (scriptText[j]=='/') {//division
			int k=i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'/');
			else
				result=(float)ParseIntArg(scriptText, data, localBindings, i, '/');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result/=(float)ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetNumVar(result, scriptText, data, k, '/');
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			int k=i;
			Vector result=ParseVecArg(scriptText, data, localBindings, i,'+');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result+=ParseVecArg(scriptText, data, localBindings, i,')');
			++i;
			if(scriptText[j+1]=='=')SetVecVar(result, scriptText, data, k, '*');
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			int k=i;
			Vector result=ParseVecArg(scriptText, data, localBindings, i,'-');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result-=ParseVecArg(scriptText, data, localBindings, i,')');
			++i;
			if(scriptText[j+1]=='=')SetVecVar(result, scriptText, data, k, '*');
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Matrix-Vector
				int k=i;
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'+');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				++i;
				Vector result=m1*v2;
				if(scriptText[j+1]=='=')SetVecVar(result, scriptText, data, k, '*');
				return result;
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'){//Cross Produt
				int k=i;
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				++i;
				if(scriptText[j+1]=='=')SetVecVar(Vector::CrossProduct(v1, v2), scriptText, data, k, '*');
				return Vector::CrossProduct(v1, v2);
			}
			else{
				int l=i;
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					k=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				v1*=k;
				if(scriptText[j+1]=='=')SetVecVar(v1, scriptText, data, l, '*');
				return v1;
			}
		}
		else if (scriptText[j]=='/') {//division
			int l=i;

			Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				k=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			v1*=1/k;
			if(scriptText[j+1]=='=')SetVecVar(v1, scriptText, data, l, '*');
			return v1;		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			int k=i;
			Matrix result=ParseMatArg(scriptText, data, localBindings, i,'+');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result+=ParseMatArg(scriptText, data, localBindings, i,')');
			++i;
			if(scriptText[j+1]=='=')SetMatVar(result, scriptText, data, k, '+');
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			int k=i;
			Matrix result=ParseMatArg(scriptText, data, localBindings, i,'+');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result-=ParseMatArg(scriptText, data, localBindings, i,')');
			++i;
			if(scriptText[j+1]=='=')SetMatVar(result, scriptText, data, k, '-');
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Matrix-Matrix
				int k=i;
				Matrix result=ParseMatArg(scriptText, data, localBindings, i,'+');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				result*=ParseMatArg(scriptText, data, localBindings, i,')');
				++i;
				if(scriptText[j+1]=='=')SetMatVar(result, scriptText, data, k, '*');
				return result;
			}
			else{
				int l=i;
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'*');
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					k=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				if(scriptText[j+1]=='=')SetMatVar(m1*k, scriptText, data, l, '*');
				return m1*k;
			}
		}
		else if (scriptText[j]=='/') {//division
			int l=i;
			Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'*');
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				m1*=1/ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				m1*=1/ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			if(scriptText[j+1]=='=')SetMatVar(m1, scriptText, data, l, '*');
			return m1;
		}
	}
	return Matrix();
}

#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	switch (fNum) {
		case 0://X_AND
		{
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ',');
			bool arg2=ParseBoolArg(scriptText, data, localBindings, i, ')');
			return arg1&&arg2;
		}
		case 1://X_OR
		{
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ',');
			bool arg2=ParseBoolArg(scriptText, data, localBindings, i, ')');
			return arg1||arg2;
		}
		case 2://X_NOT
		{
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ')');
			return !arg1;
		}
		case 3://X_LESS
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1<arg2;
		}
		case 4://X_MORE
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1>arg2;
		}
		case 5://X_EQUAL
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1==arg2;
		}
		
	}
	return false;
}
int XPINSBuiltIn::ParseIntBIF(int fNum,string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	
	switch (fNum) {
		case 0://X_MOD
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
			return arg1%arg2;
		}
		case 1://X_RAND
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
			return arc4random()%(arg2-arg1)+arg1;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatBIF(int fNum,string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	switch (fNum) {
		case 0://X_ADD
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1+arg2;
		}
		case 1://X_SUB
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1-arg2;
		}
		case 2://X_MULT
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1*arg2;
		}
		case 3://X_DIV
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return arg1/arg2;
		}
		case 4://X_TSIN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return sinf(arg1);
		}
		case 5://X_TCOS
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return cosf(arg1);
		}
		case 6://X_TTAN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return tanf(arg1);
		}
		case 7://X_TATAN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return atan2f(arg1, arg2);
		}
		case 8://X_POW
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return powf(arg1, arg2);
		}
		case 9://X_VADDPOLAR
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return addPolar(arg1, arg2);
		}
			//TBImplemented
		case 10://X_VDIST
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return dist(arg1, arg2,arg3);
		}
		case 11://X_VX
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return x;
		}
		case 12://X_VY
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return y;
		}
		case 13://X_VZ
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return z;
		}
		case 14://X_VR
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VTHETA
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VRHO
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 17://X_VPHI
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 18://X_VDOT
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Vector::DotProduct(arg1, arg2);
		}
		case 19://X_VANG
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Vector::AngleBetweenVectors(arg1, arg2);

		}
		case 20://X_MGET
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg3=ParseIntArg(scriptText, data, localBindings, i, ')');
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 21://X_MDET
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Determinant(arg1);
		}
			
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum,string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	
	switch (fNum) {
		case 0://X_VREC
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Vector(arg1,arg2,arg3);
		}
		case 1://X_VPOL
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Vector::PolarVector(arg1,arg2,arg3);
		}
		case 2://X_VSHPERE
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Vector::SphericalVector(arg1,arg2,arg3);
		}
		case 3://X_VADD
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Vector::Add(arg1,arg2);
		}
		case 4://X_VSUB
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			Vector temp=Vector::Scale(arg2, -1);
			return Vector::Add(arg1,temp);
		}
		case 5://X_VSCALE
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Vector::Scale(arg1,arg2);
		}
		case 6://X_VPROJ
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 7://X_VCROSS
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Vector::CrossProduct(arg1,arg2);
		}
		case 8://X_MMTV
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::VectorForMatrix(arg1);

		}
		case 9://X_MVMULT
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Matrix::MultiplyMatrixVector(arg1, arg2);
		}
	}
	return Vector();

}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	
	switch (fNum) {
		case 0://X_MMAKE
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')');
			return Matrix(arg1,arg2);
		}
		case 1://X_MID
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ')');
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 2://X_MROT
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',');
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 3://X_MADD
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Add(arg1,arg2);
		}
		case 4://X_MSUB
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')');
			Matrix temp=Matrix::Scale(arg2, -1);
			return Matrix::Add(arg1,temp);
		}
		case 5://X_MSCALE
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Scale(arg1,arg2);
		}
		case 6://X_MMULT
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Multiply(arg1,arg2);

		}
		case 7://X_MINV
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Invert(arg1);
		}
		case 8://X_MTRANS
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')');
			return Matrix::Transpose(arg1);
		}
		case 9://X_MVTM
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')');
			return Matrix::MatrixForVector(arg1);
		}
	}
	return Matrix();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum,string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings,int& i)
{
	switch (fNum) {
		case 0:{//X_PRINT
			string str=XPINSParser::ParseStrArg(scriptText, data, localBindings, i, ')');
			cout<<str;
		}break;
		case 1:{//X_MSET
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			int arg3=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg4=ParseIntArg(scriptText, data, localBindings, i, ')');
			arg1.SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}
