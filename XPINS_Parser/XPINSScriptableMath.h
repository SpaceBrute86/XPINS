//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
namespace XPINSScriptableMath{
	class Vector{
	public:
		//creating vectors
		Vector (float, float,float);
		static Vector PolarVector(float,float,float);
		static Vector SphericalVector(float,float,float);
		Vector copy();
		//getting coordinate data
		void RectCoords(float*,float*,float*);//X,Y,Z
		void PolarCoords(float*,float*,float*);//R,Theta,Z
		void SphericalCoords(float*,float*,float*);//Rho, Theta, Phi
		float magnitude();
		float direction();
		float altitude();
		//Vector manipulations.
		static Vector addVectors(Vector,Vector);
		static Vector addVectorArr(Vector*,int);
		static Vector scaledVector(Vector, float);
		static float angleBetweenVectors(Vector,Vector);
		static float dotProduct(Vector,Vector);
		static Vector crossProduct(Vector,Vector);
		static Vector projectionInDirection(Vector,float,float);
	private:
		float x;
		float y;
		float z;
	};
	float addPolar(float,float);
	float dist(float,float);
}
#endif /* defined(__SailingGame__SLNVector__) */
