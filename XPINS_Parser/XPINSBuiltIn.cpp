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
		case MODULUS:
		{
			double* var=ParseNumArg(script,args[0]);
			int result=*var;
			result %= (int)*ParseNumArg(script,args[1]);
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
		case 1://X_MARKOV_REACHABLE
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
		case 1://X_SIN
		{
			double arg1=*ParseNumArg(script, args[0]);
			return sin(arg1);
		}
		case 2://X_COS
		{
			double arg1=*ParseNumArg(script, args[0]);
			return cos(arg1);
		}
		case 3://X_TAN
		{
			double arg1=*ParseNumArg(script, args[0]);
			return tan(arg1);
		}
		case 4://X_ASIN
		{
			double arg1=*ParseNumArg(script, args[0]);
			return asin(arg1);
		}
		case 5://X_ACOS
		{
			double arg1=*ParseNumArg(script, args[0]);
			return acos(arg1);
		}
		case 6://X_ATAN
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return atan2(arg1, arg2);
		}
		case 7://X_SQRT
		{
			double arg1=*ParseNumArg(script, args[0]);
			return sqrt(arg1);
		}
		case 8://X_LN
		{
			double arg1=*ParseNumArg(script, args[0]);
			return log(arg1);
		}
		case 9://X_LOG
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return log(arg1)/log(arg2);
		}
		case 10://X_ABS
		{
			double arg1=*ParseNumArg(script, args[0]);
			return fabs(arg1);
		}
		case 11://X_FLOOR
		{
			double arg1=*ParseNumArg(script, args[0]);
			return floor(arg1);
		}
		case 12://X_ADDPOLAR
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Vector::AddPolar(arg1, arg2);
		}
		case 13://X_DIST
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Vector::Dist(arg1, arg2,arg3);
		}
		case 14://X_VX
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double x=0;
			arg1.Coords(&x, NULL,NULL,Vector::Cartesian);
			return x;
		}
		case 15://X_VY
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double y=0;
			arg1.Coords(NULL, &y, NULL,Vector::Cartesian);
			return y;
		}
		case 16://X_VZ
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Cartesian);
			return z;
		}
		case 17://X_VR
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double r=0;
			arg1.Coords(&r, NULL, NULL,Vector::Polar);
			return r;
		}
		case 18://X_VTHETA
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double t=0;
			arg1.Coords(NULL,&t, NULL,Vector::Polar);
			return t;
		}
		case 19://X_VMAG
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			return arg1.Magnitude();
		}
		case 20://X_VPHI
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Spherical);
			return z;
		}
		case 21://X_VECTOR_ANGLE
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Vector::AngleBetweenVectors(arg1, arg2);
			
		}
		case 22://X_MATRIX_GET
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			int arg3=*ParseNumArg(script, args[2]);
			return arg1.ValueAtPosition(arg2, arg3);
		}
		case 23://X_DETERMINANT
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Determinant(arg1);
		}
		case 24://X_RAND
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::UniformRV(arg1, arg2-arg1);
		}
		case 25://X_RV_BERNOULLI
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::BernoulliRV(arg1);
		}
		case 26://X_RV_NOMRAL
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::NormalRV(arg1, arg2);
		}
		case 27://X_RV_EXP
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::ExponentialRV(arg1);
		}
		case 28://X_RV_POISSON
		{
			double arg1=*ParseNumArg(script, args[0]);
			return Probability::PoissonRV(arg1);
		}
		case 29://X_COIN_FLIP
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::CoinFlip(arg1, arg2);
		}
		case 30://X_DICE_ROLL
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			return Probability::FairDiceRoll(arg1, arg2);
		}
		case 31://X_MARKOV_SIM
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::SimulateMarkovChain(arg1, arg2, arg3);
		}
		case 32://X_MARKOV_PROB
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			double arg4=*ParseNumArg(script, args[3]);
			return Probability::TransitionProbability(arg1, arg2, arg3, arg4);
		}
		case 33://X_MARKOV_STEADYSTATE
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[0]);
			return Probability::SteadyStateProbability(arg1, arg2);

		}
		case 34://X_MARKOV_ABSORB_PROB
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::AbsorbtionProbability(arg1, arg2, arg3);

		}
		case 35://X_MARKOV_ABSORB_TIME
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::AbsorbtionTime(arg1, arg2, arg3);
		}
		case 36://X_MARKOV_ABSORB_SIM
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Probability::SimulateAbsorbtionTime(arg1, arg2, arg3);
		}
		case 37://X_QR
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			double r;
			arg1.Components(NULL,NULL,NULL,&r);
			return r;
		}
		case 38://X_QMAG
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return arg1.Magnitude();
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case 1://X_VPROJECT
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Vector::ProjectionOntoVector(arg1,arg2);
		}
		case 2://X_UNIT_VECTOR
		{
			Vector arg1=*ParseVecArg(script, args[0]);
			return Vector::UnitVectorFromVector(arg1);
		}
		case 3://X_QV
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			double x,y,z;
			arg1.Components(&x, &y, &z, NULL);
			return Vector(x,y,z,Vector::Cartesian);
		}
		case 4://X_QUATERNION_ROTATE
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Quaternion::RotateVector(arg1, arg2);
		}
	}
	return Vector();
}
XPINSScriptableMath::Quaternion XPINSBuiltIn::ParseQuatBIF(int fNum, XPINSScriptSpace& script, vector<Argument> args)
{
	
	switch (fNum)
	{
		case 1://X_QUATERNION_CONJUGATE
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Quaternion::ConjugateQuaternion(arg1);
		}
		case 2://X_QUATERNION_INVERSE
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Quaternion::InvertQuaternion(arg1);
		}
		case 3://X_UNIT_QUATERNION
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
		case 1://X_ZERO_MATRIX
		{
			int arg1=*ParseNumArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Matrix(arg1,arg2);
		}
		case 2://X_IDENTITY_MATRIX
		{
			int arg1=*ParseNumArg(script, args[0]);
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case 3://X_ROTATION_MATRIX
		{
			double arg1=*ParseNumArg(script, args[0]);
			Vector arg2=*ParseVecArg(script, args[1]);
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case 4://X_EULER_ANGLE_MATRIX
		{
			double arg1=*ParseNumArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			double arg3=*ParseNumArg(script, args[2]);
			return Matrix::RotationMatrixWithEulerAngles(arg1, arg2, arg3);
		}
		case 5://X_QUATERNION_MATRIX
		{
			Quaternion arg1=*ParseQuatArg(script, args[0]);
			return Matrix::RotationMatrixWithQuaternion(arg1);
		}
		case 6://X_INVERT
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Invert(arg1);
		}
		case 7://X_TRANSPOSE
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::Transpose(arg1);
		}
		case 8://X_APPEND
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			Matrix arg2=*ParseMatArg(script, args[1]);
			return Matrix::Append(arg1, arg2);
		}
		case 9://X_ROW_ECHELON
		{
			Matrix arg1=*ParseMatArg(script, args[0]);
			return Matrix::RowEchelon(arg1);
		}
		case 10://X_REDUCED_ROW_ECHELON
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
		case 1://X_DERIVE
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Polynomial::Derivative(arg1, arg2);
		}
		case 2://X_INTEGRATE
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			int arg2=*ParseNumArg(script, args[1]);
			return Polynomial::Integrate(arg1, arg2);
		}
		case 3://X_DIVERGENCE
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			return arg1.Divergence();
		}
		case 4://X_SCALAR_LINE_INTEGRAL
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			Bound::Bound(arg3,arg4);
			return VectorField::LineIntegral(arg1, arg2, Bound(arg3,arg4));
		}
		case 5://X_VECTOR_LINE_INTEGRAL
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			return VectorField::LineIntegral(arg1, arg2, Bound(arg3,arg4));
		}
		case 6://X_SCALAR_SURFACE_INTEGRAL
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			Polynomial arg5=*ParsePolyArg(script, args[4]);
			Polynomial arg6=*ParsePolyArg(script, args[5]);
			return VectorField::SurfaceIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6));
		}
		case 7://X_VECTOR_LINE_INTEGRAL
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			VectorField arg2=*ParseFieldArg(script, args[1]);
			Polynomial arg3=*ParsePolyArg(script, args[2]);
			Polynomial arg4=*ParsePolyArg(script, args[3]);
			Polynomial arg5=*ParsePolyArg(script, args[4]);
			Polynomial arg6=*ParsePolyArg(script, args[5]);
			return VectorField::SurfaceIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6));
		}
		case 8://X_VOLUME_INTEGRAL
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
		case 1://X_GRADIENT_VECTOR
		{
			Polynomial arg1=*ParsePolyArg(script, args[0]);
			return VectorField::GradientField(arg1);
		}
		case 2://X_VECTOR_DERIVE
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			double  arg2=*ParseNumArg(script, args[1]);
			return VectorField::Derivative(arg1, arg2);
		}
		case 3://X_VECTOR_INTEGRATE
		{
			VectorField arg1=*ParseFieldArg(script, args[0]);
			double  arg2=*ParseNumArg(script, args[1]);
			return VectorField::Integrate(arg1, arg2);
		}
		case 4://X_CURL
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
		case 1://X_PRINT
		{
			string str=*XPINSParser::ParseStrArg(script, args[0]);
			cout<<str;
		}break;
		case 2://X_MSET
		{
			Matrix* arg1=ParseMatArg(script, args[0]);
			double arg2=*ParseNumArg(script, args[1]);
			int arg3=*ParseNumArg(script, args[2]);
			int arg4=*ParseNumArg(script, args[3]);
			arg1->SetValueAtPosition(arg2, arg3, arg4);
		}break;
	}
}