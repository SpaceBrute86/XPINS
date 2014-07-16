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
	if(script.currentChar()!='~')return new bool(false);
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
		return Vector(x,y ,z, Vector::Cartesian);
	}
	else if(script.matchesString("~P<"))//Polar Vector
	{
		script.index+=3;
		double r=*ParseNumArg(script, ',');
		double t=*ParseNumArg(script, ',');
		double z=*ParseNumArg(script, '>');
		return Vector(r, t, z, Vector::Polar);
	}
	else if(script.matchesString("~S<"))//Spherical Vector
	{
		script.index+=3;
		double r=*ParseNumArg(script, ',');
		double t=*ParseNumArg(script, ',');
		double p=*ParseNumArg(script, '>');
		return Vector(r, t ,p, Vector::Spherical);
	}
	return Vector();
}
XPINSScriptableMath::Quaternion XPINSBuiltIn::ParseQuatConst(XPINSParser::XPINSScriptSpace& script)
{
	if(!script.matchesString("~Q<"))return Quaternion();
	script.index+=3;
	double r=*ParseNumArg(script, ',');
	if(script.instructions[script.index+2]=='V'||script.instructions[script.index+2]=='<'||script.instructions[script.index+3]=='<')
	{
		Vector v=*ParseVecArg(script, '>');
		return Quaternion(r,v);
	}
	
	double x=*ParseNumArg(script, ',');
	double y=*ParseNumArg(script, ',');
	double z=*ParseNumArg(script, '>');
	Vector v=Vector(x,y,z,Vector::Cartesian);
	return Quaternion(r,v);
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
				case 'U':
				case 'u':
					expIndex=5;
					break;
				case 'V':
				case 'v':
					expIndex=6;
					break;
				case 'W':
				case 'w':
					expIndex=7;
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
XPINSScriptableMath::VectorField XPINSBuiltIn::ParseFieldConst(XPINSParser::XPINSScriptSpace& script)
{
	if(!script.matchesString("~F<"))return VectorField();
	script.index+=3;
	Polynomial x=*ParsePolyArg(script, ',');
	Polynomial y=*ParsePolyArg(script, ',');
	Polynomial z=*ParsePolyArg(script, '>');
	return VectorField(x,y,z);
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
	while (script.currentChar()!='{')++script.index;
	size_t size=1;
	int temp=script.index;
	for (;script.currentChar()!='}'; ++script.index)//Find Col Count
		if(script.currentChar()==',')++size;
	XPINSArray arr=XPINSArray();
	arr.values.resize(size);
	script.index=temp;
	for(int i=0;i<size;++i)
	{
		char type='O';
		if(i==size-1)
			arr.values[i]=*ParsePointerArg(script, '}',&type);
		else
			arr.values[i]=*ParsePointerArg(script, ',',&type);
		arr.types+=type;
	}
	return arr;
}

#pragma mark Expression Processing

enum opCode
{
	NOT,OR,AND,//LOGICAL
	LESS,LESSEQ,GREATER,GREATEREQ,NOTEQUAL,EQAUAL,//RELATIONAL
	ADD,SUBTRACT,MULTIPLY,DIVIDE,POWER,MODULUS,//ARITHMATIC
	PREINCREMENT,PREDECREMENT,POSTDECREMENT,POSTINCREMENT,//INCREMENT and DECREMENT
	COMPOSITION,EVALUATE,
	INVALID//INVALID op code
};
enum resType
{
	BOOL,NUM,VEC,QUAT,MAT,POLY,FIELD
};
opCode FindOp(XPINSScriptSpace& script, bool* assign,resType type)
{
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
					if(type==POLY||type==FIELD){
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
					if(type==NUM||type==VEC||type==QUAT||type==MAT||type==POLY||type==FIELD){
						if(script.instructions[script.index+1]=='+')
							return script.instructions[script.index-1]=='('?PREINCREMENT:POSTINCREMENT;
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return ADD;
					}break;
				case '-':
					if(type==NUM||type==VEC||type==QUAT||type==MAT||type==POLY||type==FIELD){
						if(script.instructions[script.index+1]=='-')
							return script.instructions[script.index-1]=='('?PREDECREMENT:POSTDECREMENT;
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return SUBTRACT;
					}break;
				case '*':
					if(type==NUM||type==VEC||type==QUAT||type==MAT||type==POLY||type==FIELD){
						if(assign&&script.instructions[script.index+1]=='=')*assign=true;
						return MULTIPLY;
					}break;
				case '/':
					if(type==NUM||type==VEC||type==QUAT||type==MAT||type==POLY||type==FIELD){
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
					if(type==NUM||type==VEC) return EVALUATE;
					if(type==POLY||type==FIELD){
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
		case 'Q': ParseQuatExp(script);
			break;
		case 'M': ParseMatExp(script);
			break;
		case 'P': ParsePolyExp(script);
			break;
		case 'F': ParseFieldExp(script);
			break;
	}
}
bool XPINSBuiltIn::ParseBoolExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	int temp=script.index;
	opCode op=FindOp(script, NULL,BOOL);
	script.index=temp;
	switch (op)
	{
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
	int temp=script.index;
	opCode op=FindOp(script, NULL,NUM);
	script.index=temp;
	switch (op)
	{
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
			char t1='N';
			char t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='V'&&t2=='V')//Dot Product
			{
				return Vector::DotProduct(*(Vector*)arg1, *(Vector*)arg2);
			}
			else if(t1=='N'&&t2=='N')
			{
				double* var=(double*)arg1;
				double result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else return 0;
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
			for (int i=0;script.currentChar()==','||script.currentChar()==':'; ++i)
			{
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
	int temp=script.index;
	opCode op=FindOp(script, NULL,VEC);
	script.index=temp;
	switch (op)
	{
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
			char t1='N';
			char t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='M'&&t2=='V')//Matrix-Vector
			{
				Matrix mat=*(Matrix*)arg1;
				Vector* var=(Vector*)arg2;
				Vector result=mat*(*var);
				if(assign) *var=result;
				return result;
			}
			else if(t1=='V'&&t2=='V')//Cross Product
			{
				Vector* var=(Vector*)arg1;
				Vector result=*var;
				result=Vector::CrossProduct(result,*(Vector*)arg2);
				if(assign) *var=result;
				return result;
			}
			else if(t1=='V'&&t2=='N')//Vector Scale
			{
				Vector* var=(Vector*)arg1;
				Vector result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;

			}
			else if(t1=='N'&&t2=='V')//Vector Scale
			{
				Vector* var=(Vector*)arg2;
				Vector result=*var;
				double a=*(double*)arg1;
				result*=a;
				return result;
			}
			else return Vector();
		}
		case DIVIDE:
		{
			Vector* var=ParseVecArg(script,'/');
			Vector result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			VectorField* field=ParseFieldArg(script,':');
			vector<double>args=vector<double>();
			for (int i=0;script.currentChar()==','||script.currentChar()==':'; ++i)
			{
				args.resize(i+1);
				double arg=*ParseNumArg(script, ',');
				args[i]=arg;
			}
			return field->Evaluate(args);
		}
		default:return Vector();
			
	}
}
Quaternion XPINSBuiltIn::ParseQuatExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	int temp=script.index;
	opCode op=FindOp(script, NULL,QUAT);
	script.index=temp;
	switch (op)
	{
		case ADD:
		{
			Quaternion* var=ParseQuatArg(script,'+');
			Quaternion result=*var;
			result+=*ParseQuatArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Quaternion* var=ParseQuatArg(script,'-');
			Quaternion result=*var;
			result-=*ParseQuatArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			char t1='N';
			char t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='Q'&&t2=='Q')//Quaternion-Quaternion
			{
				Quaternion* var=(Quaternion*)arg1;
				Quaternion result=*var;
				Quaternion q=*(Quaternion*)arg2;
				result*=q;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='Q'&&t2=='N')//Quatnerion-Scalar
			{
				Quaternion* var=(Quaternion*)arg1;
				Quaternion result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='N'&&t2=='Q')//Quatnerion-Scalar
			{
				Quaternion* var=(Quaternion*)arg2;
				Quaternion result=*var;
				double a=*(double*)arg1;
				result*=a;
				return result;
			}
			else return Quaternion();
		}
		case DIVIDE:
		{
			Quaternion* var=ParseQuatArg(script,'/');
			Quaternion result=*var;
			result/=*ParseNumArg(script,')');
			if(assign) *var=result;
			return result;
		}
		default:return Quaternion();
	}
}
Matrix XPINSBuiltIn::ParseMatExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	int temp=script.index;
	opCode op=FindOp(script, NULL,MAT);
	script.index=temp;
	switch (op)
	{
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
			char t1='N',t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='M'&&(t2=='M'||t2=='V'))//Matrix-Matrix
			{
				Matrix* var=(Matrix*)arg1;
				Matrix result=*var;
				Matrix a=(t2=='M')?(*(Matrix*)arg2):(Matrix::MatrixForVector(*(Vector*)arg2));
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='M'&&t2=='N')//Matrix Scale
			{
				Matrix* var=(Matrix*)arg1;
				Matrix result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='N'&&t2=='M')//Matrix Scale
			{
				Matrix* var=(Matrix*)arg2;
				Matrix result=*var;
				double a=*(double*)arg1;
				result*=a;
				return result;
			}
			else return Matrix();
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
	int temp=script.index;
	opCode op=FindOp(script, NULL,POLY);
	script.index=temp;
	switch (op)
	{
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
			char t1='N';
			char t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='P'&&t2=='P')//Polynomial-Polynomial
			{
				Polynomial* var=(Polynomial*)arg1;
				Polynomial result=*var;
				Polynomial a=*(Polynomial*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='P'&&t2=='N')//Polynomial-Scalar
			{
				Polynomial* var=(Polynomial*)arg1;
				Polynomial result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='N'&&t2=='P')//Polynomial-Scalar
			{
				Polynomial* var=(Polynomial*)arg2;
				Polynomial result=*var;
				double a=*(double*)arg1;
				result*=a;
				return result;
			}
			else if((t1=='F'||t1=='V')&&(t2=='F'||t2=='V'))//Vector Field Dot Product
			{
				if(t1=='N')arg1=new VectorField(*(VectorField*)arg1);
				if(t2=='N')arg2=new VectorField(*(VectorField*)arg2);
				return VectorField::Dot(*(VectorField*)arg1, *(VectorField*)arg2);
			}
			else return Polynomial();
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
VectorField XPINSBuiltIn::ParseFieldExp(XPINSScriptSpace& script){
	while(script.currentChar()!='?')++script.index;
	script.index+=3;
	bool assign=false;
	int temp=script.index;
	opCode op=FindOp(script, NULL,FIELD);
	script.index=temp;
	switch (op)
	{
		case ADD:
		{
			VectorField* var=ParseFieldArg(script,'+');
			VectorField result=*var;
			result+=*ParseFieldArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			VectorField* var=ParseFieldArg(script,'-');
			VectorField result=*var;
			result-=*ParseVecArg(script,')');
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			char t1='N';
			char t2='N';
			void* arg1=ParseArg(script, '*', t1);
			void* arg2=ParseArg(script, ')', t2);
			if(t1=='M'&&t2=='F')//Matrix-VectorField
			{
				Matrix mat=*(Matrix*)arg1;
				VectorField* var=(VectorField*)arg2;
				VectorField result=VectorField::MatMult(mat,*var);
				if(assign) *var=result;
				return result;
			}
			else if((t1=='F'||t1=='V')&&(t2=='F'||t2=='V'))//Cross Product
			{
				VectorField result=(t1=='F')?*(VectorField*)arg1:VectorField(*(Vector*)arg1);
				VectorField v2=(t2=='F')?*(VectorField*)arg2:VectorField(*(Vector*)arg2);
				result=VectorField::Cross(result,v2);
				if(t1=='F'&&assign) *(VectorField*)arg1=result;
				return result;
			}
			else if(t1=='F'&&t2=='P')//VectorField Scale
			{
				VectorField* var=(VectorField*)arg1;
				VectorField result=*var;
				Polynomial a=*(Polynomial*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='P'&&t2=='F')//VectorField Scale
			{
				VectorField* var=(VectorField*)arg2;
				VectorField result=*var;
				Polynomial a=*(Polynomial*)arg1;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='F'&&t2=='N')//VectorField Scale
			{
				VectorField* var=(VectorField*)arg1;
				VectorField result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1=='N'&&t2=='F')//VectorField Scale
			{
				VectorField* var=(VectorField*)arg2;
				VectorField result=*var;
				double a=*(double*)arg1;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else return Vector();
		}
		case DIVIDE:
		{
			VectorField* var=ParseFieldArg(script,'/');
			VectorField result=*var;
			result*= 1/(*ParseNumArg(script,')'));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			VectorField* var=ParseFieldArg(script,'&');
			Polynomial inner=*ParsePolyArg(script,',');
			int variable=*ParseNumArg(script, ')');
			VectorField result=VectorField::Compose(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			VectorField* var=ParseFieldArg(script,':');
			int value=*ParseNumArg(script,',');
			int variable=*ParseNumArg(script, ')');
			VectorField result=var->PartialEvaluate(value,variable);
			if(assign) *var=result;
			return result;
		}
		default:return Vector();
	}
}
#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum)
	{
		case 1://X_MARKOV_REACHABLE
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
	switch (fNum)
	{
		case 1://X_SIN
		{
			double arg1=*ParseNumArg(script, ')');
			return sin(arg1);
		}
		case 2://X_COS
		{
			double arg1=*ParseNumArg(script, ')');
			return cos(arg1);
		}
		case 3://X_TAN
		{
			double arg1=*ParseNumArg(script, ')');
			return tan(arg1);
		}
		case 4://X_ASIN
		{
			double arg1=*ParseNumArg(script, ',');
			return asin(arg1);
		}
		case 5://X_ACOS
		{
			double arg1=*ParseNumArg(script, ',');
			return acos(arg1);
		}
		case 6://X_ATAN
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
		case 12://X_ADDPOLAR
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Vector::AddPolar(arg1, arg2);
		}
		case 13://X_DIST
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ')');
			return Vector::Dist(arg1, arg2,arg3);
		}
		case 14://X_VX
		{
			Vector arg1=*ParseVecArg(script, ')');
			double x=0;
			arg1.Coords(&x, NULL,NULL,Vector::Cartesian);
			return x;
		}
		case 15://X_VY
		{
			Vector arg1=*ParseVecArg(script, ')');
			double y=0;
			arg1.Coords(NULL, &y, NULL,Vector::Cartesian);
			return y;
		}
		case 16://X_VZ
		{
			Vector arg1=*ParseVecArg(script, ')');
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Cartesian);
			return z;
		}
		case 17://X_VR
		{
			Vector arg1=*ParseVecArg(script, ')');
			double r=0;
			arg1.Coords(&r, NULL, NULL,Vector::Polar);
			return r;
		}
		case 18://X_VTHETA
		{
			Vector arg1=*ParseVecArg(script, ')');
			double t=0;
			arg1.Coords(NULL,&t, NULL,Vector::Polar);
			return t;
		}
		case 19://X_VMAG
		{
			Vector arg1=*ParseVecArg(script, ')');
			return arg1.Magnitude();
		}
		case 20://X_VPHI
		{
			Vector arg1=*ParseVecArg(script, ')');
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Spherical);
			return z;
		}
		case 21://X_VECTOR_ANGLE
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Vector::AngleBetweenVectors(arg1, arg2);
			
		}
		case 22://X_MATRIX_GET
		{
			Matrix arg1=*ParseMatArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ')');
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 23://X_DETERMINANT
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Determinant(arg1);
		}
		case 24://X_RAND
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::UniformRV(arg1, arg2-arg1);
		}
		case 25://X_RV_BERNOULLI
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::BernoulliRV(arg1);
		}
		case 26://X_RV_NOMRAL
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::NormalRV(arg1, arg2);
		}
		case 27://X_RV_EXP
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::ExponentialRV(arg1);
		}
		case 28://X_RV_POISSON
		{
			double arg1=*ParseNumArg(script, ',');
			return Probability::PoissonRV(arg1);
		}
		case 29://X_COIN_FLIP
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::CoinFlip(arg1, arg2);
		}
		case 30://X_DICE_ROLL
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ')');
			return Probability::FairDiceRoll(arg1, arg2);
		}
		case 31://X_MARKOV_SIM
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::SimulateMarkovChain(arg1, arg2, arg3);
		}
		case 32://X_MARKOV_PROB
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			double arg4=*ParseNumArg(script, ',');
			return Probability::TransitionProbability(arg1, arg2, arg3, arg4);
		}
		case 33://X_MARKOV_STEADYSTATE
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			return Probability::SteadyStateProbability(arg1, arg2);

		}
		case 34://X_MARKOV_ABSORB_PROB
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::AbsorbtionProbability(arg1, arg2, arg3);

		}
		case 35://X_MARKOV_ABSORB_TIME
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::AbsorbtionTime(arg1, arg2, arg3);
		}
		case 36://X_MARKOV_ABSORB_SIM
		{
			Matrix arg1=*ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Probability::SimulateAbsorbtionTime(arg1, arg2, arg3);
		}
		case 37://X_QR
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			double r;
			arg1.Components(NULL,NULL,NULL,&r);
			return r;
		}
		case 38://X_QMAG
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			return arg1.Magnitude();
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum)
	{
		case 1://X_VPROJECT
		{
			Vector arg1=*ParseVecArg(script, ',');
			Vector arg2=*ParseVecArg(script, ',');
			return Vector::ProjectionOntoVector(arg1,arg2);
		}
		case 2://X_UNIT_VECTOR
		{
			Vector arg1=*ParseVecArg(script, ',');
			return Vector::UnitVectorFromVector(arg1);
		}
		case 3://X_QV
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			double x,y,z;
			arg1.Components(&x, &y, &z, NULL);
			return Vector(x,y,z,Vector::Cartesian);
		}
		case 4://X_QUATERNION_ROTATE
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			Vector arg2=*ParseVecArg(script, ',');
			return Quaternion::RotateVector(arg1, arg2);
		}
	}
	return Vector();
}
XPINSScriptableMath::Quaternion XPINSBuiltIn::ParseQuatBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum)
	{
		case 1://X_QUATERNION_CONJUGATE
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			return Quaternion::ConjugateQuaternion(arg1);
		}
		case 2://X_QUATERNION_INVERSE
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			return Quaternion::InvertQuaternion(arg1);
		}
		case 3://X_UNIT_QUATERNION
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			return Quaternion::UnitQuaternion(arg1);
		}
	}
	return Quaternion();
}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum)
	{
		case 1://X_ZERO_MATRIX
		{
			int arg1=*ParseNumArg(script, ',');
			int arg2=*ParseNumArg(script, ')');
			return Matrix(arg1,arg2);
		}
		case 2://X_IDENTITY_MATRIX
		{
			int arg1=*ParseNumArg(script, ')');
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 3://X_ROTATION_MATRIX
		{
			double arg1=*ParseNumArg(script, ',');
			Vector arg2=*ParseVecArg(script, ')');
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 4://X_EULER_ANGLE_MATRIX
		{
			double arg1=*ParseNumArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			double arg3=*ParseNumArg(script, ',');
			return Matrix::RotationMatrixWithEulerAngles(arg1, arg2, arg3);
		}
		case 5://X_QUATERNION_MATRIX
		{
			Quaternion arg1=*ParseQuatArg(script, ',');
			return Matrix::RotationMatrixWithQuaternion(arg1);
		}
		case 6://X_INVERT
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Invert(arg1);
		}
		case 7://X_TRANSPOSE
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::Transpose(arg1);
		}
		case 8://X_APPEND
		{
			Matrix arg1=*ParseMatArg(script, ',');
			Matrix arg2=*ParseMatArg(script, ')');
			return Matrix::Append(arg1, arg2);
		}
		case 9://X_ROW_ECHELON
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::RowEchelon(arg1);
		}
		case 10://X_REDUCED_ROW_ECHELON
		{
			Matrix arg1=*ParseMatArg(script, ')');
			return Matrix::ReducedRowEchelon(arg1);
		}
	}
	return Matrix();
}
XPINSScriptableMath::Polynomial XPINSBuiltIn::ParsePolyBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum)
	{
		case 1://X_DERIVE
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			return Polynomial::Derivative(arg1, arg2);
		}
		case 2://X_INTEGRATE
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			int arg2=*ParseNumArg(script, ',');
			return Polynomial::Integrate(arg1, arg2);
		}
		case 3://X_DIVERGENCE
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			return arg1.Divergence();
		}
		case 4://X_SCALAR_LINE_INTEGRAL
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			VectorField arg2=*ParseFieldArg(script, ',');
			Polynomial arg3=*ParsePolyArg(script, ',');
			Polynomial arg4=*ParsePolyArg(script, ',');
			Bound::Bound(arg3,arg4);
			return VectorField::LineIntegral(arg1, arg2, Bound(arg3,arg4));
		}
		case 5://X_VECTOR_LINE_INTEGRAL
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			VectorField arg2=*ParseFieldArg(script, ',');
			Polynomial arg3=*ParsePolyArg(script, ',');
			Polynomial arg4=*ParsePolyArg(script, ',');
			return VectorField::LineIntegral(arg1, arg2, Bound(arg3,arg4));
		}
		case 6://X_SCALAR_SURFACE_INTEGRAL
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			VectorField arg2=*ParseFieldArg(script, ',');
			Polynomial arg3=*ParsePolyArg(script, ',');
			Polynomial arg4=*ParsePolyArg(script, ',');
			Polynomial arg5=*ParsePolyArg(script, ',');
			Polynomial arg6=*ParsePolyArg(script, ',');
			return VectorField::SurfaceIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6));
		}
		case 7://X_VECTOR_LINE_INTEGRAL
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			VectorField arg2=*ParseFieldArg(script, ',');
			Polynomial arg3=*ParsePolyArg(script, ',');
			Polynomial arg4=*ParsePolyArg(script, ',');
			Polynomial arg5=*ParsePolyArg(script, ',');
			Polynomial arg6=*ParsePolyArg(script, ',');
			return VectorField::SurfaceIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6));
		}
		case 8://X_VOLUME_INTEGRAL
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			VectorField arg2=*ParseFieldArg(script, ',');
			Polynomial arg3=*ParsePolyArg(script, ',');
			Polynomial arg4=*ParsePolyArg(script, ',');
			Polynomial arg5=*ParsePolyArg(script, ',');
			Polynomial arg6=*ParsePolyArg(script, ',');
			Polynomial arg7=*ParsePolyArg(script, ',');
			Polynomial arg8=*ParsePolyArg(script, ',');
			return VectorField::VolumeIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6),Bound(arg7,arg8));
		}
	}
	return Polynomial();
}
XPINSScriptableMath::VectorField XPINSBuiltIn::ParseFieldBIF(int fNum, XPINSScriptSpace& script)
{
	
	switch (fNum)
	{
		case 1://X_GRADIENT_VECTOR
		{
			Polynomial arg1=*ParsePolyArg(script, ',');
			return VectorField::GradientField(arg1);
		}
		case 2://X_VECTOR_DERIVE
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			double  arg2=*ParseNumArg(script, ',');
			return VectorField::Derivative(arg1, arg2);
		}
		case 3://X_VECTOR_INTEGRATE
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			double  arg2=*ParseNumArg(script, ',');
			return VectorField::Integrate(arg1, arg2);
		}
		case 4://X_CURL
		{
			VectorField arg1=*ParseFieldArg(script, ',');
			return arg1.Curl();
		}
	}
	return VectorField();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSScriptSpace& script)
{
	switch (fNum) {
		case 1://X_PRINT
		{
			string str=*XPINSParser::ParseStrArg(script, ')');
			cout<<str;
		}break;
		case 2://X_MSET
		{
			Matrix* arg1=ParseMatArg(script, ',');
			double arg2=*ParseNumArg(script, ',');
			int arg3=*ParseNumArg(script, ',');
			int arg4=*ParseNumArg(script, ')');
			arg1->SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}