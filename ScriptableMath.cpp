//
//	ScriptableMath.cpp
//
//  Created by Robbie Markwick on 5/2/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "ScriptableScriptableMath.h"
#include <ScriptableMath.h>

ScriptableMath::Vector::Vector(float x,float y){
	this->x=x;
	this->y=y;
}
ScriptableMath::Vector* ScriptableMath::Vector::copy(){
	return new Vector(x,y);
}
ScriptableMath::Vector* ScriptableMath::Vector::PolarVector(float dist, float dir){
	float x=dist*cosf(dir);
	float y=dist*sinf(dir);
	return new Vector(x,y);
}
float ScriptableMath::Vector::magnitude(){
	return sqrtf(powf(x, 2)+powf(y, 2));
}
float ScriptableMath::Vector::direction(){
	return atan2f(y, x);
}
void ScriptableMath::Vector::PolarCoords(float *dist, float *dir){
	*dist=magnitude();
	*dir=direction();
}
void ScriptableMath::Vector::RectCoords(float *x, float *y){
	*x=this->x;
	*y=this->y;
}
ScriptableMath::Vector* ScriptableMath::Vector::addVectors(Vector *vec1, Vector *vec2){
	float x=vec1->x+vec2->x;
	float y=vec1->y+vec2->y;
	return new Vector(x,y);
}
ScriptableMath::Vector* ScriptableMath::Vector::addVectorArr(Vector **vectors, int count){
	float x=0,y=0;
	for(int i=0;i<count;i++){
		x+=(vectors[i])->x;
		y+=(vectors[i])->y;
	}
	return new Vector(x,y);
}
float ScriptableMath::addPolar(float x,float y){
	float res=x+y;
	while (res<0) res+=2*M_PI;
	while (res>=2*M_PI) res-=2*M_PI;
	return res;
}
float ScriptableMath::dist(float x, float y){
	return sqrtf(x*x+y*y);
}
ScriptableMath::Vector* ScriptableMath::Vector::scaledVector(Vector *vec, float scale){
	return new Vector(vec->x*scale,vec->y*scale);
}
float ScriptableMath::Vector::angleBetweenVectors(Vector *vec1, Vector *vec2){
	float angle=addPolar(vec1->direction(),-1*vec2->direction());
	return angle;
}
