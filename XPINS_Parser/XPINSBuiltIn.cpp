//
//  XPINSBuiltIn.cpp
//  XPINS
//
//  Created by Robbie Markwick on 3/23/14.
//
//

#include "XPINSParser.h"
#include "XPINSBindings.h"
#include "XPINSBIFMap.h"

#include <math.h>

using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;

#pragma mark Expression Processing

bool XPINSBuiltIn::ParseBoolExp(opCode op,bool assign,XPINSScriptSpace& script, vector<Argument> args){
	switch (op)
	{
		case NOT:
		{
			bool* arg=ParseBoolArg(script, args[0]);
			bool result=!*arg;
			if(assign)*arg=result;
			return result;
		}
		case OR:
		{
			bool* arg1=ParseBoolArg(script,args[0]);
			bool result=*arg1||*ParseBoolArg(script,args[1]);
			if(assign)*arg1=result;
			return result;
		}
		case AND:
		{
			bool* arg1=ParseBoolArg(script,args[0]);
			bool result=*arg1&&*ParseBoolArg(script,args[1]);
			if(assign)*arg1=result;
			return result;
		}
		case LESS:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1<arg2;
		}
		case LESSEQ:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1<=arg2;
		}
		case GREATER:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1>arg2;
		}
		case GREATEREQ:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1>=arg2;
		}
		case NOTEQUAL:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1!=arg2;
		}
		case EQAUAL:
		{
			double arg1=*ParseNumArg(script,args[0]);
			double arg2=*ParseNumArg(script,args[1]);
			return arg1==arg2;
		}
		default:return false;
	}
}
double XPINSBuiltIn::ParseNumExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument> args){
	switch (op)
	{
		case ADD:
		{
			double* var=ParseNumArg(script,args[0]);
			double result=*var;
			result+=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			double* var=ParseNumArg(script,args[0]);
			double result=*var;
			result-=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER;
			DataType t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==VECTOR&&t2==VECTOR)//Dot Product
			{
				return Vector::DotProduct(*(Vector*)arg1, *(Vector*)arg2);
			}
			else if(t1==NUMBER&&t2==NUMBER)
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
			double* var=ParseNumArg(script,args[0]);
			double result=*var;
			result/=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			double* var=ParseNumArg(script,args[0]);
			double result=*var;
			result=pow(result,*ParseNumArg(script,args[1]));
			if(assign) *var=result;
			return result;
		}
		case REMAINDER:
		{
			double* var=ParseNumArg(script,args[0]);
			
			double num2 = (int)*ParseNumArg(script,args[1]);
			double quotient= *var / num2;
			quotient-=floor(quotient);
			double result=*var-num2*quotient;
			if(assign) *var=result;
			return result;
		}
		case PREINCREMENT:
		{
			return ++(*ParseNumArg(script,args[0]));
		}
		case POSTINCREMENT:
		{
			return (*ParseNumArg(script,args[0]))++;
		}
		case PREDECREMENT:
		{
			return --(*ParseNumArg(script,args[0]));
		}
		case POSTDECREMENT:
		{
			return (*ParseNumArg(script,args[0]))--;
		}
		case EVALUATE:
		{
			Polynomial* poly=ParsePolyArg(script,args[0]);
			vector<double>nums=vector<double>();
			for (int i=1;i<args.size(); ++i)
			{
				nums.resize(i);
				nums[i-1]=*ParseNumArg(script, args[i]);
			}
			return poly->Evaluate(nums);
		}
		default:return 0.0;
	}
}
Vector XPINSBuiltIn::ParseVecExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument> args){
	switch (op)
	{
		case ADD:
		{
			Vector* var=ParseVecArg(script,args[0]);
			Vector result=*var;
			result+=*ParseVecArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Vector* var=ParseVecArg(script,args[0]);
			Vector result=*var;
			result-=*ParseVecArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER;
			DataType t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==MATRIX&&t2==VECTOR)//Matrix-Vector
			{
				Matrix mat=*(Matrix*)arg1;
				Vector* var=(Vector*)arg2;
				Vector result=mat*(*var);
				if(assign) *var=result;
				return result;
			}
			else if(t1==VECTOR&&t2==VECTOR)//Cross Product
			{
				Vector* var=(Vector*)arg1;
				Vector result=*var;
				result=Vector::CrossProduct(result,*(Vector*)arg2);
				if(assign) *var=result;
				return result;
			}
			else if(t1==VECTOR&&t2==NUMBER)//Vector Scale
			{
				Vector* var=(Vector*)arg1;
				Vector result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;

			}
			else if(t1==NUMBER&&t2==VECTOR)//Vector Scale
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
			Vector* var=ParseVecArg(script,args[0]);
			Vector result=*var;
			result/=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			VectorField* field=ParseFieldArg(script,args[0]);
			vector<double>nums=vector<double>();
			for (int i=1;i<args.size(); ++i)
			{
				nums.resize(i+1);
				nums[i]=*ParseNumArg(script, args[i]);
			}
			return field->Evaluate(nums);
		}
		default:return Vector();
			
	}
}
Quaternion XPINSBuiltIn::ParseQuatExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument> args){
	switch (op)
	{
		case ADD:
		{
			Quaternion* var=ParseQuatArg(script,args[0]);
			Quaternion result=*var;
			result+=*ParseQuatArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Quaternion* var=ParseQuatArg(script,args[0]);
			Quaternion result=*var;
			result-=*ParseQuatArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER;
			DataType t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==QUATERNION&&t2==QUATERNION)//Quaternion-Quaternion
			{
				Quaternion* var=(Quaternion*)arg1;
				Quaternion result=*var;
				Quaternion q=*(Quaternion*)arg2;
				result*=q;
				if(assign) *var=result;
				return result;
			}
			else if(t1==QUATERNION&&t2==NUMBER)//Quatnerion-Scalar
			{
				Quaternion* var=(Quaternion*)arg1;
				Quaternion result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==QUATERNION)//Quatnerion-Scalar
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
			Quaternion* var=ParseQuatArg(script,args[0]);
			Quaternion result=*var;
			result/=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		default:return Quaternion();
	}
}
Matrix XPINSBuiltIn::ParseMatExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument> args){
	switch (op)
	{
		case ADD:
		{
			Matrix* var=ParseMatArg(script,args[0]);
			Matrix result=*var;
			result+=*ParseMatArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Matrix* var=ParseMatArg(script,args[0]);
			Matrix result=*var;
			result-=*ParseMatArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER,t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==MATRIX&&(t2==MATRIX||t2==VECTOR))//Matrix-Matrix
			{
				Matrix* var=(Matrix*)arg1;
				Matrix result=*var;
				Matrix a=(t2==MATRIX)?(*(Matrix*)arg2):(Matrix::MatrixForVector(*(Vector*)arg2));
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==MATRIX&&t2==NUMBER)//Matrix Scale
			{
				Matrix* var=(Matrix*)arg1;
				Matrix result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==MATRIX)//Matrix Scale
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
			Matrix* var=ParseMatArg(script,args[0]);
			Matrix result=*var;
			result/=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		default:return Matrix();
	}
}
Polynomial XPINSBuiltIn::ParsePolyExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument> args){
	switch (op)
	{
		case ADD:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			Polynomial result=*var;
			result+=*ParsePolyArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			Polynomial result=*var;
			result-=*ParsePolyArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER;
			DataType t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==POLYNOMIAL&&t2==POLYNOMIAL)//Polynomial-Polynomial
			{
				Polynomial* var=(Polynomial*)arg1;
				Polynomial result=*var;
				Polynomial a=*(Polynomial*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==POLYNOMIAL&&t2==NUMBER)//Polynomial-Scalar
			{
				Polynomial* var=(Polynomial*)arg1;
				Polynomial result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==POLYNOMIAL)//Polynomial-Scalar
			{
				Polynomial* var=(Polynomial*)arg2;
				Polynomial result=*var;
				double a=*(double*)arg1;
				result*=a;
				return result;
			}
			else if((t1==FIELD||t1==VECTOR)&&(t2==FIELD||t2==VECTOR))//Vector Field Dot Product
			{
				if(t1==VECTOR)arg1=new VectorField(*(Vector*)arg1);
				if(t2==VECTOR)arg2=new VectorField(*(Vector*)arg2);
				return VectorField::Dot(*(VectorField*)arg1, *(VectorField*)arg2);
			}
			else return Polynomial();
		}
		case DIVIDE:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			Polynomial result=*var;
			result/=*ParseNumArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			Polynomial result=Polynomial::Power(*var, *ParseNumArg(script,args[1]));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			Polynomial inner=*ParsePolyArg(script,args[1]);
			int variable=*ParseNumArg(script, args[2]);
			Polynomial result=Polynomial::Composition(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			Polynomial* var=ParsePolyArg(script,args[0]);
			int value=*ParseNumArg(script,args[1]);
			int variable=*ParseNumArg(script, args[1]);
			Polynomial result=var->PartialEvaluate(value,variable);
			if(assign) *var=result;
			return result;
		}
		default:return Polynomial();
	}
}
VectorField XPINSBuiltIn::ParseFieldExp(opCode op,bool assign,XPINSScriptSpace& script,vector<Argument>args){
	switch (op)
	{
		case ADD:
		{
			VectorField* var=ParseFieldArg(script,args[0]);
			VectorField result=*var;
			result+=*ParseFieldArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			VectorField* var=ParseFieldArg(script,args[0]);
			VectorField result=*var;
			result-=*ParseVecArg(script,args[1]);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=NUMBER;
			DataType t2=NUMBER;
			void* arg1=ParseArg(script, args[0], t1);
			void* arg2=ParseArg(script, args[1], t2);
			if(t1==MATRIX&&t2==FIELD)//Matrix-VectorField
			{
				Matrix mat=*(Matrix*)arg1;
				VectorField* var=(VectorField*)arg2;
				VectorField result=VectorField::MatMult(mat,*var);
				if(assign) *var=result;
				return result;
			}
			else if((t1==FIELD||t1==VECTOR)&&(t2==FIELD||t2==VECTOR))//Cross Product
			{
				VectorField result=(t1==FIELD)?*(VectorField*)arg1:VectorField(*(Vector*)arg1);
				VectorField v2=(t2==FIELD)?*(VectorField*)arg2:VectorField(*(Vector*)arg2);
				result=VectorField::Cross(result,v2);
				if(t1==FIELD&&assign) *(VectorField*)arg1=result;
				return result;
			}
			else if(t1==FIELD&&t2==POLYNOMIAL)//VectorField Scale
			{
				VectorField* var=(VectorField*)arg1;
				VectorField result=*var;
				Polynomial a=*(Polynomial*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==POLYNOMIAL&&t2==FIELD)//VectorField Scale
			{
				VectorField* var=(VectorField*)arg2;
				VectorField result=*var;
				Polynomial a=*(Polynomial*)arg1;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==FIELD&&t2==NUMBER)//VectorField Scale
			{
				VectorField* var=(VectorField*)arg1;
				VectorField result=*var;
				double a=*(double*)arg2;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==FIELD)//VectorField Scale
			{
				VectorField* var=(VectorField*)arg2;
				VectorField result=*var;
				double a=*(double*)arg1;
				result*=a;
				if(assign) *var=result;
				return result;
			}
			else return VectorField();
		}
		case DIVIDE:
		{
			VectorField* var=ParseFieldArg(script,args[0]);
			VectorField result=*var;
			result*= 1/(*ParseNumArg(script,args[1]));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			VectorField* var=ParseFieldArg(script,args[0]);
			Polynomial inner=*ParsePolyArg(script,args[1]);
			int variable=*ParseNumArg(script, args[2]);
			VectorField result=VectorField::Compose(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		case EVALUATE:
		{
			VectorField* var=ParseFieldArg(script,args[0]);
			int value=*ParseNumArg(script,args[1]);
			int variable=*ParseNumArg(script, args[2]);
			VectorField result=var->PartialEvaluate(value,variable);
			if(assign) *var=result;
			return result;
		}
		default:return VectorField();
	}
}
#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	switch (fNum)
	{
		case X_MARKOV_REACHABLE:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::Reachable(arg1, arg2, arg3);
		}
		default: return false;
	}
}

double XPINSBuiltIn::ParseNumBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	switch (fNum)
	{
		case X_SIN:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return sin(arg1);
		}
		case X_COS:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return cos(arg1);
		}
		case X_TAN:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return tan(arg1);
		}
		case X_CSC:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return 1/sin(arg1);
		}
		case X_SEC:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return 1/cos(arg1);
		}
		case X_COT:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return 1/tan(arg1);
		}
		case X_ASIN:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return asin(arg1);
		}
		case X_ACOS:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return acos(arg1);
		}
		case X_ATAN:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return atan(arg1);
		}
		case X_ACSC:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return asin(1/arg1);
		}
		case X_ASEC:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return acos(1/arg1);
		}
		case X_ACOT:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return atan(1/arg1);
		}
		case X_ATAN2:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return atan2(arg1, arg2);
		}
		case X_ADDPOLAR:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return atan2(arg1, arg2);
		}
		case X_SQRT:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return sqrt(arg1);
		}
		case X_ABS:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return fabs(arg1);
		}
		case X_FLOOR:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return floor(arg1);
		}
		case X_LN:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return log(arg1);
		}
		case X_LOG:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return log(arg1)/log(arg2);
		}
		case X_DIST:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Vector::Dist(arg1, arg2,arg3);
		}
		case X_X:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double x=0;
			arg1.Coords(&x, NULL,NULL,Vector::Cartesian);
			return x;
		}
		case X_Y:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double y=0;
			arg1.Coords(NULL, &y, NULL,Vector::Cartesian);
			return y;
		}
		case X_Z:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Cartesian);
			return z;
		}
		case X_R:
		{
			DataType t1=VECTOR;
			void* arg1=ParseArg(script, args[0], t1);
			if (t1==QUATERNION) {
				double r;
				((Quaternion*)arg1)->Components(NULL,NULL,NULL,&r);
				return r;
			} else if (t1==VECTOR) {
				double r=0;
				((Vector*)arg1)->Coords(&r, NULL, NULL,Vector::Polar);
				return r;
			} else return 0;
			
		}
		case X_THETA:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double t=0;
			arg1.Coords(NULL,&t, NULL,Vector::Polar);
			return t;
		}
		case X_MAGNITUDE:
		{
			DataType t1=VECTOR;
			void* arg1=ParseArg(script, args[0], t1);
			if (t1==QUATERNION) {
				return ((Quaternion*)arg1)->Magnitude();
			} else if (t1==VECTOR) {
				return ((Vector*)arg1)->Magnitude();
			} else return 0;
		}
		case X_PHI:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Spherical);
			return z;
		}
		case X_ANGLE_BETWEEN_VECTORS:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Vector::AngleBetweenVectors(arg1, arg2);
		}
		case X_DETERMINANT:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Determinant(arg1);
		}
		case X_RAND:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double rv= Probability::UniformRV(arg1, arg2-arg1);
			return rv;
		}
		case X_RV_BERNOULLI:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::BernoulliRV(arg1);
		}
		case X_RV_NORMAL:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::NormalRV(arg1, arg2);
		}
		case X_RV_EXP:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::ExponentialRV(arg1);
		}
		case X_RV_POISSON:
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::PoissonRV(arg1);
		}
		case X_COIN_FLIP:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::CoinFlip(arg1, arg2);
		}
		case X_DICE_ROLL:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::FairDiceRoll(arg1, arg2);
		}
		case X_MARKOV_SIM:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::SimulateMarkovChain(arg1, arg2, arg3);
		}
		case X_MARKOV_PROB:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			double arg4=*ParseNumArg(script, args[3]);
			return Probability::TransitionProbability(arg1, arg2, arg3, arg4);
		}
		case X_MARKOV_STEADYSTATE:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[0]);
			return Probability::SteadyStateProbability(arg1, arg2);

		}
		case X_MARKOV_ABSORB_PROB:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::AbsorbtionProbability(arg1, arg2, arg3);

		}
		case X_MARKOV_ABSORB_TIME:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::AbsorbtionTime(arg1, arg2, arg3);
		}
		case X_MARKOV_ABSORB_SIM:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::SimulateAbsorbtionTime(arg1, arg2, arg3);
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case X_PROJECT_ONTO_VECTOR:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Vector::ProjectionOntoVector(arg1,arg2);
		}
		case X_UNIT_VECTOR:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			return Vector::UnitVectorFromVector(arg1);
		}
		case X_V:
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			double x,y,z;
			arg1.Components(&x, &y, &z, NULL);
			return Vector(x,y,z,Vector::Cartesian);
		}
		case X_ROTATE_VECTOR:
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			Quaternion arg2=*ParseQuatArg(script, args[1]);
			return Quaternion::RotateVector(arg2, arg1);
		}
	}
	return Vector();
}
XPINSScriptableMath::Quaternion XPINSBuiltIn::ParseQuatBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case X_CONJUGATE:
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Quaternion::ConjugateQuaternion(arg1);
		}
		case X_INVERSE:
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Quaternion::InvertQuaternion(arg1);
		}
		case X_UNIT_QUATERNION:
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Quaternion::UnitQuaternion(arg1);
		}
	}
	return Quaternion();
}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case X_ZERO_MATRIX:
		{
			int arg1=*ParseNumArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Matrix(arg1,arg2);
		}
		case X_IDENTITY_MATRIX:
		{
			int arg1=*ParseNumArg(script, args[0]);
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case X_ROTATION_MATRIX:
		{
			double arg1=*ParseNumArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case X_EULER_ANGLE_MATRIX:
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Matrix::RotationMatrixWithEulerAngles(arg1, arg2, arg3);
		}
		case X_QUATERNION_MATRIX:
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Matrix::RotationMatrixWithQuaternion(arg1);
		}
		case X_INVERT:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Invert(arg1);
		}
		case X_TRANSPOSE:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Transpose(arg1);
		}
		case X_APPEND:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			Matrix arg2=*ParseMatArg(script, args[1]);
			return Matrix::Append(arg1, arg2);
		}
		case X_ROW_ECHELON:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::RowEchelon(arg1);
		}
		case X_REDUCED_ROW_ECHELON:
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::ReducedRowEchelon(arg1);
		}
	}
	return Matrix();
}
XPINSScriptableMath::Polynomial XPINSBuiltIn::ParsePolyBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case X_DERIVE:
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Polynomial::Derivative(arg1, arg2);
		}
		case X_INTEGRATE:
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Polynomial::Integrate(arg1, arg2);
		}
		case X_DIVERGENCE:
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			return arg1.Divergence();
		}
		case X_LINE_INTEGRAL:
		{
			DataType t1=POLYNOMIAL;
			void* arg1=ParseArg(script, args[0], t1);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			if (t1==POLYNOMIAL) {
				return VectorField::LineIntegral(*((Polynomial*)arg1), arg2, Bound(arg3,arg4));
			} else if (t1==FIELD) {
				return VectorField::LineIntegral(*((VectorField*)arg1), arg2, Bound(arg3,arg4));
			} else return Polynomial();
		}
		case X_SURFACE_INTEGRAL:
		{
			DataType t1=POLYNOMIAL;
			void* arg1=ParseArg(script, args[0], t1);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			Polynomial arg5=*ParsePolyArg(script, args[4]);
			Polynomial arg6=*ParsePolyArg(script, args[5]);
			if (t1==POLYNOMIAL) {
				return VectorField::SurfaceIntegral(*((Polynomial*)arg1), arg2, Bound(arg3,arg4),Bound(arg5,arg6));
			} else if (t1==FIELD) {
				return VectorField::SurfaceIntegral(*((VectorField*)arg1), arg2, Bound(arg3,arg4),Bound(arg5,arg6));
			} else return Polynomial();
		
		}
		case X_VOLUME_INTEGRAL:
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			Polynomial arg5=*ParsePolyArg(script, args[4]);
			Polynomial arg6=*ParsePolyArg(script, args[5]);
			Polynomial arg7=*ParsePolyArg(script, args[6]);
			Polynomial arg8=*ParsePolyArg(script, args[7]);
			return VectorField::VolumeIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6),Bound(arg7,arg8));
		}
	}
	return Polynomial();
}
XPINSScriptableMath::VectorField XPINSBuiltIn::ParseFieldBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case X_GRADIENT_VECTOR:
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			return VectorField::GradientField(arg1);
		}
		case X_COMPONENT_DERIVE:
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			double  arg2=*ParseNumArg(script, args[1]);
			return VectorField::Derivative(arg1, arg2);
		}
		case X_COMPONENT_INTEGRATE:
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			double  arg2=*ParseNumArg(script, args[1]);
			return VectorField::Integrate(arg1, arg2);
		}
		case X_CURL:
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			return arg1.Curl();
		}
	}
	return VectorField();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSScriptSpace& script,vector<Argument>args)
{
	switch (fNum) {
		case X_PRINT:
		{
			string str=*XPINSParser::ParseStrArg(script, args[0]);
			cout<<str;
		}break;
	}
}