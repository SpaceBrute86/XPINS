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
	values=std::vector<std::vector<float>>();
	this->Resize(1, 1);
}
Matrix::Matrix (size_t rows, size_t cols){
	values=std::vector<std::vector<float>>();
	this->Resize(rows, cols);
}
Matrix Matrix::IdentityMatrixOfSize(size_t size){
	Matrix matrix=Matrix(size,size);
	for(size_t i=0;i<size;++i){
		matrix.values[i][i]=1;
	}
	return matrix;
}
Matrix Matrix::DiagonalMatrixWithValues(std::vector<float> vals){
	Matrix matrix=Matrix(vals.size(),vals.size());
	for(size_t i=0;i<vals.size();++i){
		matrix.values[i][i]=vals[i];
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
	matrix.values[0][0]=cosf(angle)+x*x*(1-cosf(angle));
	matrix.values[0][1]=z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[0][2]=-y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[1][0]=-z*sinf(angle)+x*y*(1-cosf(angle));
	matrix.values[1][1]=cosf(angle)+y*y*(1-cosf(angle));
	matrix.values[1][2]=x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[2][0]=y*sinf(angle)+x*z*(1-cosf(angle));
	matrix.values[2][1]=-x*sinf(angle)+z*y*(1-cosf(angle));
	matrix.values[2][2]=cosf(angle)+z*z*(1-cosf(angle));
	return matrix;
}
//Accessing Values
float Matrix::ValueAtPosition(size_t i,size_t j){
	return values[i][j];
}
void Matrix::SetValueAtPosition(float val,size_t i,size_t j){
	values[i][j]=val;
}
void Matrix::Resize(size_t rows, size_t cols){
	//Min Size = 1x1;
	if(rows==0)rows=1;
	if(cols==0)cols=1;
	values.resize(rows);
	for(size_t i=0;i<cols;++i){
		values[i].resize(cols);
	}
}
//Converting Matricies to/from Vectors
Vector Matrix::VectorForMatrix(Matrix m)
{
	if(m.values.size()==0)return Vector(0,0,0);
	if(m.values.size()==1){
		return Vector(m.values[0][0],m.values[0][1],m.values[0][2]);
	}
	else
	{
		return Vector(m.values[0][0],m.values[1][0],m.values[2][0]);
	}
}
Matrix Matrix::MatrixForVector(Vector v)
{
	Matrix m=Matrix(3,1);
	m.values[0][0]=v.x;
	m.values[1][0]=v.y;
	m.values[2][0]=v.z;
	return m;
}

//Matrix Operations
Matrix Matrix::Add(Matrix a,Matrix b)
{
	if (a.values.size()!=b.values.size()||a.values[0].size()!=b.values[0].size()) //Sizes don't match
	{
		size_t rows=a.values.size()<b.values.size()?b.values.size():a.values.size();
		size_t cols=a.values[0].size()<b.values[0].size()?b.values[0].size():a.values[0].size();
		a.Resize(rows, cols);
		b.Resize(rows, cols);
	}
	Matrix m=Matrix(a.values.size(),a.values[0].size());
	for (size_t i=0; i<a.values.size(); ++i)
	{
		for (size_t j=0; j<a.values[0].size(); ++j)
		{
			m.values[i][j]=a.values[i][j]+b.values[i][j];
		}
	}
	return m;
}
Matrix Matrix::Scale(Matrix a,float b)
{
	Matrix m=Matrix(a.values.size(),a.values[0].size());
	for (size_t i=0; i<a.values.size(); ++i)
	{
		for (size_t j=0; j<a.values[0].size(); ++j)
		{
			m.values[i][j]=a.values[i][j]*b;
		}
	}
	return m;
}
Matrix Matrix::Multiply(Matrix a,Matrix b)
{
	if (a.values[0].size()!=b.values.size())//Compensate for size errors
	{
		size_t p=a.values[0].size()<b.values.size()?a.values[0].size():b.values.size();
		a.Resize(a.values.size(), p);
		b.Resize(p, b.values[0].size());
	}
	Matrix m=Matrix(a.values.size(),b.values[0].size());
	for (size_t i=0; i<a.values.size(); ++i) {
		for (size_t j=0; j<b.values[0].size(); ++j) {
			for (size_t p=0; p<a.values[0].size(); ++p) {
				m.values[i][j]+=a.values[i][p]*b.values[p][j];
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
	Matrix out=Matrix(in.values[0].size(),in.values.size());
	for (size_t i=0; i<in.values.size(); ++i) {
		for (size_t j=0; j<in.values[0].size(); ++j) {
			out.values[j][i]=in.values[i][j];
		}
	}
	return Matrix(0,0);
}

float Matrix::Determinant(Matrix m)
{
	//Collapse to square Matrix
	if (m.values.size()<m.values[0].size()) {
		m.Resize(m.values.size(), m.values.size());
	}
	else if (m.values.size()>m.values[0].size()) {
		m.Resize(m.values[0].size(), m.values[0].size());
	}
	switch (m.values.size()) {
		case 0:return 0;
		case 1:
			return m.values[0][0];//1x1 matrix
		case 2://2x2 matrix
			return m.values[0][0]*m.values[1][1]-m.values[0][1]*m.values[1][0];
		default://3x3+ matrix
			float det=0;
			for (size_t p=0; p<m.values.size(); ++p) {
				det+=powf(-1, p)*m.values[0][p]*Determinant(MinorMatrix(m, 0, p));
			}
			return det;
	}
	return 0;
}
Matrix Matrix::MinorMatrix(Matrix m, size_t r, size_t c)
{
	Matrix minor=Matrix(m.values.size()-1,m.values.size()-1);
	for (size_t i=0; i<m.values.size()-1; ++i) {
		for (size_t j=0; j<m.values.size()-1; ++j) {
			minor.values[i][j]=m.values[i+r<=i?1:0][j+c<=j?1:0];
		}
	}
	return minor;
}

Matrix Matrix::Invert(Matrix a)
{
	//Collapse to square Matrix
	if (a.values.size()<a.values[0].size()) {
		a.Resize(a.values.size(), a.values.size());
	}
	else if (a.values.size()>a.values[0].size()) {
		a.Resize(a.values[0].size(), a.values[0].size());
	}
	//return Identity Matrix if inverse can't be found
	if (Determinant(a)==0) {
		return IdentityMatrixOfSize(a.values.size());
	}
	//Procede with Cramer's rule stuff
	Matrix b=Matrix(a.values.size(),a.values.size());
	for (int i=0; i<a.values.size(); ++i) {
		for (int j=0; j<a.values.size(); ++j) {
			b.values[i][j]=powf(-1, i+j)*Determinant(MinorMatrix(a, j, i));
		}
	}
	return b;
}