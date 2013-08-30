//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSScriptableMath.h"
#include <math.h>

XPINSScriptableMath::Vector::Vector(float x,float y){
	this->x=x;
	this->y=y;
}
XPINSScriptableMath::Vector* XPINSScriptableMath::Vector::copy(){
	return new Vector(x,y);
}
XPINSScriptableMath::Vector* XPINSScriptableMath::Vector::PolarVector(float dist, float dir){
	float x=dist*cosf(dir);
	float y=dist*sinf(dir);
	return new Vector(x,y);
}
float XPINSScriptableMath::Vector::magnitude(){
	return sqrtf(powf(x, 2)+powf(y, 2));
}
float XPINSScriptableMath::Vector::direction(){
	return atan2f(y, x);
}
void XPINSScriptableMath::Vector::PolarCoords(float *dist, float *dir){
	*dist=magnitude();
	*dir=direction();
}
void XPINSScriptableMath::Vector::RectCoords(float *x, float *y){
	*x=this->x;
	*y=this->y;
}
XPINSScriptableMath::Vector* XPINSScriptableMath::Vector::addVectors(Vector *vec1, Vector *vec2){
	float x=vec1->x+vec2->x;
	float y=vec1->y+vec2->y;
	return new Vector(x,y);
}
XPINSScriptableMath::Vector* XPINSScriptableMath::Vector::addVectorArr(Vector **vectors, int count){
	float x=0,y=0;
	for(int i=0;i<count;i++){
		x+=(vectors[i])->x;
		y+=(vectors[i])->y;
	}
	return new Vector(x,y);
}
float XPINSScriptableMath::addPolar(float x,float y){
	float res=x+y;
	while (res<0) res+=2*M_PI;
	while (res>=2*M_PI) res-=2*M_PI;
	return res;
}
float XPINSScriptableMath::dist(float x, float y){
	return sqrtf(x*x+y*y);
}
XPINSScriptableMath::Vector* XPINSScriptableMath::Vector::scaledVector(Vector *vec, float scale){
	return new Vector(vec->x*scale,vec->y*scale);
}
float XPINSScriptableMath::Vector::angleBetweenVectors(Vector *vec1, Vector *vec2){
	float angle=addPolar(vec1->direction(),-1*vec2->direction());
	return angle;
}
