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
		if (scriptText[i]=='!') {//not
			++i;
			bool arg=ParseBoolArg(scriptText, data, localBindings, i, ')',NULL);
			++i;
			return !arg;
		}
		else {//two inputs
			int j=i;
			int expCount=0;
			while ((scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!')||expCount!=0)
			{
				if(scriptText[j]=='?')
				{
					if(scriptText[j+2]=='(')++expCount;
					else --expCount;
				}
				++j;
			}
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=ParseBoolArg(scriptText, data, localBindings, i,'|',NULL);
				i+=2;
				bool arg2=ParseBoolArg(scriptText,data, localBindings, i,')',NULL);
				++i;
				return arg1||arg2;
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=ParseBoolArg(scriptText, data, localBindings, i,'|',NULL);
				i+=2;
				bool arg2=ParseBoolArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1&&arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=ParseFloatArg(scriptText, data, localBindings, i,'<',NULL);
				++i;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=arg1=ParseFloatArg(scriptText, data, localBindings, i,'<',NULL);
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=ParseFloatArg(scriptText, data, localBindings, i,'>',NULL);
				++i;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=ParseFloatArg(scriptText, data, localBindings, i,'>',NULL);
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=ParseFloatArg(scriptText, data, localBindings, i,'!',NULL);
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=ParseFloatArg(scriptText, data, localBindings, i,'=',NULL);
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				return arg1==arg2;
			}
		}
	}
	return false;
}
int XPINSBuiltIn::ParseIntExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	int result=0;
	int varIndex=-1;
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
					j+=2;
					bool floatVar=scriptText[i+1]!='I';
					result=ParseIntArg(scriptText, data, localBindings, j,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++data->fVars[varIndex];
						else ++data->iVars[varIndex];
					}
					return result+1;
				}
				else
				{
					bool floatVar=scriptText[i+1]!='I';
					result=ParseIntArg(scriptText, data, localBindings, i,'+',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++data->fVars[varIndex];
						else ++data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool floatVar=scriptText[i+1]!='I';
				result=ParseIntArg(scriptText, data, localBindings, i,'+',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result+=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
				{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[j+1]=='-'){
				if(scriptText[j+2]=='$')
				{
					j+=2;
					bool floatVar=scriptText[i+1]!='I';
					result=ParseIntArg(scriptText, data, localBindings, j,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--data->fVars[varIndex];
						else --data->iVars[varIndex];
					}
					return result-1;
				}
				else
				{
					bool floatVar=scriptText[i+1]!='I';
					result=ParseIntArg(scriptText, data, localBindings, i,'-',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--data->fVars[varIndex];
						else --data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool floatVar=scriptText[i+1]!='I';
				result=ParseIntArg(scriptText, data, localBindings, i,'-',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result-=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
				{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (scriptText[j]=='*') {//multiplication
			bool floatVar=scriptText[i+1]!='I';
			result=ParseIntArg(scriptText, data, localBindings, i, '*',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result*=ParseIntArg(scriptText,data,localBindings, i, ')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
			{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
			}
			return result;
		}
		else if (scriptText[j]=='/') {//division
			bool floatVar=scriptText[i+1]!='I';
			result=ParseIntArg(scriptText, data, localBindings, i, '/',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result/=ParseIntArg(scriptText,data,localBindings, i, ')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
			{
				if(floatVar)data->fVars[varIndex]=result;
				else data->iVars[varIndex]=result;
			}
			return result;
		}
		else if (scriptText[j]=='%') {//modulus
			bool floatVar=scriptText[i+1]!='I';
			result=ParseIntArg(scriptText, data, localBindings, i, '%',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
			result%=ParseIntArg(scriptText,data,localBindings, i, ')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
			{
				if(floatVar)data->fVars[varIndex]=result;
				else data->iVars[varIndex]=result;
			}
			return result;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	float result=0.0;
	int varIndex=-1;
	while(scriptText[i++]!='?');
	if(scriptText[i]!='F'){
	//	cerr<<"Could not parse XPINS expression, returning 0.0\n";
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
					j+=2;
					bool floatVar=scriptText[i+1]!='I';
					result=ParseFloatArg(scriptText, data, localBindings, j,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++data->fVars[varIndex];
						else ++data->iVars[varIndex];
					}
					return result+1;
				}
				else
				{
					bool floatVar=scriptText[i+1]!='I';
					result=ParseFloatArg(scriptText, data, localBindings, i,'+',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++data->fVars[varIndex];
						else ++data->iVars[varIndex];
					}
					return result;
				}

			}
			else
			{
				bool floatVar=scriptText[i+1]!='I';
				result=ParseIntArg(scriptText, data, localBindings, i,'+',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result+=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
				{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[j+1]=='-'){
				if(scriptText[j+2]=='$')
				{
					j+=2;
					bool floatVar=scriptText[i+1]!='I';
					result=ParseFloatArg(scriptText, data, localBindings, j,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--data->fVars[varIndex];
						else --data->iVars[varIndex];
					}
					return result-1;
				}
				else
				{
					bool floatVar=scriptText[i+1]!='I';
					result=ParseFloatArg(scriptText, data, localBindings, i,'-',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--data->fVars[varIndex];
						else --data->iVars[varIndex];
					}
					return result;
				}
				
			}
			else
			{
				bool floatVar=scriptText[i+1]!='I';
				result=ParseIntArg(scriptText, data, localBindings, i,'-',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result-=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
				{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*',NULL);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;//Get to Key Character
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')',NULL);
				result=Vector::DotProduct(v1, v2);
				++i;
				return result;
			}
			else if(scriptText[j+2]=='F'){
				bool floatVar=scriptText[i+1]!='I';
				result=ParseIntArg(scriptText, data, localBindings, i,'*',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result-=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
				{
					if(floatVar)data->fVars[varIndex]=result;
					else data->iVars[varIndex]=result;
				}
				return result;			}
		}
		else if (scriptText[j]=='/') {//division
			bool floatVar=scriptText[i+1]!='I';
			result=ParseIntArg(scriptText, data, localBindings, i,'/',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result/=ParseIntArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
			{
				if(floatVar)data->fVars[varIndex]=result;
				else data->iVars[varIndex]=result;
			}
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	int varIndex=-1;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Vector result=ParseVecArg(scriptText, data, localBindings, i,'+',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result+=ParseVecArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->vVars[varIndex]=result;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			Vector result=ParseVecArg(scriptText, data, localBindings, i,'-',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result+=ParseVecArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->vVars[varIndex]=result;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Matrix-Vector
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'*',NULL);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')',&varIndex);
				Vector result=m1*v2;
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
					data->vVars[varIndex]=result;
				return result;
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'||scriptText[j+3]=='<'){//Cross Produt
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')',NULL);
				Vector result=Vector::CrossProduct(v1, v2);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
					data->vVars[varIndex]=result;
				return result;
			}
			else{
				Vector result=ParseVecArg(scriptText, data, localBindings, i,'*',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result*=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
					data->vVars[varIndex]=result;
				return result;
			}
		}
		else if (scriptText[j]=='/') {//division
			Vector result=ParseVecArg(scriptText, data, localBindings, i,'/',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result/=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->vVars[varIndex]=result;
			return result;
		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(string scriptText,XPINSVarSpace* data, XPINSBindings* localBindings, int& i){
	int varIndex=-1;
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Matrix result=ParseMatArg(scriptText, data, localBindings, i,'+',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result+=ParseMatArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->mVars[varIndex]=result;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			Matrix result=ParseMatArg(scriptText, data, localBindings, i,'-',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result-=ParseMatArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->mVars[varIndex]=result;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Matrix-Matrix
				Matrix result=ParseMatArg(scriptText, data, localBindings, i,'*',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result*=ParseMatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
					data->mVars[varIndex]=result;
				return result;
			}
			else{
				Matrix result=ParseMatArg(scriptText, data, localBindings, i,'*',&varIndex);
				while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
				result*=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
				++i;
				if(scriptText[j+1]=='='&&varIndex>-1)
					data->mVars[varIndex]=result;
				return result;
			}
		}
		else if (scriptText[j]=='/') {//division
			Matrix result=ParseMatArg(scriptText, data, localBindings, i,'*',&varIndex);
			while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&	scriptText[i]!='X') ++i;//Get to Key Character
			result/=ParseFloatArg(scriptText, data, localBindings, i,')',NULL);
			++i;
			if(scriptText[j+1]=='='&&varIndex>-1)
				data->mVars[varIndex]=result;
			return result;

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
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ',',NULL);
			bool arg2=ParseBoolArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1&&arg2;
		}
		case 1://X_OR
		{
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ',',NULL);
			bool arg2=ParseBoolArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1||arg2;
		}
		case 2://X_NOT
		{
			bool arg1=ParseBoolArg(scriptText, data, localBindings, i, ')',NULL);
			return !arg1;
		}
		case 3://X_LESS
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1<arg2;
		}
		case 4://X_MORE
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1>arg2;
		}
		case 5://X_EQUAL
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
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
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',',NULL);
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1%arg2;
		}
		case 1://X_RAND
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',',NULL);
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
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
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1+arg2;
		}
		case 1://X_SUB
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1-arg2;
		}
		case 2://X_MULT
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1*arg2;
		}
		case 3://X_DIV
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1/arg2;
		}
		case 4://X_TSIN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return sinf(arg1);
		}
		case 5://X_TCOS
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return cosf(arg1);
		}
		case 6://X_TTAN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return tanf(arg1);
		}
		case 7://X_TATAN
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return atan2f(arg1, arg2);
		}
		case 8://X_POW
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return powf(arg1, arg2);
		}
		case 9://X_VADDPOLAR
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return addPolar(arg1, arg2);
		}
			//TBImplemented
		case 10://X_VDIST
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return dist(arg1, arg2,arg3);
		}
		case 11://X_VX
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return x;
		}
		case 12://X_VY
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return y;
		}
		case 13://X_VZ
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return z;
		}
		case 14://X_VR
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VTHETA
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VRHO
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 17://X_VPHI
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 18://X_VDOT
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::DotProduct(arg1, arg2);
		}
		case 19://X_VANG
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::AngleBetweenVectors(arg1, arg2);

		}
		case 20://X_MGET
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ',',NULL);
			int arg3=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 21://X_MDET
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
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
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector(arg1,arg2,arg3);
		}
		case 1://X_VPOL
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::PolarVector(arg1,arg2,arg3);
		}
		case 2://X_VSHPERE
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::SphericalVector(arg1,arg2,arg3);
		}
		case 3://X_VADD
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::Add(arg1,arg2);
		}
		case 4://X_VSUB
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			Vector temp=Vector::Scale(arg2, -1);
			return Vector::Add(arg1,temp);
		}
		case 5://X_VSCALE
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::Scale(arg1,arg2);
		}
		case 6://X_VPROJ
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg3=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 7://X_VCROSS
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			return Vector::CrossProduct(arg1,arg2);
		}
		case 8://X_MMTV
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::VectorForMatrix(arg1);

		}
		case 9://X_MVMULT
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
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
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ',',NULL);
			int arg2=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix(arg1,arg2);
		}
		case 1://X_MID
		{
			int arg1=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 2://X_MROT
		{
			float arg1=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			Vector arg2=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 3://X_MADD
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::Add(arg1,arg2);
		}
		case 4://X_MSUB
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			Matrix temp=Matrix::Scale(arg2, -1);
			return Matrix::Add(arg1,temp);
		}
		case 5://X_MSCALE
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::Scale(arg1,arg2);
		}
		case 6://X_MMULT
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			Matrix arg2=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::Multiply(arg1,arg2);

		}
		case 7://X_MINV
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::Invert(arg1);
		}
		case 8://X_MTRANS
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ')',NULL);
			return Matrix::Transpose(arg1);
		}
		case 9://X_MVTM
		{
			Vector arg1=ParseVecArg(scriptText, data, localBindings, i, ')',NULL);
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
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',',NULL);
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',',NULL);
			int arg3=ParseIntArg(scriptText, data, localBindings, i, ',',NULL);
			int arg4=ParseIntArg(scriptText, data, localBindings, i, ')',NULL);
			arg1.SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}
