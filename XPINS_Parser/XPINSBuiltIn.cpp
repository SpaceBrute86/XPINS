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
bool XPINSBuiltIn::ParseBoolExp(string scriptText,XPINSDataRef data, int& i){
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='B'){
		//cerr<<"Could not parse XPINS expression, returning false\n";
		return false;
	}
	else{
		i+=2;
		if (scriptText[i]=='!'&&scriptText[i+1]=='=') {//not
			++i;
			bool arg=ParseBoolArg(scriptText, data, i, ')');
			++i;
			return !arg;
		}
		else {//two inputs
			int j=i;
			while (scriptText[j]!='|'&&scriptText[j]!='&'&&scriptText[j]!='<'&&scriptText[j]!='='&&scriptText[j]!='>'&&scriptText[j]!='!') ++j;
			if(scriptText[j]=='|'&&scriptText[j+1]=='|'){//OR
				bool arg1=ParseBoolArg(scriptText, data, i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText,data, i,')');
				++i;
				return arg1||arg2;
			}
			else if(scriptText[j]=='&'&&scriptText[j+1]=='&'){//And
				bool arg1=ParseBoolArg(scriptText, data, i,'|');
				i+=2;
				bool arg2=ParseBoolArg(scriptText, data, i,')');
				++i;
				return arg1&&arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]!='='){//Less Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, i,'<');
				else
					arg1=ParseIntArg(scriptText, data, i, '<');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, i,')');
				else
					arg2=ParseIntArg(scriptText, data, i, ')');
				++i;
				return arg1<arg2;
			}
			else if(scriptText[j]=='<'&&scriptText[j+1]=='='){//Less Than or Equal
				float arg1=arg1=ParseFloatArg(scriptText, data, i,'<');
				i+=2;
				float arg2=ParseFloatArg(scriptText, data, i,')');
				++i;
				return arg1<=arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]!='='){//Greater Than
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, i,'>');
				else
					arg1=ParseIntArg(scriptText, data, i, '>');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, i,')');
				else
					arg2=ParseIntArg(scriptText, data, i, ')');
				++i;
				return arg1>arg2;
			}
			else if(scriptText[j]=='>'&&scriptText[j+1]=='='){//Greater than Or equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, i,'>');
				else
					arg1=ParseIntArg(scriptText, data, i, '>');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, i,')');
				else
					arg2=ParseIntArg(scriptText, data, i, ')');
				++i;
				return arg1>=arg2;
			}
			else if(scriptText[j]=='!'&&scriptText[j+1]=='='){//Not Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, i,'!');
				else
					arg1=ParseIntArg(scriptText, data, i, '!');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, i,')');
				else
					arg2=ParseIntArg(scriptText, data, i, ')');
				++i;
				return arg1!=arg2;
			}
			else if(scriptText[j]=='='&&scriptText[j+1]=='='){//Equal
				float arg1=0,arg2=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					arg1=ParseFloatArg(scriptText, data, i,'=');
				else
					arg1=ParseIntArg(scriptText, data, i, '=');
				i+=2;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 2 is float
					arg2=ParseFloatArg(scriptText, data, i,')');
				else
					arg2=ParseIntArg(scriptText, data, i, ')');
				++i;
				return arg1==arg2;
			}
		}
	}
	return false;
}
int XPINSBuiltIn::ParseIntExp(string scriptText,XPINSDataRef data, int& i){
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
				result=ParseFloatArg(scriptText, data, i,'+');
			else
				result=ParseIntArg(scriptText, data, i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText, data, i,')');
			else
				result+=ParseIntArg(scriptText, data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, i,'-');
			else
				result=ParseIntArg(scriptText, data, i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, data, i,')');
			else
				result-=ParseIntArg(scriptText, data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, i,'*');
			else
				result=ParseIntArg(scriptText, data, i, '*');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result*=ParseFloatArg(scriptText, data, i,')');
			else
				result*=ParseIntArg(scriptText,data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, i,'/');
			else
				result=ParseIntArg(scriptText, data, i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, i,')');
			else
				result/=ParseIntArg(scriptText, data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='%') {//modulus
			result=ParseIntArg(scriptText, data, i, '%');
			++i;
			result%=ParseIntArg(scriptText, data, i, ')');
			++i;
			return result;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatExp(string scriptText,XPINSDataRef data, int& i){
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
				result=ParseFloatArg(scriptText,data, i,'+');
			else
				result=(float)ParseIntArg(scriptText,data, i, '+');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result+=ParseFloatArg(scriptText,data, i,')');
			else
				result+=(float)ParseIntArg(scriptText,data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='-') {//subtraction
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, i,'-');
			else
				result=(float)ParseIntArg(scriptText, data, i, '-');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result-=ParseFloatArg(scriptText, data, i,')');
			else
				result-=(float)ParseIntArg(scriptText,data, i, ')');
			++i;
			return result;
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='V'){//Dot Produt
				Vector v1=ParseVecArg(scriptText, data, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, i,')');
				result=Vector::DotProduct(v1, v2);
				++i;
				return result;
			}
			else if(scriptText[j+2]=='V'){
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result=ParseFloatArg(scriptText,data, i,'*');
				else
					result=(float)ParseIntArg(scriptText, data, i, '*');
				++i;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					result*=ParseFloatArg(scriptText, data, i,')');
				else
					result*=(float)ParseIntArg(scriptText,data, i, ')');
				++i;
				return result;
			}
		}
		else if (scriptText[j]=='/') {//division
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result=ParseFloatArg(scriptText, data, i,'/');
			else
				result=(float)ParseIntArg(scriptText, data, i, '/');
			++i;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				result/=ParseFloatArg(scriptText, data, i,')');
			else
				result/=(float)ParseIntArg(scriptText, data, i, ')');
			++i;
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(string scriptText,XPINSDataRef data, int& i){
	
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Vector v1=ParseVecArg(scriptText, data, i,'+');
			++i;
			Vector v2=ParseVecArg(scriptText, data, i,')');
			++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Vector v1=ParseVecArg(scriptText, data, i,'-');
			++i;
			Vector temp=ParseVecArg(scriptText, data, i,')');
			Vector v2=Vector::Scale(temp, -1);
			++i;
			return Vector::Add(v1, v2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[i+1]=='M'||scriptText[i+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, data, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, i,')');
				++i;
				return Matrix::MultiplyMatrixVector(m1, v2);
			}
			else if(scriptText[j+2]=='V'||scriptText[j+2]=='<'){//Cross Produt
				Vector v1=ParseVecArg(scriptText, data, i,'+');
				++i;
				Vector v2=ParseVecArg(scriptText, data, i,')');
				++i;
				return Vector::CrossProduct(v1, v2);
			}
			else{
				Vector v1=ParseVecArg(scriptText, data, i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, i,')');
				else
					k=ParseIntArg(scriptText, data, i, ')');
				++i;
				return Vector::Scale(v1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Vector v1=ParseVecArg(scriptText, data, i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,data, i,')');
			else
				k=ParseIntArg(scriptText, data, i, ')');
			++i;
			return Vector::Scale(v1, 1/k);
		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(string scriptText,XPINSDataRef data, int& i){
	
	while(scriptText[i++]!='?'){}
	if(scriptText[i]!='V'){
		cerr<<"Could not parse XPINS expression, returning <0,0>\n";
	}
	else{
		i+=2;
		int j=i;
		while (scriptText[j]!='+'&&scriptText[j]!='-'&&scriptText[j]!='*'&&scriptText[j]!='/') ++j;
		if (scriptText[j]=='+') {//addition
			Matrix m1=ParseMatArg(scriptText, data, i,'+');
			++i;
			Matrix m2=ParseMatArg(scriptText, data, i,')');
			++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='-') {//subtraction
			Matrix m1=ParseMatArg(scriptText, data, i,'+');
			++i;
			Matrix temp=ParseMatArg(scriptText, data, i,')');
			++i;
			Matrix m2=Matrix::Scale(temp, -1);
			++i;
			return Matrix::Add(m1, m2);
		}
		else if (scriptText[j]=='*') {//multiplication
			if(scriptText[j+2]=='M'||scriptText[j+2]=='['){//Cross Produt
				Matrix m1=ParseMatArg(scriptText, data, i,'+');
				++i;
				Matrix m2=ParseMatArg(scriptText, data, i,')');
				++i;
				return Matrix::Add(m1, m2);
				
			}
			else{
				Matrix m1=ParseMatArg(scriptText, data, i,'*');
				++i;
				float k=0;
				if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
					k=ParseFloatArg(scriptText, data, i,')');
				else
					k=ParseIntArg(scriptText, data, i, ')');
				++i;
				return Matrix::Scale(m1, k);
			}
		}
		else if (scriptText[j]=='/') {//division
			Matrix m1=ParseMatArg(scriptText, data, i,'*');
			++i;
			float k=0;
			if(scriptText[i]=='^'||scriptText[i+1]=='F')//Input 1 is float
				k=ParseFloatArg(scriptText,data, i,')');
			else
				k=ParseIntArg(scriptText, data, i, ')');
			++i;
			return Matrix::Scale(m1, 1/k);
		}
	}
	return Matrix();
}

#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, string scriptText,XPINSDataRef data,int& i)
{
	switch (fNum) {
		case 0://X_AND
		{
			bool param1=ParseBoolArg(scriptText, data, i, ',');
			bool param2=ParseBoolArg(scriptText, data, i, ')');
			return param1&&param2;
		}
		case 1://X_OR
		{
			bool param1=ParseBoolArg(scriptText, data, i, ',');
			bool param2=ParseBoolArg(scriptText, data, i, ')');
			return param1||param2;
		}
		case 2://X_NOT
		{
			bool param1=ParseBoolArg(scriptText, data, i, ')');
			return !param1;
		}
		case 3://X_LESS
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1<param2;
		}
		case 4://X_MORE
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1>param2;
		}
		case 5://X_EQUAL
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1==param2;
		}
		
	}
	return false;
}
int XPINSBuiltIn::ParseIntBIF(int fNum,string scriptText,XPINSDataRef data,int& i)
{
	
	switch (fNum) {
		case 0://X_MOD
		{
			int param1=ParseIntArg(scriptText, data, i, ',');
			int param2=ParseIntArg(scriptText, data, i, ')');
			return param1%param2;
		}
		case 1://X_RAND
		{
			int param1=ParseIntArg(scriptText, data, i, ',');
			int param2=ParseIntArg(scriptText, data, i, ')');
			return arc4random()%(param2-param1)+param1;
		}
	}
	return 0;
}
float XPINSBuiltIn::ParseFloatBIF(int fNum,string scriptText,XPINSDataRef data,int& i)
{
	switch (fNum) {
		case 0://X_ADD
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1+param2;
		}
		case 1://X_SUB
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1-param2;
		}
		case 2://X_MULT
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1*param2;
		}
		case 3://X_DIV
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return param1/param2;
		}
		case 4://X_TSIN
		{
			float param1=ParseFloatArg(scriptText, data, i, ')');
			return sinf(param1);
		}
		case 5://X_TCOS
		{
			float param1=ParseFloatArg(scriptText, data, i, ')');
			return cosf(param1);
		}
		case 6://X_TTAN
		{
			float param1=ParseFloatArg(scriptText, data, i, ')');
			return tanf(param1);
		}
		case 7://X_TATAN
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return atan2f(param1, param2);
		}
		case 8://X_POW
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return powf(param1, param2);
		}
		case 9://X_VADDPOLAR
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return addPolar(param1, param2);
		}
			//TBImplemented
		case 10://X_VDIST
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			float param3=ParseFloatArg(scriptText, data, i, ')');
			return dist(param1, param2,param3);
		}
		case 11://X_VX
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.RectCoords(&x, &y, &z);
			return x;
		}
		case 12://X_VY
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.RectCoords(&x, &y, &z);
			return y;
		}
		case 13://X_VZ
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.RectCoords(&x, &y, &z);
			return z;
		}
		case 14://X_VR
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VTHETA
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VRHO
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 17://X_VPHI
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			float x=0,y=0,z=0;
			param1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 18://X_VDOT
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Vector::DotProduct(param1, param2);
		}
		case 19://X_VANG
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Vector::AngleBetweenVectors(param1, param2);

		}
		case 20://X_MGET
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			int param2=ParseIntArg(scriptText, data, i, ',');
			int param3=ParseIntArg(scriptText, data, i, ')');
			return param1.ValueAtPosition(param2, param3);
		}
		case 21://X_MDET
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ')');
			return Matrix::Determinant(param1);
		}
			
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum,string scriptText,XPINSDataRef data,int& i)
{
	
	switch (fNum) {
		case 0://X_VREC
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			float param3=ParseFloatArg(scriptText, data, i, ')');
			return Vector(param1,param2,param3);
		}
		case 1://X_VPOL
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			float param3=ParseFloatArg(scriptText, data, i, ')');
			return Vector::PolarVector(param1,param2,param3);
		}
		case 2://X_VSHPERE
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			float param3=ParseFloatArg(scriptText, data, i, ')');
			return Vector::SphericalVector(param1,param2,param3);
		}
		case 3://X_VADD
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Vector::Add(param1,param2);
		}
		case 4://X_VSUB
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			Vector temp=Vector::Scale(param2, -1);
			return Vector::Add(param1,temp);
		}
		case 5://X_VSCALE
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return Vector::Scale(param1,param2);
		}
		case 6://X_VPROJ
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			float param3=ParseFloatArg(scriptText, data, i, ')');
			return Vector::ProjectionInDirection(param1,param2,param3);
		}
		case 7://X_VCROSS
		{
			Vector param1=ParseVecArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Vector::CrossProduct(param1,param2);
		}
		case 8://X_MMTV
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ')');
			return Matrix::VectorForMatrix(param1);

		}
		case 9://X_MVMULT
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Matrix::MultiplyMatrixVector(param1, param2);
		}
	}
	return Vector();

}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, string scriptText,XPINSDataRef data,int& i)
{
	
	switch (fNum) {
		case 0://X_MMAKE
		{
			int param1=ParseIntArg(scriptText, data, i, ',');
			int param2=ParseIntArg(scriptText, data, i, ')');
			return Matrix(param1,param2);
		}
		case 1://X_MID
		{
			int param1=ParseIntArg(scriptText, data, i, ')');
			return Matrix::IdentityMatrixOfSize(param1);
		}
		case 2://X_MROT
		{
			float param1=ParseFloatArg(scriptText, data, i, ',');
			Vector param2=ParseVecArg(scriptText, data, i, ')');
			return Matrix::RotationMatrixWithAngleAroundVector(param2,param1);
		}
		case 3://X_MADD
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			Matrix param2=ParseMatArg(scriptText, data, i, ')');
			return Matrix::Add(param1,param2);
		}
		case 4://X_MSUB
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			Matrix param2=ParseMatArg(scriptText, data, i, ')');
			Matrix temp=Matrix::Scale(param2, -1);
			return Matrix::Add(param1,temp);
		}
		case 5://X_MSCALE
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ')');
			return Matrix::Scale(param1,param2);
		}
		case 6://X_MMULT
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			Matrix param2=ParseMatArg(scriptText, data, i, ')');
			return Matrix::Multiply(param1,param2);

		}
		case 7://X_MINV
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ')');
			return Matrix::Invert(param1);
		}
		case 8://X_MTRANS
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ')');
			return Matrix::Transpose(param1);
		}
		case 9://X_MVTM
		{
			Vector param1=ParseVecArg(scriptText, data, i, ')');
			return Matrix::MatrixForVector(param1);
		}
	}
	return Matrix();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum,string scriptText,XPINSDataRef data,int& i)
{
	switch (fNum) {
		case 0://X_MSET
		{
			Matrix param1=ParseMatArg(scriptText, data, i, ',');
			float param2=ParseFloatArg(scriptText, data, i, ',');
			int param3=ParseIntArg(scriptText, data, i, ',');
			int param4=ParseIntArg(scriptText, data, i, ')');
			param1.SetValueAtPosition(param2, param3, param4);
		}
	}
}








