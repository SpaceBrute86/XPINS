//
//  SLNVector.h
//  SailingGame
//
//  Created by Robbie Markwick on 5/2/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#ifndef __Script__Math__
#define __Script__Math__
#include <iostream>
namespace ScriptableMath{
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
