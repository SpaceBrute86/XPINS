//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
#include <vector>
using namespace std;


namespace XPINSScriptableMath{
	
// Vector Calculations
	class Vector
	{
		friend class Matrix;
	public:
	// Creating Vectors
		Vector ();															//Zero Vector
		Vector (double x, double y,double z);								//Cartesian Vector
		static Vector PolarVector(double r,double theta,double z);			//Cylindrical Vector
		static Vector SphericalVector(double rho,double theta,double phi);	//Spheircal Vector
	// Getting Coordinate values (NOTE: Pass NULL for any values you are not intersted in)
		void RectCoords(double* x,double* y,double* z);					//X,Y,Z
		void PolarCoords(double* r,double* theta,double* z);			//R,Theta,Z
		void SphericalCoords(double* rho,double* theta,double* phi);	//Rho, Theta, Phi
		double Magnitude();												//Magnitude
	// Vector Operations.
		static Vector Add(Vector,Vector);			//Vector Addition
		static Vector Scale(Vector, double);		//Vector-Scalar Multiplication
		static double DotProduct(Vector,Vector);	//Vector Dot Product
		static Vector CrossProduct(Vector,Vector);	//Vector Cross Product
	// Miscillaneous Vector Functions
		static double AngleBetweenVectors(Vector,Vector);			//Angle between two vectors
		static Vector ProjectionInDirection(Vector,double,double);	//Projection onto another Vector
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
		
	private:
		double x,y,z;
	};
	
// Matrix Calculations
	class Matrix
	{
		friend class Vector;
	public:
	// Creating and Deleting Matrices
		Matrix ();										//Empty
		Matrix (size_t rows, size_t cols);				//Zero Matrix
		Matrix (size_t rows, size_t cols, double val);	//Constant Value matrix
		Matrix copy();									//Copy Matrix
		~Matrix();										//Delete values
	// Creating Special Matrices
		static Matrix IdentityMatrixOfSize(size_t size);							//Identity Matrix
		static Matrix RotationMatrixWithAngleAroundVector(Vector v,double angle);	//Rotation Matrix
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
		
	private:
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
		int CoinFlip(double p,int n);		//Flip n coins of bias p
		int FairDiceRoll(int s,int n);	//Roll n Fair s-sided die
	// Markov Chains
		double SimulateMarkovChain(Matrix chain, int n, int start);				//Simulate n stages
		double TransitionProbability(Matrix chain, int n, int start, int stop);	//Transition Probability
		double SteadyStateProbability(Matrix chain, int state);					//Steady-state Probability
		double AbosorbtionProbability(Matrix chain, int state,int start);		//Constrained Steady-State
		double AbosorbtionTime(Matrix chain, int state,int start);				//Expected Absorbtion Time
		int SimulateAbosorbtionTime(Matrix chain, int state,int start);			//Simulate Absorbtion Time
		bool Reachable(Matrix chain, int b, int a);								//Is b reachable form a
		Matrix Subchain(Matrix,int&,int&,bool);
	}
	
// Polynomial Calculations
	class Polynomial{
	public:
	// Monomial Construction
		struct Monomial{
			double coeff;
			vector<unsigned int> exponents;
			static Monomial monomial(double coeff,double xp, double yp, double zp, double tp);
		};
	//Polynomial Creation
		Polynomial ();					//Empty Polynomial
		Polynomial (vector<Monomial>);	//Polynomial with monomials
		Polynomial Copy();				//Copy of Polynomial
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
		Polynomial	operator - (const Polynomial &v)const	{	return Add(*this,v*(-1));	}//Subtract
		void		operator -=(const Polynomial &v)		{	*this=*this-v;				}
		Polynomial	operator * (const double &f)const		{	return Scale(*this,f);		}//Multiply
		void		operator *=(const double &f)			{	*this=*this*f;				}
		Polynomial	operator / (const double &f)const		{	return Scale(*this,1/f);	}//Divide
		void		operator /=(const double &f)			{	*this=*this/f;				}
		Polynomial	operator * (const Polynomial &v)const	{	return Multiply(*this,v);	}//Multiply
		void		operator *=(const Polynomial &v)		{	*this=*this*v;				}
		
	private:
		vector<Monomial> values;
	};
}
#endif