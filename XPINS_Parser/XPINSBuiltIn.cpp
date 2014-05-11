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

enum opCode{
	NOT,OR,AND,//LOGICAL
	LESS,LESSEQ,GREATER,GREATEREQ,NOTEQUAL,EQAUAL,//RELATIONAL
	ADD,SUBTRACT,MULTIPLY,DIVIDE,POWER,MODULUS,//ARITHMATIC
	PREINCREMENT,PREDECREMENT,POSTDECREMENT,POSTINCREMENT,//INCREMENT and DECREMENT
	COMPOSITION,EVALUATE,
	INVALID//INVALID op code
};
enum resType{
	BOOL,NUM,VEC,MAT,POLY
};
opCode FindOp(XPINSScriptSpace sc, bool* assign,resType type)
{
	XPINSScriptSpace script=sc;
	for (; true; ++script.index) {
		if(script.currentChar()=='(')
		{
			for (int i=1; i>0;) {
				++script.index;
				if(script.currentChar()=='(')
					++i;
				else if(script.currentChar()==')')
					--i;
			}
			++script.index;
		}
		switch (script.currentChar()) {
				case '|':
					if(type==BOOL)return OR;
					break;
				case '&':
					if(type==BOOL)return AND;
					if(type==POLY){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return COMPOSITION;
					}break;
				case '!':
					if(type==BOOL)return script.instructions[script.index+1]=='='?NOTEQUAL:NOT;
					break;
				case '<':
					if(type==BOOL)return script.instructions[script.index+1]=='='?LESSEQ:LESS;
					break;
				case '>':
					if(type==BOOL)return script.instructions[script.index+1]=='='?GREATEREQ:GREATER;
					break;
				case '=':
					if(type==BOOL) return EQAUAL;
					break;
				case '+':
					if(type==NUM||type==VEC||type==MAT||type==POLY){
						if(script.instructions[script.index+1]=='+')
							return script.instructions[script.index-1]=='('?PREINCREMENT:POSTINCREMENT;
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return ADD;
					}break;
				case '-':
					if(type==NUM||type==VEC||type==MAT||type==POLY){
						if(script.instructions[script.index+1]=='-')
							return script.instructions[script.index-1]=='('?PREDECREMENT:POSTDECREMENT;
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return SUBTRACT;
					}break;
				case '*':
					if(type==NUM||type==VEC||type==MAT||type==POLY){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return MULTIPLY;
					}break;
				case '/':
					if(type==NUM||type==VEC||type==MAT||type==POLY){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return DIVIDE;
					}break;
				case '^':
					if(type==NUM||type==POLY){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return POWER;
					}break;
				case '%':
					if(type==NUM){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return MODULUS;
					}break;
				case ':':
					if(type==NUM) return EVALUATE;
					if(type==POLY){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return EVALUATE;
					}break;
		}
	}
	return INVALID;
}


#pragma mark Expression Processing

void XPINSBuiltIn::ParseVoidExp(XPINSParser::XPINSScriptSpace& script)
{
	switch (script.instructions[script.index+1])
	{
		case 'B': ParseBoolExp(script);
			break;
		case 'N': ParseNumExp(script);
			break;
		case 'V': ParseVecExp(script);
			break;
		case 'M': ParseMatExp(script);
			break;
	}
}
bool XPINSBuiltIn::ParseBoolExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	switch (FindOp(script, NULL,BOOL)) {
		case NOT:
		{
			bool arg=*ParseBoolArg(script, ')');
			return !arg;
		}
		case OR:
		{
			bool arg1=*ParseBoolArg(script,'|');
			script.index+=2;
			bool arg2=*ParseBoolArg(script,')');
			return arg1||arg2;
		}
		case AND:
		{
			bool arg1=*ParseBoolArg(script,'&');
			script.index+=2;
			bool arg2=*ParseBoolArg(script,')');
			return arg1&&arg2;
		}
		case LESS:
		{
			double arg1=*ParseNumArg(script,'<');
			double arg2=*ParseNumArg(script,')');
			return arg1<arg2;
		}
		case LESSEQ:
		{
			double arg1=*ParseNumArg(script,'<');
			double arg2=*ParseNumArg(script,')');
			return arg1<=arg2;
		}
		case GREATER:
		{
			double arg1=*ParseNumArg(script,'>');
			double arg2=*ParseNumArg(script,')');
			return arg1>arg2;
		}
		case GREATEREQ:
		{
			double arg1=*ParseNumArg(script,'>');
			double arg2=*ParseNumArg(script,')');
			return arg1>=arg2;
		}
		case NOTEQUAL:
		{
			double arg1=*ParseNumArg(script,'!');
			double arg2=*ParseNumArg(script,')');
			return arg1!=arg2;
		}
		case EQAUAL:
		{
			double arg1=*ParseNumArg(script,'=');
			double arg2=*ParseNumArg(script,')');
			return arg1==arg2;
		}
		default:return false;
	}
}
double XPINSBuiltIn::ParseNumExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	switch (FindOp(script, &assign,NUM)) {
		case ADD:
		{
			double* var=ParseNumArg(script,'+');
			double result=*var;
			result+=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			double* var=ParseNumArg(script,'-');
			double result=*var;
			result-=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			if(script.instructions[script.index+1]=='V'||script.instructions[script.index+1]=='<'||script.instructions[script.index+1]=='P'||script.instructions[script.index+1]=='S')//Dot Product
			{
				Vector v1=*ParseVecArg(script, '*');
				Vector v2=*ParseVecArg(script, ')');
				return Vector::DotProduct(v1, v2);
			}
			else
			{
				double* var=ParseNumArg(script,'*');
				double result=*var;
				result*=*ParseNumArg(script,')');
				if(assign) *var=result;
				return result;
			}
		}
		case DIVIDE:
		{
			double* var=ParseNumArg(script,'/');
			double result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			double* var=ParseNumArg(script,'^');
			double result=*var;
			result=pow(result,*ParseNumArg(script,')'));
			if(assign) *var=result;
			return result;
		}
		case MODULUS:
		{
			double* var=ParseNumArg(script,'%');
			double result=*var;
			result-=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case PREINCREMENT:
		{
			script.index+=2;
			return ++(*ParseNumArg(script,')'));
		}
		case POSTINCREMENT:
		{
			return (*ParseNumArg(script,'+'))++;
		}
		case PREDECREMENT:
		{
			script.index+=2;
			return --(*ParseNumArg(script,')'));
		}
		case POSTDECREMENT:
		{
			return (*ParseNumArg(script,'-'))--;
		}
		case EVALUATE:
		{
			Polynomial* poly=ParsePolyArg(script,':');
			vector<double>args=vector<double>();
			for (int i=0;script.currentChar()==','||script.currentChar()==':'; ++i) {
				args.resize(i+1);
				double arg=*ParseNumArg(script, ',');
				args[i]=arg;
			}
			return poly->Evaluate(args);
		}
		default:return 0.0;
	}
}
Vector XPINSBuiltIn::ParseVecExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	switch (FindOp(script, &assign,VEC)) {
		case ADD:
		{
			Vector* var=ParseVecArg(script,'+');
			Vector result=*var;
			result+=*ParseVecArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Vector* var=ParseVecArg(script,'-');
			Vector result=*var;
			result-=*ParseVecArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			if(script.instructions[script.index+1]=='M'||script.instructions[script.index+1]=='[')//Matrix
			{
				Matrix mat=*ParseMatArg(script, '*');
				Vector* var=ParseVecArg(script,')');
				Vector result=mat*(*var);
				if(assign) *var=result;
				return result;
			}
			else{
				Vector* var=ParseVecArg(script,'*');
				Vector result=*var;
				if(script.instructions[script.index+2]=='V'||script.instructions[script.index+2]=='<'||script.	instructions[script.index+2]=='P'||script.instructions[script.index+2]=='S')//Cross Product
				{
					result=Vector::CrossProduct(result, *ParseVecArg(script, ')'));
				}
				else result*=*ParseNumArg(script,')');
				if(assign) *var=result;
				return result;
			}
		}
		case DIVIDE:
		{
			Vector* var=ParseVecArg(script,'/');
			Vector result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		default:return Vector();
	}
}
Matrix XPINSBuiltIn::ParseMatExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	switch (FindOp(script, &assign,MAT)) {
		case ADD:
		{
			Matrix* var=ParseMatArg(script,'+');
			Matrix result=*var;
			result+=*ParseMatArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Matrix* var=ParseMatArg(script,'-');
			Matrix result=*var;
			result-=*ParseMatArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			Matrix* var=ParseMatArg(script,'*');
			Matrix result=*var;
			if(script.instructions[script.index+2]=='M'||script.instructions[script.index+2]=='[')//Cross Product
			{
				result=result* *ParseMatArg(script, ')');
			}
			else result*=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
			
		}
		case DIVIDE:
		{
			Matrix* var=ParseMatArg(script,'/');
			Matrix result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		default:return Matrix();
	}
}
Polynomial XPINSBuiltIn::ParsePolyExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	switch (FindOp(script, &assign,POLY)) {
		case ADD:
		{
			Polynomial* var=ParsePolyArg(script,'+');
			Polynomial result=*var;
			result+=*ParsePolyArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Polynomial* var=ParsePolyArg(script,'-');
			Polynomial result=*var;
			result-=*ParsePolyArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			Polynomial* var=ParsePolyArg(script,'*');
			Polynomial result=*var;
			if(script.instructions[script.index+2]=='P'||script.instructions[script.index+2]=='(')//Cross Product
			{
				result=Polynomial::Multiply(result, *ParsePolyArg(script, ')'));
			}
			else result*=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case DIVIDE:
		{
			Polynomial* var=ParsePolyArg(script,'/');
			Polynomial result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			Polynomial* var=ParsePolyArg(script,'^');
			Polynomial result=Polynomial::Power(*var, *ParseNumArg(script,')'));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			Polynomial* var=ParsePolyArg(script,'&');
			Polynomial inner=*ParsePolyArg(script,',');
			int variable=*ParseNumArg(script, ')');
			Polynomial result=Polynomial::Composition(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			Polynomial* var=ParsePolyArg(script,':');
			int value=*ParseNumArg(script,',');
			int variable=*ParseNumArg(script, ')');
			Polynomial result=var->PartialEvaluate(value,variable);
			if(assign) *var=result;
			return result;
		}
		default:return Polynomial();
	}
}
#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		default: return false;
	}
}

double XPINSBuiltIn::ParseNumBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 1://X_TSIN
		{
			double arg1=*ParseNumArg(script, ')');
			return sin(arg1);
		}
		case 2://X_TCOS
		{
			double arg1=*ParseNumArg(script, ')');
			return cos(arg1);
		}
		case 3://X_TTAN
		{
			double arg1=*ParseNumArg(script, ')');
			return tan(arg1);
		}
		case 4://X_TASIN
		{
			double arg1=*ParseNumArg(script, ',');
			return asin(arg1);
		}
		case 5://X_TACOS
		{
			double arg1=*ParseNumArg(script, ',');
			return acos(arg1);
		}
		case 6://X_TATAN
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return atan2(arg1, arg2);
		}
		case 7://X_SQRT
		{
			double arg1=*ParseNumArg(script, ',');
			return sqrt(arg1);
		}
		case 8://X_LN
		{
			double arg1=*ParseNumArg(script, ',');
			return log(arg1);
		}
		case 9://X_LOG
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			return log(arg1)/log(arg2);
		}
		case 10://X_ABS
		{
			double arg1=*ParseNumArg(script, ',');
			return fabs(arg1);
		}
		case 11://X_FLOOR
		{
			double arg1=*ParseNumArg(script, ',');
			return floor(arg1);
		}
		case 12://X_VADDPOLAR
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Vector::AddPolar(arg1, arg2);
		}
		case 13://X_VDIST
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::Dist(arg1, arg2,arg3);
		}
		case 14://X_VX
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return x;
		}
		case 15://X_VY
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return y;
		}
		case 16://X_VZ
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.RectCoords(&x, &y, &z);
			return z;
		}
		case 17://X_VR
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return x;
		}
		case 18://X_VTHETA
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.PolarCoords(&x, &y, &z);
			return y;
		}
		case 19://X_VRHO
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return x;
		}
		case 20://X_VPHI
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0,y=0,z=0;
			arg1.SphericalCoords(&x, &y, &z);
			return z;
		}
		case 21://X_VANG
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::AngleBetweenVectors(arg1, arg2);
			
		}
		case 22://X_MGET
		{
			Matrix arg1=*ParseMatArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ')');
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 23://X_MDET
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Determinant(arg1);
		}
		case 24://X_PRAND
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::UniformRV(arg1, arg2-arg1);
		}
		case 25://X_PBERN
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::BernoulliRV(arg1);
		}
		case 26://X_PNormal
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::NormalRV(arg1, arg2);
		}
		case 27://X_PEXP
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::ExponentialRV(arg1);
		}
		case 28://X_PPOISSON
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::PoissonRV(arg1);
		}
		case 29://X_PCOIN
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::CoinFlip(arg1, arg2);
		}
		case 30://X_PDICE
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::FairDiceRoll(arg1, arg2);
		}
		case 34://X_AEVAL
		{
			Polynomial poly=*ParsePolyArg(script, ',');
			vector<double>args=vector<double>();
			for (int i=0;script.currentChar()==','; ++i) {
				args.resize(i+1);
				double arg=*ParseNumArg(script, ',');
				args[i]=arg;
			}
			return poly.Evaluate(args);
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 1://X_VREC
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector(arg1,arg2,arg3);
		}
		case 2://X_VPOL
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::PolarVector(arg1,arg2,arg3);
		}
		case 3://X_VSHPERE
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::SphericalVector(arg1,arg2,arg3);
		}
		case 4://X_VPROJ
		{
			Vector arg1=*ParseVecArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 5://X_MMTV
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::VectorForMatrix(arg1);
			
		}
	}
	return Vector();
	
}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 1://X_MMAKE
		{
			int arg1=*ParseNumArg(script, ',');
			int arg2=*ParseNumArg(script, ')');
			return Matrix(arg1,arg2);
		}
		case 2://X_MID
		{
			int arg1=*ParseNumArg(script, ')');
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 3://X_MROT
		{
			double arg1=*ParseNumArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 4://X_MINV
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Invert(arg1);
		}
		case 5://X_MTRANS
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Transpose(arg1);
		}
		case 6://X_MVTM
		{
			Vector arg1=*ParseVecArg(script, ')');
			return Matrix::MatrixForVector(arg1);
		}
	}
	return Matrix();
}
XPINSScriptableMath::Polynomial XPINSBuiltIn::ParsePolyBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 1://X_ADERIVE
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			return Polynomial::Derivative(arg1, arg2);
		}
		case 2://X_AINTEGRATE
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			return Polynomial::Integrate(arg1, arg2);
		}
	}
	return Polynomial();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 1:{//X_PRINT
			string str=*XPINSParser::ParseStrArg(script, ')');
			cout<<str;
		}break;
		case 2:{//X_MSET
			Matrix* arg1=ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ',');
			int arg4=*ParseNumArg(script, ')');
			arg1->SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}
