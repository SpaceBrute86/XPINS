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
		case 'N':
			ParseNumExp(script);
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
			bool arg=*ParseBoolArg(script, ')');
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
				bool arg1=*ParseBoolArg(script,'|');
				script.index+=2;
				bool arg2=*ParseBoolArg(script,')');
				return arg1||arg2;
			}
			else if(script.matchesString("&&")){//And
				script.index=temp;
				bool arg1=*ParseBoolArg(script,'|');
				script.index+=2;
				bool arg2=*ParseBoolArg(script,')');
				return arg1&&arg2;
			}
			else if(script.currentChar()=='<'&&!script.matchesString("<=")){//Less Than
				script.index=temp;
				double arg1=*ParseNumArg(script,'<');
				
				double arg2=*ParseNumArg(script,')');
				return arg1<arg2;
			}
			else if(script.matchesString("<=")){//Less Than or Equal
				script.index=temp;
				double arg1=arg1=*ParseNumArg(script,'<');
				script.index+=2;
				double arg2=*ParseNumArg(script,')');
				return arg1<=arg2;
			}
			else if(script.currentChar()=='>'&&!script.matchesString(">=")){//Greater Than
				script.index=temp;
				double arg1=*ParseNumArg(script,'>');
				
				double arg2=*ParseNumArg(script,')');
				return arg1>arg2;
			}
			else if(script.matchesString(">=")){//Greater than Or equal
				script.index=temp;
				double arg1=*ParseNumArg(script,'>');
				script.index+=2;
				double arg2=*ParseNumArg(script,')');
				return arg1>=arg2;
			}
			else if(script.matchesString("!=")){//Not Equal
				script.index=temp;
				double arg1=*ParseNumArg(script,'!');
				script.index+=2;
				double arg2=*ParseNumArg(script,')');
				return arg1!=arg2;
			}
			else if(script.matchesString("==")){//Equal
				script.index=temp;
				double arg1=*ParseNumArg(script,'=');
				script.index+=2;
				double arg2=*ParseNumArg(script,')');
				return arg1==arg2;
			}
		}
	}
	return false;
}

double XPINSBuiltIn::ParseNumExp(XPINSScriptSpace& script){
	double result=0;
	while(script.currentChar()!='?')++script.index;
	++script.index;
	if(script.currentChar()!='N'){
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
					double* var=ParseNumArg(script,')');
					return ++(*var);
				}
				else
				{
					script.index=temp;
					double* var=ParseNumArg(script,'+');
					return (*var)++;
				}
			}
			else
			{
				bool assign=script.matchesString("+=");
				script.index=temp;
				double* var=ParseNumArg(script,'+');
				result=*var;
				result+=*ParseNumArg(script,')');
				if(assign)
					*var=result;
				return result;
			}
		}
		else if (script.currentChar()=='-') {//subtraction
			if(script.matchesString("--")){
				if(script.instructions[script.index+2]=='$')
				{
					script.index+=2;
					double* var=ParseNumArg(script,')');
					return --(*var);
				}
				else
				{
					script.index=temp;
					double* var=ParseNumArg(script,'-');
					return (*var)--;
				}
			}
			else
			{
				bool assign=script.matchesString("-=");
				script.index=temp;
				double* var=ParseNumArg(script,'-');
				result=*var;
				result-=*ParseNumArg(script,')');
				if(assign)
					*var=result;
				return result;
			}
		}
		else if (script.currentChar()=='*') {//multiplication
			script.index=temp;
			if(script.instructions[script.index+1]=='V'||script.instructions[script.index+2]=='<'||script.instructions[script.index+3]=='<'){//Dot Produt
				Vector v1=*ParseVecArg(script,'*');
				Vector v2=*ParseVecArg(script,')');
				result=Vector::DotProduct(v1, v2);
				return result;
			}
			else
			{
				bool assign=script.matchesString("*=");
				script.index=temp;
				double* var=ParseNumArg(script,'*');
				result=*var;
				result*=*ParseNumArg(script,')');
				if(assign)
					*var=result;
				return result;
			}
		}
		else if (script.currentChar()=='/') {//division
			bool assign=script.matchesString("/=");
			script.index=temp;
			double* var=ParseNumArg(script,'/');
			result=*var;
			result/=*ParseNumArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='^') {//Power
			bool assign=script.matchesString("^=");
			script.index=temp;
			double* var=ParseNumArg(script,'-');
			result=*var;
			result=powf(result,*ParseNumArg(script,')'));
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='%') {//modulus
			bool assign=script.matchesString("%=");
			script.index=temp;
			double* var=ParseNumArg(script,'/');
			int i1=*var;
			int i2=*ParseNumArg(script,')');
			result=i1%i2;
			
			if(assign)
				*var=result;
			return result;
		}
	}
	return 0.0;
}
Vector XPINSBuiltIn::ParseVecExp(XPINSScriptSpace& script){
	Vector result;
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
			Vector* var=ParseVecArg(script,'+');
			result=*var;
			result+=*ParseVecArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='-') {//subtraction
			bool assign=script.matchesString("-=");
			script.index=temp;
			Vector* var=ParseVecArg(script,'-');
			result=*var;
			result-=*ParseVecArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='*') {//multiplication
			if(script.instructions[script.index+2]=='V'||script.instructions[script.index+2]=='<'||script.instructions[script.index+3]=='<'){//Cross Produt
				bool assign=script.matchesString("*=");
				script.index=temp;
				Vector* var=ParseVecArg(script,'*');
				result=*var;
				result=Vector::CrossProduct(result,*ParseVecArg(script,')'));
				
				if(assign)
					*var=result;
				return result;
			}
			else if(script.instructions[temp+1]=='M'||script.instructions[temp+2]=='['){//Matrix-Vector
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix m=*ParseMatArg(script,'*');
				Vector* var=ParseVecArg(script,')');
				result=m*(*var);
				
				if(assign)
					*var=result;
				return result;
			}
			else{//Vector-Scalar
				bool assign=script.matchesString("*=");
				script.index=temp;
				Vector* var=ParseVecArg(script,'*');
				result=*var;
				result*=*ParseNumArg(script,')');
				
				if(assign)
					*var=result;
				return result;
			}
		}
		else if (script.currentChar()=='/') {//Division
			bool assign=script.matchesString("/=");
			script.index=temp;
			Vector* var=ParseVecArg(script,'/');
			result=*var;
			result/=*ParseNumArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
	}
	return Vector();
}
Matrix XPINSBuiltIn::ParseMatExp(XPINSScriptSpace& script){
	Matrix result;
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
			Matrix* var=ParseMatArg(script,'+');
			result=*var;
			result+=*ParseMatArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='-') {//subtraction
			bool assign=script.matchesString("-=");
			script.index=temp;
			Matrix* var=ParseMatArg(script,'-');
			result=*var;
			result-=*ParseMatArg(script,')');
			
			if(assign)
				*var=result;
			return result;
		}
		else if (script.currentChar()=='*') {//multiplication
			if(script.instructions[script.index+2]=='M'||script.instructions[script.index+2]=='['){//Matrix-Matrix
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix* var=ParseMatArg(script,'*');
				result=*var;
				result*=*ParseMatArg(script,')');
				
				if(assign)
					*var=result;
				return result;
			}
			else{
				bool assign=script.matchesString("*=");
				script.index=temp;
				Matrix* var=ParseMatArg(script,'*');
				result=*var;
				result*=*ParseNumArg(script,')');
				
				if(assign)
					*var=result;
				return result;
			}
		}
		else if (script.currentChar()=='/') {//division
			bool assign=script.matchesString("/=");
			script.index=temp;
			Matrix* var=ParseMatArg(script,'/');
			result=*var;
			result/=*ParseNumArg(script,')');
			
			if(assign)
				*var=result;
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
			bool arg1=*ParseBoolArg(script, ',');
			bool arg2=*ParseBoolArg(script, ')');
			return arg1&&arg2;
		}
		case 1://X_OR
		{
			bool arg1=*ParseBoolArg(script, ',');
			bool arg2=*ParseBoolArg(script, ')');
			return arg1||arg2;
		}
		case 2://X_NOT
		{
			bool arg1=*ParseBoolArg(script, ')');
			return !arg1;
		}
		case 3://X_LESS
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1<arg2;
		}
		case 4://X_MORE
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1>arg2;
		}
		case 5://X_EQUAL
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1==arg2;
		}
		
	}
	return false;
}

double XPINSBuiltIn::ParseNumBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 0://X_ADD
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1+arg2;
		}
		case 1://X_SUB
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1-arg2;
		}
		case 2://X_MULT
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1*arg2;
		}
		case 3://X_DIV
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return arg1/arg2;
		}
		case 4://X_TSIN
		{
			double arg1=*ParseNumArg(script, ')');
			return sinf(arg1);
		}
		case 5://X_TCOS
		{
			double arg1=*ParseNumArg(script, ')');
			return cosf(arg1);
		}
		case 6://X_TTAN
		{
			double arg1=*ParseNumArg(script, ')');
			return tanf(arg1);
		}
		case 7://X_TATAN
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return atan2f(arg1, arg2);
		}
		case 8://X_POW
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return powf(arg1, arg2);
		}
		case 9://X_VADDPOLAR
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return addPolar(arg1, arg2);
		}
			//TBImplemented
		case 10://X_VDIST
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return dist(arg1, arg2,arg3);
		}
		case 11://X_VX
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return x;
		}
		case 12://X_VY
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return y;
		}
		case 13://X_VZ
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return z;
		}
		case 14://X_VR
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VTHETA
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VRHO
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 17://X_VPHI
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 18://X_VDOT
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::DotProduct(arg1, arg2);
		}
		case 19://X_VANG
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::AngleBetweenVectors(arg1, arg2);

		}
		case 20://X_MGET
		{
			Matrix arg1=*ParseMatArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ')');
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 21://X_MDET
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Determinant(arg1);
		}
		case 22://X_MOD
		{
			int arg1=*ParseNumArg(script, ',');
			int arg2=*ParseNumArg(script, ')');
			return arg1%arg2;
		}
		case 23://X_LN
		{
			double arg1=*ParseNumArg(script, ',');
			return log(arg1);
		}
		case 24://X_LOG
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			return log(arg1)/log(arg2);
		}
		case 25://X_ABS
		{
			double arg1=*ParseNumArg(script, ',');
			return fabs(arg1);
		}
		case 26://X_FLOOR
		{
			double arg1=*ParseNumArg(script, ',');
			return floor(arg1);
		}
		case 27://X_PRAND
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::UniformRV(arg1, arg2-arg1);
		}
		case 28://X_PBERN
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::BernoulliRV(arg1);
		}
		case 29://X_PNormal
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::UniformRV(arg1, arg2);
		}
		case 30://X_PEXP
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::ExponentialRV(arg1);
		}
		case 31://X_PPOISSON
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::PoissonRV(arg1);
		}
			
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 0://X_VREC
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector(arg1,arg2,arg3);
		}
		case 1://X_VPOL
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::PolarVector(arg1,arg2,arg3);
		}
		case 2://X_VSHPERE
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::SphericalVector(arg1,arg2,arg3);
		}
		case 3://X_VADD
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::Add(arg1,arg2);
		}
		case 4://X_VSUB
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			Vector temp=Vector::Scale(arg2, -1);
			return Vector::Add(arg1,temp);
		}
		case 5://X_VSCALE
		{
			Vector arg1=*ParseVecArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Vector::Scale(arg1,arg2);
		}
		case 6://X_VPROJ
		{
			Vector arg1=*ParseVecArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 7://X_VCROSS
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::CrossProduct(arg1,arg2);
		}
		case 8://X_MMTV
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::VectorForMatrix(arg1);

		}
		case 9://X_MVMULT
		{
			Matrix arg1=*ParseMatArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
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
			int arg1=*ParseNumArg(script, ',');
			int arg2=*ParseNumArg(script, ')');
			return Matrix(arg1,arg2);
		}
		case 1://X_MID
		{
			int arg1=*ParseNumArg(script, ')');
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 2://X_MROT
		{
			double arg1=*ParseNumArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 3://X_MADD
		{
			Matrix arg1=*ParseMatArg(script, ',');
			Matrix arg2=*ParseMatArg(script, ')');
			return Matrix::Add(arg1,arg2);
		}
		case 4://X_MSUB
		{
			Matrix arg1=*ParseMatArg(script, ',');
			Matrix arg2=*ParseMatArg(script, ')');
			Matrix temp=Matrix::Scale(arg2, -1);
			return Matrix::Add(arg1,temp);
		}
		case 5://X_MSCALE
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Matrix::Scale(arg1,arg2);
		}
		case 6://X_MMULT
		{
			Matrix arg1=*ParseMatArg(script, ',');
			Matrix arg2=*ParseMatArg(script, ')');
			return Matrix::Multiply(arg1,arg2);

		}
		case 7://X_MINV
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Invert(arg1);
		}
		case 8://X_MTRANS
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Transpose(arg1);
		}
		case 9://X_MVTM
		{
			Vector arg1=*ParseVecArg(script, ')');
			return Matrix::MatrixForVector(arg1);
		}
	}
	return Matrix();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 0:{//X_PRINT
			string str=*XPINSParser::ParseStrArg(script, ')');
			cout<<str;
		}break;
		case 1:{//X_MSET
			Matrix* arg1=ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ',');
			int arg4=*ParseNumArg(script, ')');
			arg1->SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}
