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

#pragma mark Constant Processing

bool XPINSBuiltIn::ParseBoolConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return false;
	++script.index;
	return script.currentChar()=='T';
}
double XPINSBuiltIn::ParseNumConst(XPINSParser::XPINSScriptSpace& script,char stop)
{
	if(script.currentChar()!='~')return 0;
	double val=0;
	int exp=0;
	bool fpart=false;
	bool isNeg=false;
	while(++script.index<script.instructions.length()&&script.currentChar()!=stop&&script.currentChar()!=')'&&script.currentChar()!='e'&&script.currentChar()!='E')
	{
		if(fpart)--exp;//record decimal place
		val*=10;
		if(script.currentChar()=='1')val+=1;
		else if(script.currentChar()=='2')val+=2;
		else if(script.currentChar()=='3')val+=3;
		else if(script.currentChar()=='4')val+=4;
		else if(script.currentChar()=='5')val+=5;
		else if(script.currentChar()=='6')val+=6;
		else if(script.currentChar()=='7')val+=7;
		else if(script.currentChar()=='8')val+=8;
		else if(script.currentChar()=='9')val+=9;
		else if(script.currentChar()!='0')val/=10;
		if(script.currentChar()=='-')isNeg=true;
		if(script.currentChar()=='.')fpart=true;//Start recording decimal places
	}
	bool isENeg=false;
	--script.index;
	while(++script.index<script.instructions.length()&&script.currentChar()!=stop&&script.currentChar()!=')')
	{
		exp*=10;
		if(script.currentChar()=='1')exp+=1;
		else if(script.currentChar()=='2')exp+=2;
		else if(script.currentChar()=='3')exp+=3;
		else if(script.currentChar()=='4')exp+=4;
		else if(script.currentChar()=='5')exp+=5;
		else if(script.currentChar()=='6')exp+=6;
		else if(script.currentChar()=='7')exp+=7;
		else if(script.currentChar()=='8')exp+=8;
		else if(script.currentChar()=='9')exp+=9;
		else if(script.currentChar()!='0')exp/=10;
		if(script.currentChar()=='-')isENeg=true;
	}
	val=(isNeg?-1:1) * val * pow(10, (isENeg?-1:1)*exp);
	return val;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return Vector();
	if(script.matchesString("~<"))//Cartesian vector
	{
		script.index+=2;
		double x=*ParseNumArg(script, ',');
		double y=*ParseNumArg(script, ',');
		double z=*ParseNumArg(script, '>');
		return Vector(x,y ,z);
	}
	else if(script.matchesString("~P<"))//Polar Vector
	{
		script.index+=3;
		double r=*ParseNumArg(script, ',');
		double t=*ParseNumArg(script, ',');
		double z=*ParseNumArg(script, '>');
		return Vector::PolarVector(r, t, z);
	}
	else if(script.matchesString("~S<"))//Spherical Vector
	{
		script.index+=3;
		double r=*ParseNumArg(script, ',');
		double t=*ParseNumArg(script, ',');
		double p=*ParseNumArg(script, '>');
		return Vector::SphericalVector(r, t ,p);
	}
	return Vector();
}


 XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return Matrix();
	size_t rows=1,cols=1;
	script.index+=2;
	int temp=script.index;
	for (;script.currentChar()!='|'&&script.currentChar()!=']'; ++script.index)//Find Column Count
	{
		if(script.currentChar()=='(')//Skip Parenthesis blocks
		{
			++script.index;
			for (int count=1; count>0; ++script.index) {
				if(script.currentChar()=='(')++count;
				else if(script.currentChar()==')')--count;
			}
		}
		if(script.currentChar()==',')++cols;
	}
	for (script.index=temp; script.currentChar()!=']'; ++script.index)//Find Row Count
	{
		if(script.currentChar()=='(')//Skip Parenthesis blocks
		{
			++script.index;
			for (int count=1; count>0; ++script.index) {
				if(script.currentChar()=='(')++count;
				else if(script.currentChar()==')')--count;
			}
		}
		if(script.currentChar()=='|')++rows;
	}
	Matrix mat=Matrix(rows,cols);//Create Matrix
	script.index=temp;
	for(int r=0;r<rows;++r)
	{
		for(int c=0;c<cols;++c)
		{
			double val;
			if(c==cols-1)
			{
				if(r==rows-1)val=*ParseNumArg(script, ']');
				else val=*ParseNumArg(script, '|');
			}
			else val=*ParseNumArg(script, ',');
			mat.SetValueAtPosition(val,r,c);
		}
	}
	return mat;
}

XPINSScriptableMath::Polynomial XPINSBuiltIn::ParsePolyConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return Polynomial();
	while (script.currentChar()!='(')++script.index;
	++script.index;
	vector<Polynomial::Monomial> mons=vector<Polynomial::Monomial>();
	for (int i=0;true;++i)
	{
		mons.resize(i+1);
		mons[i].coeff=*ParseNumArg(script, '_');
		mons[i].exponents=vector<unsigned int>();
		while (script.currentChar()=='_')
		{
			++script.index;
			unsigned int expIndex=0;
			switch (script.currentChar())
			{
				case 'X':
				case 'x':
					expIndex=1;
					break;
				case 'Y':
				case 'y':
					expIndex=2;
					break;
				case 'Z':
				case 'z':
					expIndex=3;
					break;
				case 'T':
				case 't':
					expIndex=4;
					break;
			}
			++script.index;
			if(script.currentChar()=='+'||script.currentChar()=='-'||script.currentChar()==')')
			{
				if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
				mons[i].exponents[expIndex-1]+=1;
				break;
			}
			if(script.currentChar()=='_')
			{
				if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
				mons[i].exponents[expIndex-1]+=1;
			}
			if(expIndex!=0)
			{
				if(expIndex>mons[i].exponents.size())mons[i].exponents.resize(expIndex);
				mons[i].exponents[expIndex-1]+=readInt(script, '_');
			}
		}
		if(script.currentChar()==')')break;
	}
	++script.index;
	return Polynomial(mons);
}
string XPINSBuiltIn::ParseStrConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return "";
	string str="";
	++script.index;
	while (++script.index<script.instructions.length())
	{
		if(script.currentChar()=='\\')
		{
			++script.index;
			switch (script.currentChar()) {
				case 'n':str+='\n';
					break;
				case '\t':str+='\t';
					break;
				case '\\':str+='\\';
					break;
				case '\'':str+='\'';
					break;
				case '\"':str+='\"';
					break;
				case '\r':str+='\r';
					break;
				case 'a':str+='\a';
					break;
				case 'b':str+='\b';
					break;
				case 'f':str+='\f';
					break;
				case 'v':str+='\v';
					break;
				case 'e':str+='\e';
					break;
				default:
					break;
			}
		}
		else if(script.currentChar()=='\"')break;
		else str+=script.currentChar();
	}
	return str;
}
XPINSParser::XPINSArray XPINSBuiltIn::ParseArrConst(XPINSParser::XPINSScriptSpace& script)
{
	if(script.currentChar()!='~')return XPINSArray();
	XPINSArray arr=XPINSArray();
	while (script.currentChar()!='{')++script.index;
	size_t size=1;
	int temp=script.index;
	for (;script.currentChar()!='}'; ++script.index)//Find Col Count
		if(script.currentChar()==',')++size;
	arr.values.resize(size);
	script.index=temp;
	for(int i=0;i<size;++i)
	{
		char type='O';
		if(i==size-1)
			arr.values[i]=*ParsePointerArg(script, '}',&type);
		else
			arr.values[i]=*ParsePointerArg(script, ',',&type);
		arr.types[i]=type;
	}
	return arr;
}

#pragma mark Expression Processing

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
	for (; script.index<script.instructions.length(); ++script.index) {
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
		char ch=script.currentChar();
		switch (ch) {
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
				double a=*ParseNumArg(script,')');
				result*=a;
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
			if(script.instructions[script.index+2]=='M'||script.instructions[script.index+2]=='[')//Matrix-Matrix
			{
				result=result * *ParseMatArg(script, ')');
			}
			else
			{
				double a=*ParseNumArg(script,')');
				result*=a;
			}
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
		case 1://X_PMREACHABLE
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::Reachable(arg1, arg2, arg3);
		}
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
		case 31://X_PMSIM
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::SimulateMarkovChain(arg1, arg2, arg3);
		}
		case 32://X_PMPROB
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			double arg4=*ParseNumArg(script, ',');
			return Probability::TransitionProbability(arg1, arg2, arg3, arg4);
		}
		case 33://X_PMSTEADYSTATE
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			return Probability::SteadyStateProbability(arg1, arg2);

		}
		case 34://X_PMABSORBPROB
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::AbsorbtionProbability(arg1, arg2, arg3);

		}
		case 35://X_PMABSORBTIME
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::AbsorbtionTime(arg1, arg2, arg3);
		}
		case 36://X_PMABSORBSIM
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::SimulateAbsorbtionTime(arg1, arg2, arg3);
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum) {
		case 1://X_VPROJ
		{
			Vector arg1=*ParseVecArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::ProjectionInDirection(arg1,arg2,arg3);
		}
		case 2://X_VUNIT
		{
			Vector arg1=*ParseVecArg(script, ',');
			return Vector::UnitVectorFromVector(arg1);
		}
		case 3://X_MMTV
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
