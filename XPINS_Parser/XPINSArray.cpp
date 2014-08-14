//
//  XPINSArray.cpp
//  XPINS
//
//  Created by Robbie Markwick on 8/13/14.
//
//

#include "XPINSArray.h"
void XPINSArray::clearArray(size_t index)
{
	if (index==-1)
	{
		for (size_t i=0; i<values.size(); ++i)
		{
			clearArray(i);
		}
		values.resize(0);
		dataTypes.resize(0);
	}
	else
	{
		switch (dataTypes[index])
		{
			case XPINSInstructions::BOOLEAN:
				delete ((bool*)values[index]);
				break;
			case XPINSInstructions::NUMBER:
				delete ((double*)values[index]);
				break;
			case XPINSInstructions::VECTOR:
				delete ((Vector*)values[index]);
				break;
			case XPINSInstructions::QUATERNION:
				delete ((Quaternion*)values[index]);
				break;
			case MATRIX:
				((Matrix*)values[index])->Clear();
				delete ((Matrix*)values[index]);
				break;
			case POLYNOMIAL:
				((Polynomial*)values[index])->Clear();
				delete ((Polynomial*)values[index]);
				break;
			case FIELD:
				((VectorField*)values[index])->Clear();
				delete ((VectorField*)values[index]);
				break;
			case XPINSInstructions::STRING:
				delete ((string*)values[index]);
				break;
			case XPINSInstructions::OBJECT:
				//	delete ((bool*)values[index]);
				break;
			case ARRAY:
				((XPINSArray*)values[index])->clearArray();
				delete ((XPINSArray*)values[index]);
				break;
		}
	}
}
size_t XPINSArray::storageCost()
{
	int size=0;
	for (int i=0; i<dataTypes.size(); ++i)
	{
		switch (dataTypes[i])
		{
			case XPINSInstructions::BOOLEAN:
			case XPINSInstructions::NUMBER:
			case XPINSInstructions::OBJECT:
				++size;
				break;
			case XPINSInstructions::VECTOR:
				size+=3;
				break;
			case XPINSInstructions::QUATERNION:
				size+=4;
				break;
			case XPINSInstructions::MATRIX:
				size+=((Matrix*)values[i])->GetRows()*((Matrix*)values[i])->GetCols();
				break;
			case XPINSInstructions::POLYNOMIAL:
				size+=((Polynomial*)values[i])->Size()*3;
				break;
			case XPINSInstructions::FIELD:
				size+=((VectorField*)values[i])->P.Size()*3;
				size+=((VectorField*)values[i])->Q.Size()*3;
				size+=((VectorField*)values[i])->R.Size()*3;
				break;
			case XPINSInstructions::STRING:
				size+=((string*)values[i])->length()/8;
				break;
			case XPINSInstructions::ARRAY:
				size+=((XPINSArray*)values[i])->storageCost();
				break;
			default:
				break;
		}
	}
	return size;
}
void XPINSArray::combineWithArray(XPINSArray& secondArr)
{
	size_t oldSize=size();
	resize(oldSize+secondArr.size());
	for(int i=0;i<secondArr.size();++i)
	{
		values[oldSize+i]=secondArr.values[i];
		dataTypes[oldSize+i]=secondArr.dataTypes[i];
	}
	secondArr.values.resize(0);
	secondArr.dataTypes.resize(0);
}


//Size
size_t XPINSArray::size()
{
	return values.size();
}
void XPINSArray::resize(size_t newSize)
{
	size_t oldSize=values.size();
	if (oldSize>newSize) {
		for (size_t i=newSize; i<oldSize; ++i)  clearArray(i);
	} else if (oldSize<newSize) {
		values.resize(newSize);
		dataTypes.resize(newSize);
		for (size_t i=oldSize; i<newSize; ++i){
			void* ptr=new void*(NULL);
			values[i]=new void*(ptr);
			dataTypes[i]=OBJECT;
		}
	}
}


//Get Values
DataType XPINSArray::typeAtIndex(size_t idx)
{
	return dataTypes[idx];
}
bool* XPINSArray::boolAtIndex(size_t idx)
{
	if(dataTypes[idx]==BOOLEAN)return (bool*)values[idx];
	return new bool();
}
double* XPINSArray::numAtIndex(size_t idx)
{
	if(dataTypes[idx]==NUMBER){
		return (double*)values[idx];
	}
	return new double();
}
Vector* XPINSArray::vecAtIndex(size_t idx)
{
	if(dataTypes[idx]==VECTOR)return (Vector*)values[idx];
	if(dataTypes[idx]==MATRIX)return new Vector(Matrix::VectorForMatrix(*(Matrix*)values[idx]));
	return new Vector();
}
Quaternion* XPINSArray::quatAtIndex(size_t idx)
{
	if(dataTypes[idx]==QUATERNION)return (Quaternion*)values[idx];
	return new Quaternion();
}
Matrix* XPINSArray::matAtIndex(size_t idx)
{
	if(dataTypes[idx]==MATRIX)return (Matrix*)values[idx];
	if(dataTypes[idx]==VECTOR)return new Matrix(Matrix::MatrixForVector(*(Vector*)values[idx]));
	return new Matrix();
}
Polynomial* XPINSArray::polyAtIndex(size_t idx)
{
	if(dataTypes[idx]==POLYNOMIAL)return (Polynomial*)values[idx];
	if(dataTypes[idx]==NUMBER)return new Polynomial(*(double*)values[idx]);
	return new Polynomial();
}
VectorField* XPINSArray::fieldAtIndex(size_t idx)
{
	if(dataTypes[idx]==FIELD)return (VectorField*)values[idx];
	if(dataTypes[idx]==VECTOR)return new VectorField(*(Vector*)values[idx]);
	if(dataTypes[idx]==MATRIX)return new VectorField(Matrix::VectorForMatrix(*(Matrix*)values[idx]));
	return new VectorField();
}
string* XPINSArray::strAtIndex(size_t idx)
{
	if(dataTypes[idx]==STRING)return (string*)values[idx];
	return new string();
}
void** XPINSArray::objAtIndex(size_t idx)
{
	if(dataTypes[idx]==OBJECT)return (void**)values[idx];
	return new void*();
}
XPINSArray* XPINSArray::arrAtIndex(size_t idx)
{
	if(dataTypes[idx]==ARRAY)return (XPINSArray*)values[idx];
	return new XPINSArray();
}
//Set Values
void  XPINSArray::setBoolAtIndex(bool val,size_t idx)
{
	values[idx]=new bool(val);
	dataTypes[idx]=BOOLEAN;
}
void  XPINSArray::setNumAtIndex(double val,size_t idx)
{
	values[idx]=new double(val);
	dataTypes[idx]=NUMBER;
}
void  XPINSArray::setVecAtIndex(Vector val,size_t idx)
{
	values[idx]=new Vector(val);
	dataTypes[idx]=VECTOR;
}
void  XPINSArray::setQuatAtIndex(Quaternion val,size_t idx)
{
	values[idx]=new Quaternion(val);
	dataTypes[idx]=QUATERNION;
}
void  XPINSArray::setMatAtIndex(Matrix val,size_t idx)
{
	values[idx]=new Matrix(val);
	dataTypes[idx]=MATRIX;
}
void  XPINSArray::setPolyAtIndex(Polynomial val,size_t idx)
{
	values[idx]=new Polynomial(val);
	dataTypes[idx]=POLYNOMIAL;
}
void  XPINSArray::setFieldAtIndex(VectorField val,size_t idx)
{
	values[idx]=new VectorField(val);
	dataTypes[idx]=FIELD;
}
void  XPINSArray::setStrAtIndex(string val,size_t idx)
{
	values[idx]=new string(val);
	dataTypes[idx]=STRING;
}
void  XPINSArray::setObjAtIndex(void* val,size_t idx)
{
	values[idx]=new void*(val);
	dataTypes[idx]=OBJECT;
}
void  XPINSArray::setArrAtIndex(XPINSArray val,size_t idx)
{
	values[idx]=new XPINSArray(val);
	dataTypes[idx]=ARRAY;
}
void XPINSArray::setItemAtIndex(void*val,DataType type,size_t idx){
	values[idx]=val;
	dataTypes[idx]=type;
}













