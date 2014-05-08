//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
#include <vector>


namespace XPINSScriptableMath{
	
//Probability simulation
	namespace Probability{
		//Random Variables
		double NormalRV(double mu, double sigma);
		double ExponentialRV(double lambda);
		int PoissonRV(double lambda);
		bool BernoulliRV(double p);
		double UniformRV(double low,double length);
		//Experiments
		int CoinFlip(double p,int n);
		int FairDiceRoll(int sides,int n);
	}
	
//Vector Calculations
	class Vector{
	friend class Matrix;
	public:
		//creating vectors
		Vector ();//Zero Vector
		Vector (double, double,double);
		static Vector PolarVector(double,double,double);
		static Vector SphericalVector(double,double,double);
		//getting coordinate data
		void RectCoords(double*,double*,double*);//X,Y,Z
		void PolarCoords(double*,double*,double*);//R,Theta,Z
		void SphericalCoords(double*,double*,double*);//Rho, Theta, Phi
		double Magnitude();
		double Direction();
		double Altitude();
		//Vector manipulations.
		static Vector Add(Vector,Vector);
		static Vector Scale(Vector, double);
		static double AngleBetweenVectors(Vector,Vector);
		static double DotProduct(Vector,Vector);
		static Vector CrossProduct(Vector,Vector);
		static Vector ProjectionInDirection(Vector,double,double);
		
		//Operator Overloads:
		Vector operator+(const Vector &v) const {
			return Add(*this,v);
		}
		void operator+=(const Vector &v){
			*this=*this+v;
		}
		Vector operator*(const double &f) const {
			return Scale(*this,f);
		}
		void operator*=(const double &f){
			*this=*this*f;
		}
		Vector operator/(const double &f) const {
			return Scale(*this,1/f);
		}
		void operator/=(const double &f){
			*this=*this/f;
		}
		Vector operator-(const Vector &v) const {
			return Add(*this,v*(-1));
		}
		void operator-=(const Vector &v){
			*this=*this-v;
		}
		double operator*(const Vector &v) const {
			return DotProduct(*this,v);
		}
	private:
		double x;
		double y;
		double z;
	};
	
//Matrix Calculations
	class Matrix{
		friend class Vector;

	public:
		//creating Matrices
		Matrix ();
		Matrix (size_t rows, size_t cols);
		void clean();
		//Accessing Components
		double ValueAtPosition(size_t,size_t);
		void SetValueAtPosition(double,size_t,size_t);
		//Special Matrices
		static Matrix IdentityMatrixOfSize(size_t);
		static Matrix DiagonalMatrixWithValues(std::vector<double>);
		static Matrix RotationMatrixWithAngleAroundVector(Vector,double);
		//Converting Matricies to/from Vectors
		static Vector VectorForMatrix(Matrix);
		static Matrix MatrixForVector(Vector);
		//Matrix Operations
		static Matrix Add(Matrix,Matrix);
		static Matrix Scale(Matrix,double);
		static Matrix Multiply(Matrix,Matrix);
		static Vector MultiplyMatrixVector(Matrix,Vector);
		static Matrix Invert(Matrix);
		static Matrix Transpose(Matrix);
		static double Determinant(Matrix);
		
		//Operator Overloads:
		Matrix operator+(const Matrix &m) const
		{
			return Add(*this,m);
		}
		void operator+=(const Matrix &m)
		{
			*this=*this+m;
		}
		Matrix operator*(const double &f) const
		{
			return Scale(*this,f);
		}
		void operator*=(const double &f){
			*this=*this*f;
		}
		Matrix operator/(const double &f) const
		{
			return Scale(*this,1/f);
		}
		void operator/=(const double &f){
			*this=*this/f;
		}
		Matrix operator-(const Matrix &m) const
		{
			return Add(*this,m*(-1));
		}
		void operator-=(const Matrix &m)
		{
			*this=*this-m;
		}
		Matrix operator*(const Matrix &m) const
		{
			return Multiply(*this,m);
		}
		void operator*=(const Matrix &m)
		{
			*this=*this*m;
		}
		Vector operator*(const Vector &v) const {
			return MultiplyMatrixVector(*this,v);
		}
	private:
		double* values;
		size_t rows,cols;
		static Matrix MinorMatrix(Matrix,size_t,size_t);

	};
//Miscillaneous Functions
	double addPolar(double,double);
	double dist(double,double,double);
}
#endif