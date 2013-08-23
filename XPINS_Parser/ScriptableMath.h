//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

#ifndef __XPINS__Math__
#define __XPINS__Math__
#include <iostream>
namespace XPINSScriptableMath{
	class Vector{
	public:
		//creating vectors
		Vector (float, float);
		static Vector* PolarVector(float,float);
		Vector *copy();
		//getting coordinate data
		void RectCoords(float*,float*);//X,Y
		void PolarCoords(float*,float*);//R,Theta
		float magnitude();
		float direction();
		//Vector manipulations.
		static Vector* addVectors(Vector*,Vector*);
		static Vector* addVectorArr(Vector**,int);
		static Vector* scaledVector(Vector*, float);
		static float angleBetweenVectors(Vector*,Vector*);
	private:
		float x;
		float y;
	};
	float addPolar(float,float);
	float dist(float,float);
}
#endif /* defined(__SailingGame__SLNVector__) */
