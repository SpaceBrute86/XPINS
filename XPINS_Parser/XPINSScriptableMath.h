//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
#include <vector>


namespace XPINSScriptableMath{
	class Vector{
	friend class Matrix;
	public:
		//creating vectors
		Vector ();//Zero Vector
		Vector (float, float,float);
		static Vector PolarVector(float,float,float);
		static Vector SphericalVector(float,float,float);
		//getting coordinate data
		void RectCoords(float*,float*,float*);//X,Y,Z
		void PolarCoords(float*,float*,float*);//R,Theta,Z
		void SphericalCoords(float*,float*,float*);//Rho, Theta, Phi
		float Magnitude();
		float Direction();
		float Altitude();
		//Vector manipulations.
		static Vector Add(Vector,Vector);
		static Vector Scale(Vector, float);
		static float AngleBetweenVectors(Vector,Vector);
		static float DotProduct(Vector,Vector);
		static Vector CrossProduct(Vector,Vector);
		static Vector ProjectionInDirection(Vector,float,float);
	private:
		float x;
		float y;
		float z;
	};
	class Matrix{
		friend class Vector;

	public:
		//creating Matrices
		Matrix ();
		Matrix (size_t rows, size_t cols);
		//Accessing Components
		float ValueAtPosition(size_t,size_t);
		void SetValueAtPosition(float,size_t,size_t);
		void Resize(size_t,size_t);
		//Special Matrices
		static Matrix IdentityMatrixOfSize(size_t);
		static Matrix DiagonalMatrixWithValues(std::vector<float>);
		static Matrix RotationMatrixWithAngleAroundVector(Vector,float);
		//Converting Matricies to/from Vectors
		static Vector VectorForMatrix(Matrix);
		static Matrix MatrixForVector(Vector);
		//Matrix Operations
		static Matrix Add(Matrix,Matrix);
		static Matrix Scale(Matrix,float);
		static Matrix Multiply(Matrix,Matrix);
		static Vector MultiplyMatrixVector(Matrix,Vector);
		static Matrix Invert(Matrix);
		static Matrix Transpose(Matrix);
		static float Determinant(Matrix);
	private:
		std::vector< std::vector<float> > values;
		static Matrix MinorMatrix(Matrix,size_t,size_t);

	};

	float addPolar(float,float);
	float dist(float,float,float);
}
#endif