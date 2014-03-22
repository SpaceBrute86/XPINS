//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSScriptableMath.h"
#include <math.h>

XPINSScriptableMath::Vector::Vector(float x,float y,float z){
	this->x=x;
	this->y=y;
	this->z=z;
}

XPINSScriptableMath::Vector XPINSScriptableMath::Vector::copy(){
	return Vector(x,y,z);
}
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::PolarVector(float dist, float dir,float z){
	float x=dist*cosf(dir);
	float y=dist*sinf(dir);
	return Vector(x,y,z);
}
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::SphericalVector(float dist, float dir,float alt){
	float x=dist*cosf(dir)*sinf(alt);
	float y=dist*sinf(dir)*sinf(alt);
	float z=dist*cosf(alt);
	return Vector(x,y,z);
}

float XPINSScriptableMath::Vector::magnitude(){
	return sqrtf(powf(x, 2)+powf(y, 2)+powf(z,2));
}
float XPINSScriptableMath::Vector::direction(){
	return atan2f(y, x);
}
float XPINSScriptableMath::Vector::altitude(){
	return atan2f(sqrtf(x*x+y*y), z);
}
void XPINSScriptableMath::Vector::SphericalCoords(float *dist, float *dir,float *alt){
	*dist=magnitude();
	*dir=direction();
	*alt=altitude();
}
void XPINSScriptableMath::Vector::PolarCoords(float *dist, float *dir,float *z){
	*dist=sqrtf(powf(x, 2)+powf(y, 2));
	*dir=direction();
	*z=this->z;
}
void XPINSScriptableMath::Vector::RectCoords(float *x, float *y,float *z){
	*x=this->x;
	*y=this->y;
	*z=this->z;

}
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::addVectors(Vector vec1, Vector vec2){
	float x=vec1.x+vec2.x;
	float y=vec1.y+vec2.y;
	float z=vec1.z+vec2.z;
	return Vector(x,y,z);
}
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::addVectorArr(Vector *vectors, int count){
	float x=0,y=0,z=0;
	for(int i=0;i<count;i++){
		x+=(vectors[i]).x;
		y+=(vectors[i]).y;
		z+=(vectors[i]).z;
	}
	return Vector(x,y,z);
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
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::scaledVector(Vector vec, float scale){
	return Vector(vec.x*scale,vec.y*scale,vec.z*scale);
}
float XPINSScriptableMath::Vector::angleBetweenVectors(Vector vec1, Vector vec2){
	float dot=dotProduct(vec1,vec2);
	float cos=dot/(vec1.magnitude()*vec2.magnitude());
	return acosf(cos);
}
float XPINSScriptableMath::Vector::dotProduct(Vector a,Vector b){
	float x=a.x*b.x;
	float y=a.y*b.y;
	float z=a.z*b.z;
	return x+y+z;
}
XPINSScriptableMath::Vector XPINSScriptableMath::Vector::crossProduct(Vector a,Vector b){
	float x=(a.y*b.z) - (a.z*b.y);
	float y=(a.z*b.x) - (a.x*b.z);
	float z=(a.x*b.y) - (a.y*b.x);
	return Vector(x,y,z);
}

XPINSScriptableMath::Vector XPINSScriptableMath::Vector::projectionInDirection(Vector vec,float dir,float alt){
	Vector unit=Vector::SphericalVector(1, dir,alt);
	float dot=dotProduct(vec, unit);
	Vector result=scaledVector(unit, dot);
	return result;
}






