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

bool XPINSBuiltIn::ParseBoolExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case NOT:
		{
			bool* arg=args.boolAtIndex(0);
			bool result=!*arg;
			if(assign)*arg=result;
			return result;
		}
		case OR:
		{
			bool* arg0=args.boolAtIndex(0);
			bool* arg1=args.boolAtIndex(1);
			bool result=*arg0||*arg1;
			if(assign)*arg0=result;
			return result;
		}
		case AND:
		{
			bool* arg0=args.boolAtIndex(0);
			bool* arg1=args.boolAtIndex(1);
			bool result=*arg0&&*arg1;
			if(assign)*arg0=result;
			return result;
		}
		case LESS:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0<arg1;
		}
		case LESSEQ:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0<=arg1;
		}
		case GREATER:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0>arg1;
		}
		case GREATEREQ:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0>=arg1;
		}
		case NOTEQUAL:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0!=arg1;
		}
		case EQAUAL:
		{
			double arg0=*args.numAtIndex(0);
			double arg1=*args.numAtIndex(1);
			return arg0==arg1;
		}
		default:return false;
	}
}
double XPINSBuiltIn::ParseNumExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			double* var = args.numAtIndex(0);
			double result = *var + *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			double* var = args.numAtIndex(0);
			double result = *var - *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==VECTOR&&t2==VECTOR)//Dot Product
			{
				return Vector::DotProduct(*args.vecAtIndex(0), *args.vecAtIndex(1));
			}
			else if(t1==NUMBER&&t2==NUMBER)
			{
				double* var = args.numAtIndex(0);
				double result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else return 0;
		}
		case DIVIDE:
		{
			double* var = args.numAtIndex(0);
			double result = *var / *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			double* var = args.numAtIndex(0);
			double result = pow( *var , *args.numAtIndex(1));
			if(assign) *var=result;
			return result;
		}
		case REMAINDER:
		{
			double* var = args.numAtIndex(0);
			double num2 = *args.numAtIndex(1);
			double result=*var-num2*floor(*var / num2);
			if(assign) *var=result;
			return result;
		}
		case PREINCREMENT:
			return ++(*args.numAtIndex(0));
		case POSTINCREMENT:
			return (*args.numAtIndex(0))++;
		case PREDECREMENT:
			return --(*args.numAtIndex(0));
		case POSTDECREMENT:
			return (*args.numAtIndex(0))--;
		default:return 0.0;
	}
}
Vector XPINSBuiltIn::ParseVecExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			Vector* var=args.vecAtIndex(0);
			Vector result = *var + *args.vecAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Vector* var=args.vecAtIndex(0);
			Vector result = *var - *args.vecAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==MATRIX&&t2==VECTOR)//Dot Product
			{
				Matrix mat=*args.matAtIndex(0);
				Vector* var=args.vecAtIndex(1);
				Vector result=mat*(*var);
				if(assign) *var=result;
				return result;
			}
			else if(t1==VECTOR&&t2==VECTOR)
			{
				Vector* var = args.vecAtIndex(0);
				Vector result = Vector::CrossProduct(*var , *args.vecAtIndex(1));
				if(assign) *var=result;
				return result;
			}
			else if(t1==VECTOR&&t2==NUMBER)
			{
				Vector* var=args.vecAtIndex(0);
				Vector result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==VECTOR)
			{
				Vector* var=args.vecAtIndex(1);
				Vector result = *var * *args.numAtIndex(0);
				if(assign) *var=result;
				return result;
			}
			else return Vector();
		}
		case DIVIDE:
		{
			Vector* var=args.vecAtIndex(0);
			Vector result = *var / *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		default:return Vector();
	}
}
Quaternion XPINSBuiltIn::ParseQuatExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			Quaternion* var=args.quatAtIndex(0);
			Quaternion result = *var + *args.quatAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Quaternion* var=args.quatAtIndex(0);
			Quaternion result = *var - *args.quatAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==QUATERNION&&t2==QUATERNION)
			{
				Quaternion* var = args.quatAtIndex(0);
				Quaternion result = *var * *args.quatAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==QUATERNION&&t2==NUMBER)
			{
				Quaternion* var=args.quatAtIndex(0);
				Quaternion result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==QUATERNION)
			{
				Quaternion* var=args.quatAtIndex(1);
				Quaternion result = *var * *args.numAtIndex(0);
				if(assign) *var=result;
				return result;
			}
			else return Quaternion();
		}
		case DIVIDE:
		{
			Quaternion* var=args.quatAtIndex(0);
			Quaternion result = *var / *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		default:return Quaternion();
	}
}
Matrix XPINSBuiltIn::ParseMatExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			Matrix* var=args.matAtIndex(0);
			Matrix result = *var + *args.matAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Matrix* var=args.matAtIndex(0);
			Matrix result = *var - *args.matAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==MATRIX&&(t2==MATRIX||t2==VECTOR))//Matrix-Matrix
			{
				Matrix* var=args.matAtIndex(0);
				Matrix arg2= t2==MATRIX ? *args.matAtIndex(1) : Matrix::MatrixForVector(*args.vecAtIndex(1));
				Matrix result = *var * arg2;
				if(assign) *var=result;
				return result;
			}
			else if(t1==MATRIX&&t2==NUMBER)//Matrix Scale
			{
				Matrix* var=args.matAtIndex(0);
				Matrix result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==MATRIX)//Matrix Scale
				return *args.matAtIndex(1) * *args.numAtIndex(0);
			else return Matrix();
		}
		case DIVIDE:
		{
			Matrix* var=args.matAtIndex(0);
			Matrix result=*var / *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			Matrix* var=args.matAtIndex(0);
			Matrix result=Matrix::Power(*var, *args.numAtIndex(1));
			if(assign) *var=result;
			return result;
		}

		default:return Matrix();
	}
}
Polynomial XPINSBuiltIn::ParsePolyExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			Polynomial* var=args.polyAtIndex(0);
			Polynomial result=*var + *args.polyAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			Polynomial* var=args.polyAtIndex(0);
			Polynomial result=*var - *args.polyAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==POLYNOMIAL&&t2==POLYNOMIAL)//Polynomial-Polynomial
			{
				Polynomial* var=args.polyAtIndex(0);
				Polynomial result = *var * *args.polyAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==POLYNOMIAL&&t2==NUMBER)//Polynomial-Scalar
			{
				Polynomial* var=args.polyAtIndex(0);
				Polynomial result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&t2==POLYNOMIAL)//Polynomial-Scalar
				return *args.polyAtIndex(1) * *args.numAtIndex(0);
			else if((t1==FIELD||t1==VECTOR)&&(t2==FIELD||t2==VECTOR))//Vector Field Dot Product
			{
				VectorField arg1 = t1==FIELD ? *args.fieldAtIndex(0) : VectorField(*args.vecAtIndex(0));
				VectorField arg2 = t2==FIELD ? *args.fieldAtIndex(1) : VectorField(*args.vecAtIndex(1));
				return VectorField::DotProduct(arg1, arg2);
			}
			else return Polynomial();
		}
		case DIVIDE:
		{
			Polynomial* var=args.polyAtIndex(0);
			Polynomial result = *var / *args.numAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case POWER:
		{
			Polynomial* var=args.polyAtIndex(0);
			Polynomial result=Polynomial::Power(*var, *args.numAtIndex(1));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			Polynomial* var=args.polyAtIndex(0);
			Polynomial inner=*args.polyAtIndex(1);
			int variable=*args.numAtIndex(2);
			Polynomial result=Polynomial::Composition(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		default:return Polynomial();
	}
}
VectorField XPINSBuiltIn::ParseFieldExp(opCode op,bool assign,XPINSArray args){
	switch (op)
	{
		case ADD:
		{
			VectorField* var=args.fieldAtIndex(0);
			VectorField result=*var + *args.fieldAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case SUBTRACT:
		{
			VectorField* var=args.fieldAtIndex(0);
			VectorField result=*var - *args.fieldAtIndex(1);
			if(assign) *var=result;
			return result;
		}
		case MULTIPLY:
		{
			DataType t1=args.typeAtIndex(0);
			DataType t2=args.typeAtIndex(1);
			if(t1==MATRIX&&(t2==FIELD))
			{
				Matrix mat=*args.matAtIndex(0);
				VectorField* var=args.fieldAtIndex(1);
				VectorField result=VectorField::MatMult(mat, *var);
				if(assign) *var=result;
				return result;
			}
			else if((t1==FIELD||t1==VECTOR)&&(t2==FIELD||t2==VECTOR))//Vector Field Dot Product
			{
				VectorField *var=args.fieldAtIndex(0);
				VectorField result = VectorField::CrossProduct(*var , *args.fieldAtIndex(1));
				if(assign) *var=result;
				return result;
			}
			else if((t1==FIELD||t1==VECTOR)&&t2==NUMBER)
			{
				VectorField* var=args.fieldAtIndex(0);
				VectorField result = *var * *args.numAtIndex(1);
				if(assign) *var=result;
				return result;
			}
			else if(t1==NUMBER&&(t2==FIELD||t2==VECTOR))
			{
				return *args.fieldAtIndex(1) * *args.numAtIndex(0);
			}
			else return VectorField();
		}
		case DIVIDE:
		{
			VectorField* var=args.fieldAtIndex(0);
			VectorField result = *var / (*args.numAtIndex(1));
			if(assign) *var=result;
			return result;
		}
		case COMPOSITION:
		{
			VectorField* var=args.fieldAtIndex(0);
			Polynomial inner=*args.polyAtIndex(1);
			int variable=*args.numAtIndex(2);
			VectorField result=VectorField::Compose(*var,inner,variable);
			if(assign) *var=result;
			return result;
		}
		default:return VectorField();
	}
}
#pragma mark BIF processing

bool XPINSBuiltIn::ParseBoolBIF(int fNum, XPINSArray args)
{
	switch (fNum)
	{
		case X_MARKOV_REACHABLE:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Probability::Reachable(arg1, arg2, arg3);
		}
		default: return false;
	}
}

double XPINSBuiltIn::ParseNumBIF(int fNum, XPINSArray args)
{
	switch (fNum)
	{
		case X_SIN:
		{
			double arg1=*args.numAtIndex(0);
			return sin(arg1);
		}
		case X_COS:
		{
			double arg1=*args.numAtIndex(0);
			return cos(arg1);
		}
		case X_TAN:
		{
			double arg1=*args.numAtIndex(0);
			return tan(arg1);
		}
		case X_CSC:
		{
			double arg1=*args.numAtIndex(0);
			return 1/sin(arg1);
		}
		case X_SEC:
		{
			double arg1=*args.numAtIndex(0);
			return 1/cos(arg1);
		}
		case X_COT:
		{
			double arg1=*args.numAtIndex(0);
			return 1/tan(arg1);
		}
		case X_ASIN:
		{
			double arg1=*args.numAtIndex(0);
			return asin(arg1);
		}
		case X_ACOS:
		{
			double arg1=*args.numAtIndex(0);
			return acos(arg1);
		}
		case X_ATAN:
		{
			double arg1=*args.numAtIndex(0);
			return atan(arg1);
		}
		case X_ACSC:
		{
			double arg1=*args.numAtIndex(0);
			return asin(1/arg1);
		}
		case X_ASEC:
		{
			double arg1=*args.numAtIndex(0);
			return acos(1/arg1);
		}
		case X_ACOT:
		{
			double arg1=*args.numAtIndex(0);
			return atan(1/arg1);
		}
		case X_ATAN2:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return atan2(arg1, arg2);
		}
		case X_ADDPOLAR:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return atan2(arg1, arg2);
		}
		case X_SQRT:
		{
			double arg1=*args.numAtIndex(0);
			return sqrt(arg1);
		}
		case X_ABS:
		{
			double arg1=*args.numAtIndex(0);
			return fabs(arg1);
		}
		case X_FLOOR:
		{
			double arg1=*args.numAtIndex(0);
			return floor(arg1);
		}
		case X_LN:
		{
			double arg1=*args.numAtIndex(0);
			return log(arg1);
		}
		case X_LOG:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return log(arg1)/log(arg2);
		}
		case X_DIST:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Vector::Dist(arg1, arg2,arg3);
		}
		case X_X:
		{
			Vector arg1=*args.vecAtIndex(0);
			double x=0;
			arg1.Coords(&x, NULL,NULL,Vector::Cartesian);
			return x;
		}
		case X_Y:
		{
			Vector arg1=*args.vecAtIndex(0);
			double y=0;
			arg1.Coords(NULL, &y, NULL,Vector::Cartesian);
			return y;
		}
		case X_Z:
		{
			Vector arg1=*args.vecAtIndex(0);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Cartesian);
			return z;
		}
		case X_R:
		{
			DataType t1=args.typeAtIndex(0);
			if (t1==QUATERNION) {
				double r;
				args.quatAtIndex(0)->Components(NULL,NULL,NULL,&r);
				return r;
			} else if (t1==VECTOR) {
				double r=0;
				args.vecAtIndex(0)->Coords(&r, NULL, NULL,Vector::Polar);
				return r;
			} else return 0;
			
		}
		case X_THETA:
		{
			Vector arg1=*args.vecAtIndex(0);
			double t=0;
			arg1.Coords(NULL,&t, NULL,Vector::Polar);
			return t;
		}
		case X_MAGNITUDE:
		{
			DataType t1=args.typeAtIndex(0);
			if (t1==QUATERNION) {
				return args.quatAtIndex(0)->Magnitude();
			} else if (t1==VECTOR) {
				return args.vecAtIndex(0)->Magnitude();
			} else return 0;
		}
		case X_PHI:
		{
			Vector arg1=*args.vecAtIndex(0);
			double z=0;
			arg1.Coords(NULL, NULL,&z,Vector::Spherical);
			return z;
		}
		case X_ANGLE_BETWEEN_VECTORS:
		{
			Vector arg1=*args.vecAtIndex(0);
			Vector arg2=*args.vecAtIndex(1);
			return Vector::AngleBetweenVectors(arg1, arg2);
		}
		case X_DETERMINANT:
		{
			Matrix arg1=*args.matAtIndex(0);
			return Matrix::Determinant(arg1);
		}
		case X_RAND:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double rv= Probability::UniformRV(arg1, arg2-arg1);
			return rv;
		}
		case X_RV_BERNOULLI:
		{
			double arg1=*args.numAtIndex(0);
			return Probability::BernoulliRV(arg1);
		}
		case X_RV_NORMAL:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return Probability::NormalRV(arg1, arg2);
		}
		case X_RV_EXP:
		{
			double arg1=*args.numAtIndex(0);
			return Probability::ExponentialRV(arg1);
		}
		case X_RV_POISSON:
		{
			double arg1=*args.numAtIndex(0);
			return Probability::PoissonRV(arg1);
		}
		case X_COIN_FLIP:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return Probability::CoinFlip(arg1, arg2);
		}
		case X_DICE_ROLL:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			return Probability::FairDiceRoll(arg1, arg2);
		}
		case X_MARKOV_SIM:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Probability::SimulateMarkovChain(arg1, arg2, arg3);
		}
		case X_MARKOV_PROB:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			double arg4=*args.numAtIndex(3);
			return Probability::TransitionProbability(arg1, arg2, arg3, arg4);
		}
		case X_MARKOV_STEADYSTATE:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(0);
			return Probability::SteadyStateProbability(arg1, arg2);

		}
		case X_MARKOV_ABSORB_PROB:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Probability::AbsorbtionProbability(arg1, arg2, arg3);

		}
		case X_MARKOV_ABSORB_TIME:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Probability::AbsorbtionTime(arg1, arg2, arg3);
		}
		case X_MARKOV_ABSORB_SIM:
		{
			Matrix arg1=*args.matAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Probability::SimulateAbsorbtionTime(arg1, arg2, arg3);
		}
		case X_SIZE:
		{
			XPINSArray arg1=*args.arrAtIndex(0);
			return arg1.size();
		}
		case X_EVALUATE:
		{
			Polynomial arg1=*args.polyAtIndex(0);
			vector<double> values=vector<double>(args.size()-1);
			for (int i=0; i<values.size(); ++i) {
				values[i]=*args.numAtIndex(i+1);
			}
			return arg1.Evaluate(values);
		}
	}
	return 0;
}
XPINSScriptableMath::Vector XPINSBuiltIn::ParseVecBIF(int fNum, XPINSArray  args)
{
	
	switch (fNum)
	{
		case X_PROJECT_ONTO_VECTOR:
		{
			Vector arg1=*args.vecAtIndex(0);
			Vector arg2=*args.vecAtIndex(1);
			return Vector::ProjectionOntoVector(arg1,arg2);
		}
		case X_UNIT_VECTOR:
		{
			Vector arg1=*args.vecAtIndex(0);
			return Vector::UnitVectorFromVector(arg1);
		}
		case X_V:
		{
			Quaternion arg1=*args.quatAtIndex(0);
			double x,y,z;
			arg1.Components(&x, &y, &z, NULL);
			return Vector(x,y,z,Vector::Cartesian);
		}
		case X_ROTATE_VECTOR:
		{
			Vector arg1=*args.vecAtIndex(0);
			Quaternion arg2=*args.quatAtIndex(1);
			return Quaternion::RotateVector(arg2, arg1);
		}
		case X_VECTOR_EVALUATE:
		{
			VectorField arg1=*args.fieldAtIndex(0);
			vector<double> values=vector<double>(args.size()-1);
			for (int i=0; i<values.size(); ++i) {
				values[i]=*args.numAtIndex(i+1);
			}
			return arg1.Evaluate(values);
		}

	}
	return Vector();
}
XPINSScriptableMath::Quaternion XPINSBuiltIn::ParseQuatBIF(int fNum, XPINSArray  args)
{
	
	switch (fNum)
	{
		case X_CONJUGATE:
		{
			Quaternion arg1=*args.quatAtIndex(0);
			return Quaternion::ConjugateQuaternion(arg1);
		}
		case X_INVERSE:
		{
			Quaternion arg1=*args.quatAtIndex(0);
			return Quaternion::InvertQuaternion(arg1);
		}
		case X_UNIT_QUATERNION:
		{
			Quaternion arg1=*args.quatAtIndex(0);
			return Quaternion::UnitQuaternion(arg1);
		}
	}
	return Quaternion();
}
XPINSScriptableMath::Matrix XPINSBuiltIn::ParseMatBIF(int fNum, XPINSArray args)
{
	
	switch (fNum)
	{
		case X_ZERO_MATRIX:
		{
			int arg1=*args.numAtIndex(0);
			int arg2=*args.numAtIndex(1);
			return Matrix(arg1,arg2);
		}
		case X_IDENTITY_MATRIX:
		{
			int arg1=*args.numAtIndex(0);
			return Matrix::IdentityMatrixOfSize(arg1);
		}
		case X_ROTATION_MATRIX:
		{
			double arg1=*args.numAtIndex(0);
			Vector arg2=*args.vecAtIndex(1);
			return Matrix::RotationMatrixWithAngleAroundVector(arg2,arg1);
		}
		case X_EULER_ANGLE_MATRIX:
		{
			double arg1=*args.numAtIndex(0);
			double arg2=*args.numAtIndex(1);
			double arg3=*args.numAtIndex(2);
			return Matrix::RotationMatrixWithEulerAngles(arg1, arg2, arg3);
		}
		case X_QUATERNION_MATRIX:
		{
			Quaternion arg1=*args.quatAtIndex(0);
			return Matrix::RotationMatrixWithQuaternion(arg1);
		}
		case X_INVERT:
		{
			Matrix arg1=*args.matAtIndex(0);
			return Matrix::Invert(arg1);
		}
		case X_TRANSPOSE:
		{
			Matrix arg1=*args.matAtIndex(0);
			return Matrix::Transpose(arg1);
		}
		case X_APPEND:
		{
			Matrix arg1=*args.matAtIndex(0);
			Matrix arg2=*args.matAtIndex(1);
			return Matrix::Append(arg1, arg2);
		}
		case X_ROW_ECHELON:
		{
			Matrix arg1=*args.matAtIndex(0);
			return Matrix::RowEchelon(arg1);
		}
		case X_REDUCED_ROW_ECHELON:
		{
			Matrix arg1=*args.matAtIndex(0);
			return Matrix::ReducedRowEchelon(arg1);
		}
	}
	return Matrix();
}
XPINSScriptableMath::Polynomial XPINSBuiltIn::ParsePolyBIF(int fNum, XPINSArray args)
{
	
	switch (fNum)
	{
		case X_DERIVE:
		{
			Polynomial arg1=*args.polyAtIndex(0);
			int arg2=*args.numAtIndex(1);
			return Polynomial::Derivative(arg1, arg2);
		}
		case X_INTEGRATE:
		{
			Polynomial arg1=*args.polyAtIndex(0);
			int arg2=*args.numAtIndex(1);
			return Polynomial::Integrate(arg1, arg2);
		}
		case X_DIVERGENCE:
		{
			VectorField arg1=*args.fieldAtIndex(0);
			return arg1.Divergence();
		}
		case X_LINE_INTEGRAL:
		{
			DataType t1=args.typeAtIndex(0);
			VectorField arg2=*args.fieldAtIndex(1);
			Polynomial arg3=*args.polyAtIndex(2);
			Polynomial arg4=*args.polyAtIndex(3);
			if (t1==POLYNOMIAL) {
				return VectorField::LineIntegral(*args.polyAtIndex(0), arg2, Bound(arg3,arg4));
			} else if (t1==FIELD) {
				return VectorField::LineIntegral(*args.fieldAtIndex(0), arg2, Bound(arg3,arg4));
			} else return Polynomial();
		}
		case X_SURFACE_INTEGRAL:
		{
			DataType t1=args.typeAtIndex(0);
			VectorField arg2=*args.fieldAtIndex(1);
			Polynomial arg3=*args.polyAtIndex(2);
			Polynomial arg4=*args.polyAtIndex(3);
			Polynomial arg5=*args.polyAtIndex(4);
			Polynomial arg6=*args.polyAtIndex(5);
			if (t1==POLYNOMIAL) {
				return VectorField::SurfaceIntegral(*args.polyAtIndex(0), arg2, Bound(arg3,arg4),Bound(arg5,arg6));
			} else if (t1==FIELD) {
				return VectorField::SurfaceIntegral(*args.fieldAtIndex(0), arg2, Bound(arg3,arg4),Bound(arg5,arg6));
			} else return Polynomial();
		
		}
		case X_VOLUME_INTEGRAL:
		{
			Polynomial arg1=*args.polyAtIndex(0);
			VectorField arg2=*args.fieldAtIndex(1);
			Polynomial arg3=*args.polyAtIndex(2);
			Polynomial arg4=*args.polyAtIndex(3);
			Polynomial arg5=*args.polyAtIndex(4);
			Polynomial arg6=*args.polyAtIndex(5);
			Polynomial arg7=*args.polyAtIndex(6);
			Polynomial arg8=*args.polyAtIndex(7);
			return VectorField::VolumeIntegral(arg1, arg2, Bound(arg3,arg4),Bound(arg5,arg6),Bound(arg7,arg8));
		}
	}
	return Polynomial();
}
XPINSScriptableMath::VectorField XPINSBuiltIn::ParseFieldBIF(int fNum, XPINSArray args)
{
	
	switch (fNum)
	{
		case X_GRADIENT_VECTOR:
		{
			Polynomial arg1=*args.polyAtIndex(0);
			return VectorField::GradientField(arg1);
		}
		case X_COMPONENT_DERIVE:
		{
			VectorField arg1=*args.fieldAtIndex(0);
			double  arg2=*args.numAtIndex(1);
			return VectorField::Derivative(arg1, arg2);
		}
		case X_COMPONENT_INTEGRATE:
		{
			VectorField arg1=*args.fieldAtIndex(0);
			double  arg2=*args.numAtIndex(1);
			return VectorField::Integrate(arg1, arg2);
		}
		case X_CURL:
		{
			VectorField arg1=*args.fieldAtIndex(0);
			return arg1.Curl();
		}
	}
	return VectorField();
}
void XPINSBuiltIn::ParseVoidBIF(int fNum, XPINSArray args)
{
	switch (fNum) {
		case X_PRINT:
		{
			string str=*args.strAtIndex(0);
			cout<<str;
		}break;
		case X_PRINTF:
		{
			string str=*args.strAtIndex(0);
			string temp="";
			int currentArg=1;
			for (int i=0; i<str.length(); ++i) {
				if(str[i]=='%'){
					const char * floatStr="%f";
					if(str[i+1]=='.'){
						i+=2;
						switch (str[i]) {
							case '0':floatStr="%.0f";break;
							case '1':floatStr="%.1f";break;
							case '2':floatStr="%.2f";break;
							case '3':floatStr="%.3f";break;
							case '4':floatStr="%.4f";break;
							case '5':floatStr="%.5f";break;
							case '6':floatStr="%.6f";break;
							case '7':floatStr="%.7f";break;
							case '8':floatStr="%.8f";break;
							case '9':floatStr="%.9f";break;
						}
					}
					switch (str[++i]) {
						case '%': temp+='%'; break;
						case 'b':
						case 'B':{
							cout<<temp;
							temp="";
							cout << ((*args.boolAtIndex(currentArg++)) ? "TRUE" : "FALSE");
						}break;
						case 'i':
						case 'I':{
							cout<<temp;
							temp="";
							printf("%i",(int)*args.numAtIndex(currentArg++));
						}break;
						case 'f':
						case 'F':
						case 'n':
						case 'N':{
							cout<<temp;
							temp="";
							printf(floatStr,*args.numAtIndex(currentArg++));
						}break;
						case 'v':
						case 'V': {
							cout<<temp;
							temp="";
							Vector v=*args.vecAtIndex(currentArg++);
							cout<<'<';
							printf(floatStr,v.x);
							cout<<',';
							printf(floatStr,v.y);
							cout<<',';
							printf(floatStr,v.z);
							cout<<'>';
						}break;
						case 'q':
						case 'Q': {
							cout<<temp;
							temp="";
							Quaternion q=*args.quatAtIndex(currentArg++);
							cout<<'<';
							printf(floatStr,q.r);
							cout<<',';
							printf(floatStr,q.v.x);
							cout<<',';
							printf(floatStr,q.v.y);
							cout<<',';
							printf(floatStr,q.v.z);
							cout<<'>';
						}break;
						case 'm':
						case 'M': {
							cout<<temp;
							temp="";
							Matrix m=*args.matAtIndex(currentArg++);
							cout<<'[';
							for (int r=0; r<m.rows; ++r) {
								for (int c=0; c<m.cols; ++c) {
									printf(floatStr,m.ValueAtPosition(r, c));
									if(c<m.cols-1)cout<<',';
								}
								if(r<m.rows-1)cout<<'|';
							}
							cout<<']';
						}break;
						case 'p':
						case 'P': {
							cout<<temp;
							temp="";
							Polynomial p=*args.polyAtIndex(currentArg++);
							cout<<'(';
							for (int m=0; m<p.values.size(); ++m) {
								if(p.values[m].coeff<0)cout<<'-';
								else if(m>0)cout<<'+';
								printf(floatStr,fabs(p.values[m].coeff));
								for (int i=0; i<p.values[m].exponents.size(); ++i) {
									int power=p.values[m].exponents[i];
									if (power>0) {
										switch (i) {
											case 0:cout<<"_X";break;
											case 1:cout<<"_Y";break;
											case 2:cout<<"_Z";break;
											case 3:cout<<"_T";break;
											case 4:cout<<"_U";break;
											case 5:cout<<"_V";break;
											case 6:cout<<"_W";break;
										}
										if (power>1) printf("%i",power);
									}
								}
							}
							cout<<')';
						}break;
						case 's':
						case 'S':
							cout<<temp;
							temp="";
							cout<<*args.strAtIndex(currentArg++);
						break;
					}
				}
				else temp+=str[i];
			}
			cout<<temp;
		}break;
		case X_RESIZE:
		{
			XPINSArray* arg0=args.arrAtIndex(0);
			size_t arg1=*args.numAtIndex(1);
			arg0->resize(arg1);
		}break;
	}
}