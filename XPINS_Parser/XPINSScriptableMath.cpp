//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSScriptableMath.h"
#include <math.h>
using namespace XPINSScriptableMath;


#pragma mark Vector Library

//Initializing Vectors
XPINSScriptableMath::Vector::Vector(){
	this->x=0;
	this->y=0;
	this->z=0;
}
XPINSScriptableMath::Vector::Vector(float x,float y,float z){
	this->x=x;
	this->y=y;
	this->z=z;
}
Vector Vector::PolarVector(float dist, float dir,float z){
	float x=dist*cosf(dir);
	float y=dist*sinf(dir);
	return Vector(x,y,z);
}
Vector Vector::SphericalVector(float dist, float dir,float alt){
	float x=dist*cosf(dir)*sinf(alt);
	float y=dist*sinf(dir)*sinf(alt);
	float z=dist*cosf(alt);
	return Vector(x,y,z);
}

//Vector Components
float Vector::Magnitude(){
	return sqrtf(powf(x, 2)+powf(y, 2)+powf(z,2));
}
float Vector::Direction(){
	return atan2f(y, x);
}
float Vector::Altitude(){
	return atan2f(sqrtf(x*x+y*y), z);
}
void Vector::SphericalCoords(float *dist, float *dir,float *alt){
	*dist=Magnitude();
	*dir=Direction();
	*alt=Altitude();
}
void Vector::PolarCoords(float *dist, float *dir,float *z){
	*dist=sqrtf(powf(x, 2)+powf(y, 2));
	*dir=Direction();
	*z=this->z;
}
void Vector::RectCoords(float *x, float *y,float *z){
	*x=this->x;
	*y=this->y;
	*z=this->z;

}

//Vector Arithmetic
Vector Vector::Add(Vector vec1, Vector vec2){
	float x=vec1.x+vec2.x;
	float y=vec1.y+vec2.y;
	float z=vec1.z+vec2.z;
	return Vector(x,y,z);
}
Vector Vector::Scale(Vector vec, float scale){
	return Vector(vec.x*scale,vec.y*scale,vec.z*scale);
}

float Vector::DotProduct(Vector a,Vector b){
	float x=a.x*b.x;
	float y=a.y*b.y;
	float z=a.z*b.z;
	return x+y+z;
}
Vector Vector::CrossProduct(Vector a,Vector b){
	float x=(a.y*b.z) - (a.z*b.y);
	float y=(a.z*b.x) - (a.x*b.z);
	float z=(a.x*b.y) - (a.y*b.x);
	return Vector(x,y,z);
}

//Miscillaneous Vector Functions
float Vector::AngleBetweenVectors(Vector vec1, Vector vec2){
	float dot=DotProduct(vec1,vec2);
	float cos=dot/(vec1.Magnitude()*vec2.Magnitude());
	return acosf(cos);
}
Vector Vector::ProjectionInDirection(Vector vec,float dir,float alt){
	Vector unit=Vector::SphericalVector(1, dir,alt);
	float dot=DotProduct(vec, unit);
	Vector result=Scale(unit, dot);
	return result;
}

//Related Scalar functions
float XPINSScriptableMath::addPolar(float x,float y){
	float res=x+y;
	while (res<0) res+=2*M_PI;
	while (res>=2*M_PI) res-=2*M_PI;
	return res;
}
float XPINSScriptableMath::dist(float x, float y,float z){
	return sqrtf(x*x+y*y+z*z);
}

#pragma mark Matrix Library

//Initializing Matrices and Special Matrices
Matrix::Matrix (){
	values=std::vector<float>();
	rows=0;
	cols=0;
}

Matrix::Matrix (size_t r, size_t c){
	values=std::vector<float>(r*c);
	rows=r;
	cols=c;
}
Matrix Matrix::IdentityMatrixOfSize(size_t size){
	Matrix matrix=Matrix(size,size);
	for(size_t i=0;i<size;++i){
		matrix.values[i*size+i]=1;
	}
	return matrix;
}
Matrix Matrix::DiagonalMatrixWithValues(std::vector<float> vals){
	Matrix matrix=Matrix(vals.size(),vals.size());
	for(size_t i=0;i<vals.size();++i){
		matrix.values[i*vals.size()+i]=vals[i];
	}
	return matrix;
}
Matrix Matrix::RotationMatrixWithAngleAroundVector(Vector vec,float angle){
	Matrix matrix=Matrix(3,3);
	//get Unit Vector comonents
	Vector unitVec=Vector::Scale(vec, 1/vec.Magnitude());
	float x=0,y=0,z=1;
	unitVec.RectCoords(&x, &y, &z);
	//Compute values
	//Source for Formulas: http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
	matrix.values[0]=cosf(angle)+x*x*(1-cosf(angle));
	matrix.values[1]=z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[2]=-y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[3]=-z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[4]=cosf(angle)+y*y*(1-cosf(angle));
	matrix.values[5]=x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[6]=y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[7]=-x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[8]=cosf(angle)+z*z*(1-cosf(angle));
	return matrix;
}
//Accessing Values
float Matrix::ValueAtPosition(size_t i,size_t j){
	return values[i*cols+j];
}
void Matrix::SetValueAtPosition(float val,size_t i,size_t j){
	values[i*cols+j]=val;
}
//Converting Matricies to/from Vectors
Vector Matrix::VectorForMatrix(Matrix m)
{
	if(m.rows==0)return Vector(0,0,0);
	return Vector(m.values[0],m.rows<1?0:m.values[1],m.rows<2?0:m.values[2]);
}
Matrix Matrix::MatrixForVector(Vector v)
{
	Matrix m=Matrix(3,1);
	m.values[0]=v.x;
	m.values[1]=v.y;
	m.values[2]=v.z;
	return m;
}

//Matrix Operations
Matrix Matrix::Add(Matrix a,Matrix b)
{
	if (a.rows!=b.rows||a.cols!=b.cols) //Sizes don't match
	{
		return Matrix();
	}
	Matrix m=Matrix(a.rows,a.cols);
	for (size_t i=0; i<a.rows; ++i)
	{
		for (size_t j=0; j<a.cols; ++j)
		{
			m.values[i*a.cols+j]=a.values[i*a.cols+j]+b.values[i*b.cols+j];
		}
	}
	return m;
}
Matrix Matrix::Scale(Matrix a,float b)
{
	Matrix m=Matrix(a.rows,a.cols);
	for (size_t i=0; i<a.rows; ++i)
	{
		for (size_t j=0; j<a.cols; ++j)
		{
			m.values[i*a.cols+j]=a.values[i*a.cols+j]*b;
		}
	}
	return m;
}
Matrix Matrix::Multiply(Matrix a,Matrix b)
{
	if (a.cols!=b.rows)//Can't multiply
	{
		return Matrix();
	}
	Matrix m=Matrix(a.rows,b.cols);
	for (size_t i=0; i<a.rows; ++i) {
		for (size_t j=0; j<b.cols; ++j) {
			for (size_t p=0; p<a.cols; ++p) {
				m.values[i*m.cols+j]+=a.values[i*a.cols+p]*b.values[p*b.cols+j];
			}
		}
	}
	return m;

}
Vector Matrix::MultiplyMatrixVector(Matrix m,Vector v)
{
	Matrix vectorMatrix=MatrixForVector(v);
	Matrix resultVectorMatrix=Multiply(m, vectorMatrix);
	return VectorForMatrix(resultVectorMatrix);
}
Matrix Matrix::Transpose(Matrix in)
{
	Matrix out=Matrix(in.cols,in.rows);
	for (size_t i=0; i<in.rows; ++i) {
		for (size_t j=0; j<in.cols; ++j) {
			out.values[j*out.cols+i]=in.values[i*in.cols+j];
		}
	}
	return out;
}

float Matrix::Determinant(Matrix m)
{
	//Collapse to square Matrix
	if (m.rows<m.cols||m.rows>m.cols) {
		return 0;
	}
	switch (m.rows) {
		case 0:return 0;
		case 1:
			return m.values[0];//1x1 matrix
		case 2://2x2 matrix
			return m.values[0]*m.values[3]-m.values[1]*m.values[2];
		default://3x3+ matrix
			float det=0;
			for (size_t p=0; p<m.rows; ++p) {
				det+=powf(-1, p)*m.values[p]*Determinant(MinorMatrix(m, 0, p));
			}
			return det;
	}
	return 0;
}
Matrix Matrix::MinorMatrix(Matrix m, size_t r, size_t c)
{
	Matrix minor=Matrix(m.rows-1,m.rows-1);
	for (size_t i=0; i<m.rows-1; ++i) {
		for (size_t j=0; j<m.rows-1; ++j) {
			minor.values[i*minor.cols+j]=m.values[(i+r<=i?1:0)*m.cols+j+c<=j?1:0];
		}
	}
	return minor;
}

Matrix Matrix::Invert(Matrix a)
{
	//Collapse to square Matrix
	if (a.rows!=a.cols) {
		return Matrix();
	}
	//return Identity Matrix if inverse can't be found
	if (Determinant(a)==0) {
		return IdentityMatrixOfSize(a.rows);
	}
	//Procede with Cramer's rule stuff
	Matrix b=Matrix(a.rows,a.rows);
	for (int i=0; i<a.rows; ++i) {
		for (int j=0; j<a.rows; ++j) {
			b.values[i*b.cols+j]=powf(-1, i+j)*Determinant(MinorMatrix(a, j, i));
		}
	}
	return b;
}