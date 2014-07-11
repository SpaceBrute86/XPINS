//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
#include <vector>
using namespace std;

class XPINSMathModule;
namespace XPINSScriptableMath
{
	static XPINSMathModule* mathMod; // Polynomial Calculations
	
// Vector Calculations
	class Vector
	{
	public:
		enum coordSystem
		{
			Cartesian,	//Cartesian Vector
			Polar,		//Cylindrical Vecter
			Spherical	//Spherical Vector
		};
	// Creating Vectors
		Vector ();															//Zero Vector
		Vector (double x, double y,double z, coordSystem system);			//Any Vector
	// Getting Coordinate values (NOTE: Pass NULL for any values you are not intersted in)
		void Coords(double* x,double* y,double* z,coordSystem system);	//Get Coordinates in specified system
		double Magnitude();												//Magnitude
	// Vector Operations.
		static Vector Add(Vector,Vector);			//Vector Addition
		static Vector Scale(Vector, double);		//Vector-Scalar Multiplication
		static double DotProduct(Vector,Vector);	//Vector Dot Product
		static Vector CrossProduct(Vector,Vector);	//Vector Cross Product
	// Miscillaneous Vector Functions
		static double AngleBetweenVectors(Vector,Vector);			//Angle between two vectors
		static Vector ProjectionOntoVector(Vector,Vector);			//Projection onto another Vector
		static Vector UnitVectorFromVector(Vector);					//Normalize Vector
	// Miscillaneous Vector-Related Scalar Functions
		static double AddPolar(double a,double b);			//Modular Arithmetic (mod 2pi)
		static double Dist(double x,double y,double z);	//Distance formula

	// Operator Overloads:
		Vector	operator + (const Vector &v)const	{	return Add(*this,v);		}//Add
		void	operator +=(const Vector &v)		{	*this=*this+v;				}
		Vector	operator * (const double &f)const	{	return Scale(*this,f);		}//Multiply
		void	operator *=(const double &f)		{	*this=*this*f;				}
		Vector	operator - (const Vector &v)const	{	return Add(*this,v*(-1));	}//Subtract
		void	operator -=(const Vector &v)		{	*this=*this-v;				}
		Vector	operator / (const double &f)const	{	return Scale(*this,1/f);	}//Divide
		void	operator /=(const double &f)		{	*this=*this/f;				}
		double	operator * (const Vector &v)const	{	return DotProduct(*this,v);	}//Dot Product
		
// Should be private:
		double x,y,z;
	};
	
// Quaternion Calculations
	class Quaternion
	{
	public:
		// Creating Quaterions
		Quaternion ();						//Zero Quaternion
		Quaternion (double r, Vector v);	//Any Quaternion
		// Getting Component values (NOTE: Pass NULL for any values you are not intersted in)
		void Components(double* x,double* y,double* z,double* r);	//Get Components
		double Magnitude();												//Magnitude
		// Quaternion Operations.
		static Quaternion Add(Quaternion,Quaternion);		//Quaternion Addition
		static Quaternion Scale(Quaternion, double);		//Quaternion-Scalar Multiplication
		static Quaternion Multiply(Quaternion,Quaternion);	//Quaternion-Quaternion Multiplication
		// Miscillaneous Quaternion Functions
		static Quaternion ConjugateQuaternion(Quaternion);	//Quaternion conjugate
		static Quaternion InvertQuaternion(Quaternion);		//Quaternion Inverse
		static Vector RotateVector(Quaternion,Vector);		//Rotate Vector with Quaternion
		static Quaternion UnitQuaternion(Quaternion);		//Normalize Quaternion

		// Operator Overloads:
		Quaternion	operator + (const Quaternion &v)const	{	return Add(*this,v);		}//Add
		void		operator +=(const Quaternion &v)		{	*this=*this+v;				}
		Quaternion	operator * (const double &f)const		{	return Scale(*this,f);		}//Q-S Multiply
		void		operator *=(const double &f)			{	*this=*this*f;				}
		Quaternion	operator * (const Quaternion &v)const	{	return Multiply(*this,v);	}//Q-Q Multiply
		void		operator *=(const Quaternion &v)		{	*this=*this+v;				}
		Quaternion	operator - (const Quaternion &v)const	{	return Add(*this,v*(-1));	}//Subtract
		void		operator -=(const Quaternion &v)		{	*this=*this-v;				}
		Quaternion	operator / (const double &f)const		{	return Scale(*this,1/f);	}//Divide
		void		operator /=(const double &f)			{	*this=*this/f;				}
		
		// Should be private:
		double r;
		Vector v;
	};
	
// Matrix Calculations
	class Matrix
	{
	public:
	// Creating and Deleting Matrices
		Matrix ();										//Empty
		Matrix (size_t rows, size_t cols);				//Zero Matrix
		Matrix (size_t rows, size_t cols, double val);	//Constant Value matrix
		Matrix Copy();									//Copy Matrix
		void Clear();									//Delete values
		~Matrix();										
	// Creating Special Matrices
		static Matrix IdentityMatrixOfSize(size_t size);									//Identity Matrix
		static Matrix RotationMatrixWithAngleAroundVector(Vector v,double angle);			//Rotation Matrix
		static Matrix RotationMatrixWithEulerAngles(double roll,double pitch, double yaw);	//Rotation Matrix
		static Matrix RotationMatrixWithQuaternion(Quaternion q);							//Rotation Matrix
	// Accessing Components
		size_t GetRows();											//Number of Rows
		size_t GetCols();											//Number of Columns
		double ValueAtPosition(size_t row,size_t col);				//Get Value at Postition
		void SetValueAtPosition(double val,size_t row,size_t col);	//Set Value at Position
	// Matrix-Vector Conversion
		static Vector VectorForMatrix(Matrix matrix);	//Creates Vector from 1x3 Matrix
		static Matrix MatrixForVector(Vector vector);	//Creates 1x3 Matrix from Vector
	// Basic Matrix Operations
		static Matrix Append(Matrix A,Matrix B);				//Append One matrix to another: [A|B]
		static Matrix Add(Matrix A,Matrix B);					//Add two Matrices
		static Matrix Scale(Matrix matrix,double scalar);		//Multiply Matrix by a scalar
		static Matrix Multiply(Matrix A,Matrix B);				//Multiply two Matrices
		static Matrix Power(Matrix A,int p);					//Repeated Multiplication
		static Vector MVMultiply(Matrix matrix,Vector vector);	//Multiply a Vector by a Matrix
	// Advanced Matrix Operations
		static Matrix Invert(Matrix matrix);				//Invert a Matrix
		static Matrix Transpose(Matrix matrix);				//Transpose a Matrix
		static double Determinant(Matrix matrix);			//Find the determinant for a Square Matrix
		static Matrix RowEchelon(Matrix matrix);			//Row Echelon From
		static Matrix ReducedRowEchelon(Matrix  matrix);	//Reduced Row Echelon Form
		
	//Operator Overloads:
		Matrix	operator + (const Matrix &m)const	{	return Add(*this,m);		}//Add
		void	operator +=(const Matrix &m)		{	*this=*this+m;				}
		Matrix	operator * (const double &f)const	{	return Scale(*this,f);		}//Multiply
		void	operator *=(const double &f)		{	*this=*this*f;				}
		Matrix	operator / (const double &f)const	{	return Scale(*this,1/f);	}//Divide
		void	operator /=(const double &f)		{	*this=*this/f;				}
		Matrix	operator - (const Matrix &m)const	{	return Add(*this,m*(-1));	}//Subtract
		void	operator -=(const Matrix &m)		{	*this=*this-m;				}
		Matrix	operator * (const Matrix &m)const	{	return Multiply(*this,m);	}//Multiply
		void	operator *=(const Matrix &m)		{	*this=*this*m;				}
		Vector	operator * (const Vector &v)const	{	return MVMultiply(*this,v);	}//Multiply
		
	// Should be private:
		double* values;
		size_t rows,cols;
	};
	
// Probability Simulation and Calculation
	namespace Probability{
	// Random Variables
		double NormalRV(double mu, double sigma);	//Normally distributed Random Number
		double ExponentialRV(double lambda);		//Exponentially distributed Random Number
		int PoissonRV(double lambda);				//Poisson Random Number
		bool BernoulliRV(double p);					//Weighted Random Bool
		double UniformRV(double low,double length);	//Uniform Random Number
	// Experiments
		int CoinFlip(double p,int n);	//Flip n coins of bias p
		int FairDiceRoll(int s,int n);	//Roll n Fair s-sided die
	// Markov Chains
		double SimulateMarkovChain(Matrix chain, int n, int start);				//Simulate n stages
		double TransitionProbability(Matrix chain, int n, int start, int stop);	//Transition Probability
		double SteadyStateProbability(Matrix chain, int state);					//Steady-state Probability
		double AbsorbtionProbability(Matrix chain, int state,int start);		//Constrained Steady-State
		double AbsorbtionTime(Matrix chain, int state,int start);				//Expected Absorbtion Time
		int SimulateAbsorbtionTime(Matrix chain, int state,int start);			//Simulate Absorbtion Time
		bool Reachable(Matrix chain, int a, int b);								//Is a reachable form a
		Matrix Subchain(Matrix,int&,int&,bool);
	}
	
// Polynomial Calculations (including Calculus)
	class Polynomial{
	public:
		// Monomial Construction
		struct Monomial{
			double coeff;
			vector<unsigned int> exponents;
			static Monomial monomial(double coeff,double xp, double yp, double zp, double tp, double up, double vp, double wp);
		};
		//Polynomial Creation
		Polynomial ();					//Empty Polynomial
		Polynomial (vector<Monomial>);	//Polynomial with monomials
		Polynomial (double num);		//Polynomial with constant
		Polynomial Copy();				//Copy of Polynomial
		void Clear();					//Delete elements to avoid leaks
		size_t Size();					//Number of elements
		// Polynomial Arithmetic
		static Polynomial Add(Polynomial a,Polynomial b);				//Polynomial Addition
		static Polynomial Scale(Polynomial,double);						//Polynomial-Scalar Multiplication
		static Polynomial Multiply(Polynomial,Polynomial);				//Polynomial Multiplication
		static Polynomial Power(Polynomial a,unsigned int b);			//Repeated Polynomial Multiplication
		static Polynomial Composition(Polynomial,Polynomial,int var);	//Composition of Polynomials
		void Clean();													//Condenses polynomial terms
		// Evaluating
		double Evaluate(vector<double> vars);							//Substiute all variables
		Polynomial PartialEvaluate(double val,int var);					//Substiute one variable
		// Calculus
		static Polynomial Derivative(Polynomial, int var);				//Derivatives
		static Polynomial Integrate(Polynomial, int var);				//Integrals
		
		// Operator overloads
		Polynomial	operator + (const Polynomial &v)const	{	return Add(*this,v);		}//Add
		void		operator +=(const Polynomial &v)		{	*this=*this+v;				}
		Polynomial	operator * (const double &f)const		{	return Scale(*this,f);		}//Multiply
		void		operator *=(const double &f)			{	*this=*this*f;				}
		Polynomial	operator - (const Polynomial &v)const	{	return Add(*this,v*(-1));	}//Subtract
		void		operator -=(const Polynomial &v)		{	*this=*this-v;				}
		Polynomial	operator / (const double &f)const		{	return Scale(*this,1/f);	}//Divide
		void		operator /=(const double &f)			{	*this=*this/f;				}
		Polynomial	operator * (const Polynomial &v)const	{	return Multiply(*this,v);	}//Multiply
		void		operator *=(const Polynomial &v)		{	*this=*this*v;				}
		
		// Should be private:
		vector<Monomial> values;
	};

//Vector Field Calculations
	struct Bound //Start and End points for Line Integral or Boundaries for surface integral
	{
		Polynomial start,stop;
		Bound(Polynomial start, Polynomial stop);
	};
	class VectorField //Vector with Polynomail components, is superclass
	{
	public:
		// Vector Creation
		VectorField ();											//Zero Vector
		VectorField (Polynomial p,Polynomial q,Polynomial r);	//Vector Field with values
		VectorField (Vector v);									//Constant Vector Field
		VectorField Copy();										//Copy Field
		void Clear();											//Clear Field
		//Copy Field
		static VectorField GradientField(Polynomial poly);				//Gradient Vector field
		// Vector-like Operations
		static VectorField Add(VectorField a, VectorField b);	//Add VectorFields
		static VectorField Scale(VectorField a, double b);		//Scale a VectorField by double
		static VectorField PScale(VectorField a, Polynomial b);	//Scale a VectorField by polynomial
		static Polynomial Dot(VectorField a, VectorField b);	//Dot product of VectorFields
		static VectorField Cross(VectorField a, VectorField b);	//Cross product of VectorFields
		static VectorField MatMult(Matrix a, VectorField b);	//Multiply VectorField and Matrix
		// Composition and evaluation
		static VectorField Compose(VectorField a, Polynomial b, int var);	//Composition
		VectorField PartialEvaluate(double val, int var);					//Partial Evaluation
		Vector Evaluate(vector<double> vals);								//Evaluation
		// Calculus
		static VectorField Derivative(VectorField field, int var);	//Derivative
		static VectorField Integrate(VectorField field, int var);	//Integral
	// Vector Calculus
		// Divergence and Curl
		Polynomial Divergence();
		VectorField Curl();
		// Line and Surface Integrals
		static Polynomial LineIntegral(Polynomial f,VectorField curve, Bound uB);
		static Polynomial LineIntegral(VectorField v,VectorField curve, Bound uB);
		static Polynomial SurfaceIntegral(Polynomial f,VectorField surface, Bound uB, Bound vB);
		static Polynomial SurfaceIntegral(VectorField v,VectorField surface, Bound uB, Bound vB);
		static Polynomial VolumeIntegral(Polynomial f,VectorField map, Bound uB, Bound vB, Bound wB);

		// Operator Overloads
		VectorField	operator + (const VectorField &v)const	{	return Add(*this,v);		}//Add
		void		operator +=(const VectorField &v)		{	*this=*this+v;				}
		VectorField	operator * (const Polynomial &f)const	{	return PScale(*this,f);		}//Multiply
		void		operator *=(const Polynomial &f)		{	*this=*this*f;				}
		VectorField	operator * (const double &f)const		{	return Scale(*this,f);		}//Multiply
		void		operator *=(const double &f)			{	*this=*this*f;				}
		VectorField	operator - (const VectorField &v)const	{	return Add(*this,v*(-1));	}//Subtract
		void		operator -=(const VectorField &v)		{	*this=*this-v;				}
		Polynomial	operator * (const VectorField &v)const	{	return Dot(*this,v);		}//Dot Product
		
		// Values
		Polynomial P,Q,R; //Vector is <P,Q,R>
	};
}
#endif