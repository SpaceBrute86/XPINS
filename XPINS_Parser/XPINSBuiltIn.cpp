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
//Expression Parsing
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
				result=ParseFloatArg(scriptText, data, localBindings, i,'+');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result+=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'-');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result-=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'*');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result*=ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'/');
			else
				result=ParseIntArg(scriptText, data, localBindings, i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result/=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='%') {//modulus
			result=ParseIntArg(scriptText, data, localBindings, i, '%');
			++i;
			result%=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
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
		return 0.0;
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText,data,localBindings, i,'+');
			else
				result=(float)ParseIntArg(scriptText,data,localBindings, i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				result+=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'-');
			else
				result=(float)ParseIntArg(scriptText, data, localBindings, i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result-=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				result=Vector::DotProduct(v1, v2);
				++i;
				return result;
			}
			else if(scriptText[j+2]=='V'){
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseFloatArg(scriptText,data,localBindings, i,'*');
				else
					result=(float)ParseIntArg(scriptText, data, localBindings, i, '*');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					result*=(float)ParseIntArg(scriptText,data,localBindings, i, ')');
				++i;
				return result;
			}
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, localBindings, i,'/');
			else
				result=(float)ParseIntArg(scriptText, data, localBindings, i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, localBindings, i,')');
			else
				result/=(float)ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
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
			Vector v1=ParseVecArg(scriptText, data, localBindings, i,'+');
			++i;
			Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
			++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Vector v1=ParseVecArg(scriptText, data, localBindings, i,'-');
			++i;
			Vector temp=ParseVecArg(scriptText, data, localBindings, i,')');
			Vector v2=Vector::Scale(temp, -1);
			++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				++i;
				return Matrix::MultiplyMatrixVector(m1, v2);
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'){//Cross Produt
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, localBindings, i,')');
				++i;
				return Vector::CrossProduct(v1, v2);
			}
			else{
				Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					k=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return Vector::Scale(v1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Vector v1=ParseVecArg(scriptText, data, localBindings, i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				k=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			return Vector::Scale(v1, 1/k);
		}
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
			Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'+');
			++i;
			Matrix m2=ParseMatArg(scriptText, data, localBindings, i,')');
			++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'+');
			++i;
			Matrix temp=ParseMatArg(scriptText, data, localBindings, i,')');
			++i;
			Matrix m2=Matrix::Scale(temp, -1);
			++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'+');
				++i;
				Matrix m2=ParseMatArg(scriptText, data, localBindings, i,')');
				++i;
				return Matrix::Add(m1, m2);
				
			}
			else{
				Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, localBindings, i,')');
				else
					k=ParseIntArg(scriptText, data, localBindings, i, ')');
				++i;
				return Matrix::Scale(m1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Matrix m1=ParseMatArg(scriptText, data, localBindings, i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,data,localBindings, i,')');
			else
				k=ParseIntArg(scriptText, data, localBindings, i, ')');
			++i;
			return Matrix::Scale(m1, 1/k);
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
		case 0://X_MSET
		{
			Matrix arg1=ParseMatArg(scriptText, data, localBindings, i, ',');
			float arg2=ParseFloatArg(scriptText, data, localBindings, i, ',');
			int arg3=ParseIntArg(scriptText, data, localBindings, i, ',');
			int arg4=ParseIntArg(scriptText, data, localBindings, i, ')');
			arg1.SetValueAtPosition(arg2, arg3, arg4);
		}
	}
}
