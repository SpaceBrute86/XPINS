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

void XPINSBuiltIn::ParseVoidExp(XPINSParser::XPINSScriptSpace& script)
{
	switch (script.instructions[script.index+1]) {
		case 'B':
			ParseBoolExp(script);
			break;
		case 'I':
			ParseIntExp(script);
			break;
		case 'F':
			ParseFloatExp(script);
			break;
		case 'V':
			ParseVecExp(script);
			break;
		case 'M':
			ParseMatExp(script);
			break;
	}
}

bool XPINSBuiltIn::ParseBoolExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	++script.index;
	if(script.currentChar()!='B'){
		//cerr<<"Could not parse XPINS expression, returning false\n";
		return false;
	}
	else{
		script.index+=2;
		if (script.currentChar()=='!') {//not
			++script.index;
			bool arg=ParseBoolArg(script, ')',NULL);
			++script.index;
			return !arg;
		}
		else {//two inputs
			int temp=script.index;
			int expCount=0;
			while ((script.currentChar()!='|'&&script.currentChar()!='&'&&script.currentChar()!='<'&&script.currentChar()!='='&&script.currentChar()!='>'&&script.currentChar()!='!')||expCount!=0)
			{
				if(script.currentChar()=='?')
				{
					if(script.instructions[script.index+2]=='(')++expCount;
					else --expCount;
				}
				++script.index;
			}
			if(script.matchesString("||")){//OR
				script.index=temp;
				bool arg1=ParseBoolArg(script,'|',NULL);
				script.index+=2;
				bool arg2=ParseBoolArg(script,')',NULL);
				++script.index;
				return arg1||arg2;
			}
			else if(script.matchesString("&&")){//And
				script.index=temp;
				bool arg1=ParseBoolArg(script,'|',NULL);
				script.index+=2;
				bool arg2=ParseBoolArg(script,')',NULL);
				++script.index;
				return arg1&&arg2;
			}
			else if(script.currentChar()=='<'&&!script.matchesString("<=")){//Less Than
				script.index=temp;
				float arg1=ParseFloatArg(script,'<',NULL);
				++script.index;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1<arg2;
			}
			else if(script.matchesString("<=")){//Less Than or Equal
				script.index=temp;
				float arg1=arg1=ParseFloatArg(script,'<',NULL);
				script.index+=2;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1<=arg2;
			}
			else if(script.currentChar()=='>'&&!script.matchesString(">=")){//Greater Than
				script.index=temp;
				float arg1=ParseFloatArg(script,'>',NULL);
				++script.index;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1>arg2;
			}
			else if(script.matchesString(">=")){//Greater than Or equal
				script.index=temp;
				float arg1=ParseFloatArg(script,'>',NULL);
				script.index+=2;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1>=arg2;
			}
			else if(script.matchesString("!=")){//Not Equal
				script.index=temp;
				float arg1=ParseFloatArg(script,'!',NULL);
				script.index+=2;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1!=arg2;
			}
			else if(script.matchesString("==")){//Equal
				script.index=temp;
				float arg1=ParseFloatArg(script,'=',NULL);
				script.index+=2;
				float arg2=ParseFloatArg(script,')',NULL);
				++script.index;
				return arg1==arg2;
			}
		}
	}
	return false;
}
int XPINSBuiltIn::ParseIntExp(XPINSScriptSpace& script){
	int result=0;
	int varIndex=-1;
	while(script.currentChar()!='?')++script.index;
	++script.index;
	if(script.currentChar()!='I'){
	//	cerr<<"Could not parse XPINS expression, returning 0\n";
		return 0;
	}
	else{
		script.index+=2;
		int temp=script.index;
		int expCount=0;
		while ((script.currentChar()!='+'&&script.currentChar()!='-'&&script.currentChar()!='*'&&script.currentChar()!='/'&&script.currentChar()!='%')||expCount!=0)
		{
			if(script.currentChar()=='?')
			{
				if(script.instructions[script.index+2]=='(')++expCount;
				else --expCount;
			}
			++script.index;
		}
		if (script.currentChar()=='+') {//addition
			if(script.matchesString("++")){
				if(script.instructions[script.index+2]=='$')
				{
					script.index+=2;
					bool floatVar=script.instructions[script.index+1]!='I';
					result=ParseIntArg(script,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++script.data->fVars[varIndex];
						else ++script.data->iVars[varIndex];
					}
					return result+1;
				}
				else
				{
					script.index=temp;
					bool floatVar=script.instructions[script.index+1]!='I';
					result=ParseIntArg(script,'+',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++script.data->fVars[varIndex];
						else ++script.data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool assign=script.matchesString("+=");
				script.index=temp;
				bool floatVar=script.instructions[script.index+1]!='I';
				--script.index;
				result=ParseIntArg(script,'+',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result+=ParseIntArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
				{
					if(floatVar)script.data->fVars[varIndex]=result;
					else script.data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (script.currentChar()=='-') {//subtraction
			if(script.matchesString("--")){
				if(script.instructions[script.index+2]=='$')
				{
					script.index+=2;
					bool floatVar=script.instructions[script.index+1]!='I';
					--script.index;
					result=ParseIntArg(script,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--script.data->fVars[varIndex];
						else --script.data->iVars[varIndex];
					}
					return result-1;
				}
				else
				{
					script.index=temp;
					bool floatVar=script.instructions[script.index+1]!='I';
					--script.index;
					result=ParseIntArg(script,'-',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--script.data->fVars[varIndex];
						else --script.data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool assign=script.matchesString("-=");
				script.index=temp;
				bool floatVar=script.instructions[script.index+1]!='I';
				--script.index;
				result=ParseIntArg(script,'-',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result-=ParseIntArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
				{
					if(floatVar)script.data->fVars[varIndex]=result;
					else script.data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (script.currentChar()=='*') {//multiplication
			bool assign=script.matchesString("*=");
			script.index=temp;
			bool floatVar=script.instructions[script.index+1]!='I';
			--script.index;
			result=ParseIntArg(script,'*',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
			result*=ParseIntArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
			{
				if(floatVar)script.data->fVars[varIndex]=result;
				else script.data->iVars[varIndex]=result;
			}
			return result;
		}
		else if (script.currentChar()=='/') {//division
			bool assign=script.matchesString("/=");
			script.index=temp;
			bool floatVar=script.instructions[script.index+1]!='I';
			--script.index;
			result=ParseIntArg(script,'/',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result/=ParseIntArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
			{
				if(floatVar)script.data->fVars[varIndex]=result;
				else script.data->iVars[varIndex]=result;
			}
			return result;
		}
		else if (script.currentChar()=='%') {//modulus
			bool assign=script.matchesString("%=");
			script.index=temp;
			bool floatVar=script.instructions[script.index+1]!='I';
			--script.index;
			result=ParseIntArg(script,'%',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result%=ParseIntArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
			{
				if(floatVar)script.data->fVars[varIndex]=result;
				else script.data->iVars[varIndex]=result;
			}
			return result;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatExp(XPINSScriptSpace& script){
	float result=0;
	int varIndex=-1;
	while(script.currentChar()!='?')++script.index;
	++script.index;
	if(script.currentChar()!='F'){
	//	cerr<<"Could not parse XPINS expression, returning 0\n";
		return ParseIntExp(script);
	}
	else{
		script.index+=2;
		int temp=script.index;
		int expCount=0;
		while ((script.currentChar()!='+'&&script.currentChar()!='-'&&script.currentChar()!='*'&&script.currentChar()!='/'&&script.currentChar()!='%')||expCount!=0)
		{
			if(script.currentChar()=='?')
			{
				if(script.instructions[script.index+2]=='(')++expCount;
				else --expCount;
			}
			++script.index;
		}
		if (script.currentChar()=='+') {//addition
			if(script.matchesString("++")){
				if(script.instructions[script.index+2]=='$')
				{
					script.index+=2;
					bool floatVar=script.instructions[script.index+1]!='I';
					result=ParseIntArg(script,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++script.data->fVars[varIndex];
						else ++script.data->iVars[varIndex];
					}
					return result+1;
				}
				else
				{
					script.index=temp;
					bool floatVar=script.instructions[script.index+1]!='I';
					result=ParseIntArg(script,'+',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)++script.data->fVars[varIndex];
						else ++script.data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool assign=script.matchesString("+=");
				script.index=temp;
				bool floatVar=script.instructions[script.index+1]!='I';
				--script.index;
				result=ParseIntArg(script,'+',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result+=ParseIntArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
				{
					if(floatVar)script.data->fVars[varIndex]=result;
					else script.data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (script.currentChar()=='-') {//subtraction
			if(script.matchesString("--")){
				if(script.instructions[script.index+2]=='$')
				{
					script.index+=2;
					bool floatVar=script.instructions[script.index+1]!='I';
					--script.index;
					result=ParseIntArg(script,')',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--script.data->fVars[varIndex];
						else --script.data->iVars[varIndex];
					}
					return result-1;
				}
				else
				{
					script.index=temp;
					bool floatVar=script.instructions[script.index+1]!='I';
					--script.index;
					result=ParseIntArg(script,'-',&varIndex);
					if(varIndex>-1)
					{
						if(floatVar)--script.data->fVars[varIndex];
						else --script.data->iVars[varIndex];
					}
					return result;
				}
			}
			else
			{
				bool assign=script.matchesString("-=");
				script.index=temp;
				bool floatVar=script.instructions[script.index+1]!='I';
				--script.index;
				result=ParseIntArg(script,'-',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result-=ParseIntArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
				{
					if(floatVar)script.data->fVars[varIndex]=result;
					else script.data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (script.currentChar()=='*') {//multiplication
			script.index=temp;
			if(script.instructions[script.index+1]=='V'||script.instructions[script.index+1]=='<'||script.instructions[script.index+1]=='P'||script.instructions[script.index+1]=='S'){//Dot Produt
				Vector v1=ParseVecArg(script,'*',NULL);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&script.currentChar()!='X') ++script.index;//Get to Key Character
				Vector v2=ParseVecArg(script,')',NULL);
				result=Vector::DotProduct(v1, v2);
				++script.index;
				return result;
			}
			else
			{
				bool assign=script.matchesString("*=");
				script.index=temp;
				bool floatVar=script.instructions[script.index+1]!='I';
				result=ParseIntArg(script,'*',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.	currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result*=ParseIntArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
				{
					if(floatVar)script.data->fVars[varIndex]=result;
					else script.data->iVars[varIndex]=result;
				}
				return result;
			}
		}
		else if (script.currentChar()=='/') {//division
			bool assign=script.matchesString("/=");
			script.index=temp;
			bool floatVar=script.instructions[script.index+1]!='I';
			result=ParseIntArg(script,'/',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result/=ParseIntArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
			{
				if(floatVar)script.data->fVars[varIndex]=result;
				else script.data->iVars[varIndex]=result;
			}
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(XPINSScriptSpace& script){
	int varIndex=-1;
	while(script.currentChar()!='?')++script.index;
	++script.index;
	if(script.currentChar()!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0,0>\n";
	}
	else{
		script.index+=2;
		int temp=script.index;
		while (script.currentChar()!='+'&&script.currentChar()!='-'&&script.currentChar()!='*'&&script.currentChar()!='/') ++script.index;
		if (script.currentChar()=='+') {//addition
			bool assign=script.matchesString("+=");
			script.index=temp;
			Vector result=ParseVecArg(script,'+',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result+=ParseVecArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->vVars[varIndex]=result;
			return result;
		}
		else if (script.currentChar()=='-') {//subtraction
			bool assign=script.matchesString("-=");
			script.index=temp;
			Vector result=ParseVecArg(script,'+',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result-=ParseVecArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->vVars[varIndex]=result;
			return result;
		}
		else if (script.currentChar()=='*') {//multiplication
			if(script.instructions[script.index+2]=='V'||script.instructions[script.index+2]=='<'||script.instructions[script.index+3]=='<'){//Cross Produt
				bool assign=script.matchesString("*=");
				script.index=temp;
				Vector v1=ParseVecArg(script,'*',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				Vector v2=ParseVecArg(script,')',NULL);
				Vector result=Vector::CrossProduct(v1, v2);
				++script.index;
				if(assign&&varIndex>-1)
					script.data->vVars[varIndex]=result;
				return result;
			}
			else if(script.instructions[temp+1]=='M'||script.instructions[temp+2]=='['){//Matrix-Vector
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix m1=ParseMatArg(script,'*',NULL);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				Vector v2=ParseVecArg(script,')',&varIndex);
				Vector result=m1*v2;
				++script.index;
				if(assign&&varIndex>-1)
					script.data->vVars[varIndex]=result;
				return result;
			}
			else{
				bool assign=script.matchesString("*=");
				script.index=temp;
				Vector result=ParseVecArg(script,'*',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result*=ParseFloatArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
					script.data->vVars[varIndex]=result;
				return result;
			}
		}
		else if (script.currentChar()=='/') {//Division
			bool assign=script.matchesString("/=");
			script.index=temp;
			Vector result=ParseVecArg(script,'+',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result/=ParseFloatArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->vVars[varIndex]=result;
			return result;
		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(XPINSScriptSpace& script){
	int varIndex=-1;
	while(script.currentChar()!='?')++script.index;
	if(script.currentChar()!='M'){
		cerr<<"Could not parse XPINS expression, returning [0]\n";
	}
	else{
		script.index+=2;
		int temp=script.index;
		while (script.currentChar()!='+'&&script.currentChar()!='-'&&script.currentChar()!='*'&&script.currentChar()!='/') ++script.index;
		if (script.currentChar()=='+') {//addition
			bool assign=script.matchesString("+=");
			script.index=temp;
			Matrix result=ParseMatArg(script,'+',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result+=ParseMatArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->mVars[varIndex]=result;
			return result;
		}
		else if (script.currentChar()=='-') {//subtraction
			bool assign=script.matchesString("-=");
			script.index=temp;
			Matrix result=ParseMatArg(script,'+',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result-=ParseMatArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->mVars[varIndex]=result;
			return result;
		}
		else if (script.currentChar()=='*') {//multiplication
			if(script.instructions[script.index+2]=='M'||script.instructions[script.index+2]=='['){//Matrix-Matrix
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix result=ParseMatArg(script,'+',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result*=ParseMatArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
					script.data->mVars[varIndex]=result;
				return result;
			}
			else{
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix result=ParseMatArg(script,'*',&varIndex);
				while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
				result*=ParseFloatArg(script,')',NULL);
				++script.index;
				if(assign&&varIndex>-1)
					script.data->mVars[varIndex]=result;
				return result;
			}
		}
		else if (script.currentChar()=='/') {//division
			bool assign=script.matchesString("/=");
			script.index=temp;
			Matrix result=ParseMatArg(script,'*',&varIndex);
			while (script.currentChar()!='$'&&script.currentChar()!='^'&&script.currentChar()!='?'&&script.currentChar()!='#'&&	script.currentChar()!='X') ++script.index;//Get to Key Character
			result/=ParseFloatArg(script,')',NULL);
			++script.index;
			if(assign&&varIndex>-1)
				script.data->mVars[varIndex]=result;
			return result;
		}
	}
	return Matrix();
}

#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 0://X_AND
		{
			bool arg1=ParseBoolArg(script, ',',NULL);
			bool arg2=ParseBoolArg(script, ')',NULL);
			return arg1&&arg2;
		}
		case 1://X_OR
		{
			bool arg1=ParseBoolArg(script, ',',NULL);
			bool arg2=ParseBoolArg(script, ')',NULL);
			return arg1||arg2;
		}
		case 2://X_NOT
		{
			bool arg1=ParseBoolArg(script, ')',NULL);
			return !arg1;
		}
		case 3://X_LESS
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1<arg2;
		}
		case 4://X_MORE
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1>arg2;
		}
		case 5://X_EQUAL
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1==arg2;
		}
		
	}
	return false;
}
int XPINSBuiltIn::ParseIntBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 0://X_MOD
		{
			int arg1=ParseIntArg(script, ',',NULL);
			int arg2=ParseIntArg(script, ')',NULL);
			return arg1%arg2;
		}
		case 1://X_RAND
		{
			int arg1=ParseIntArg(script, ',',NULL);
			int arg2=ParseIntArg(script, ')',NULL);
			return arc4random()%(arg2-arg1)+arg1;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 0://X_ADD
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1+arg2;
		}
		case 1://X_SUB
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1-arg2;
		}
		case 2://X_MULT
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1*arg2;
		}
		case 3://X_DIV
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return arg1/arg2;
		}
		case 4://X_TSIN
		{
			float arg1=ParseFloatArg(script, ')',NULL);
			return sinf(arg1);
		}
		case 5://X_TCOS
		{
			float arg1=ParseFloatArg(script, ')',NULL);
			return cosf(arg1);
		}
		case 6://X_TTAN
		{
			float arg1=ParseFloatArg(script, ')',NULL);
			return tanf(arg1);
		}
		case 7://X_TATAN
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return atan2f(arg1, arg2);
		}
		case 8://X_POW
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return powf(arg1, arg2);
		}
		case 9://X_VADDPOLAR
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return addPolar(arg1, arg2);
		}
			//TBImplemented
		case 10://X_VDIST
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			float arg3=ParseFloatArg(script, ')',NULL);
			return dist(arg1, arg2,arg3);
		}
		case 11://X_VX
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return x;
		}
		case 12://X_VY
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return y;
		}
		case 13://X_VZ
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return z;
		}
		case 14://X_VR
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VTHETA
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VRHO
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 17://X_VPHI
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			float x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 18://X_VDOT
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Vector::DotProduct(arg1, arg2);
		}
		case 19://X_VANG
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Vector::AngleBetweenVectors(arg1, arg2);

		}
		case 20://X_MGET
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			int arg2=ParseIntArg(script, ',',NULL);
			int arg3=ParseIntArg(script, ')',NULL);
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 21://X_MDET
		{
			Matrix arg1=ParseMatArg(script, ')',NULL);
			return Matrix::Determinant(arg1);
		}
			
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 0://X_VREC
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			float arg3=ParseFloatArg(script, ')',NULL);
			return Vector(arg1,arg2,arg3);
		}
		case 1://X_VPOL
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			float arg3=ParseFloatArg(script, ')',NULL);
			return Vector::PolarVector(arg1,arg2,arg3);
		}
		case 2://X_VSHPERE
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			float arg3=ParseFloatArg(script, ')',NULL);
			return Vector::SphericalVector(arg1,arg2,arg3);
		}
		case 3://X_VADD
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Vector::Add(arg1,arg2);
		}
		case 4://X_VSUB
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			Vector temp=Vector::Scale(arg2, -1);
			return Vector::Add(arg1,temp);
		}
		case 5://X_VSCALE
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return Vector::Scale(arg1,arg2);
		}
		case 6://X_VPROJ
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			float arg3=ParseFloatArg(script, ')',NULL);
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 7://X_VCROSS
		{
			Vector arg1=ParseVecArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Vector::CrossProduct(arg1,arg2);
		}
		case 8://X_MMTV
		{
			Matrix arg1=ParseMatArg(script, ')',NULL);
			return Matrix::VectorForMatrix(arg1);

		}
		case 9://X_MVMULT
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Matrix::MultiplyMatrixVector(arg1, arg2);
		}
	}
	return Vector();

}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 0://X_MMAKE
		{
			int arg1=ParseIntArg(script, ',',NULL);
			int arg2=ParseIntArg(script, ')',NULL);
			return Matrix(arg1,arg2);
		}
		case 1://X_MID
		{
			int arg1=ParseIntArg(script, ')',NULL);
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 2://X_MROT
		{
			float arg1=ParseFloatArg(script, ',',NULL);
			Vector arg2=ParseVecArg(script, ')',NULL);
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 3://X_MADD
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			Matrix arg2=ParseMatArg(script, ')',NULL);
			return Matrix::Add(arg1,arg2);
		}
		case 4://X_MSUB
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			Matrix arg2=ParseMatArg(script, ')',NULL);
			Matrix temp=Matrix::Scale(arg2, -1);
			return Matrix::Add(arg1,temp);
		}
		case 5://X_MSCALE
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ')',NULL);
			return Matrix::Scale(arg1,arg2);
		}
		case 6://X_MMULT
		{
			Matrix arg1=ParseMatArg(script, ',',NULL);
			Matrix arg2=ParseMatArg(script, ')',NULL);
			return Matrix::Multiply(arg1,arg2);

		}
		case 7://X_MINV
		{
			Matrix arg1=ParseMatArg(script, ')',NULL);
			return Matrix::Invert(arg1);
		}
		case 8://X_MTRANS
		{
			Matrix arg1=ParseMatArg(script, ')',NULL);
			return Matrix::Transpose(arg1);
		}
		case 9://X_MVTM
		{
			Vector arg1=ParseVecArg(script, ')',NULL);
			return Matrix::MatrixForVector(arg1);
		}
	}
	return Matrix();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 0:{//X_PRINT
			string str=XPINSParser::ParseStrArg(script, ')');
			cout<<str;
		}break;
		case 1:{//X_MSET
			Matrix arg1=ParseMatArg(script, ',',NULL);
			float arg2=ParseFloatArg(script, ',',NULL);
			int arg3=ParseIntArg(script, ',',NULL);
			int arg4=ParseIntArg(script, ')',NULL);
			arg1.SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}
