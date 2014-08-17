//
//  XPINSArray.h
//  XPINS
//
//  Created by Robbie Markwick on 8/13/14.
//
//

#ifndef __XPINS__XPINSArray__
#define __XPINS__XPINSArray__

#include <stdio.h>
#include <vector>
#include <string>
#include "XPINSInstruction.h"
#include "XPINSScriptableMath.h"
using namespace::std;
using namespace::XPINSInstructions;
using namespace::XPINSScriptableMath;
// Array Data Type
class XPINSArray{
public:
	//Get size info
	size_t size();
	void resize(size_t);
	//Memory management
	size_t storageCost();
	void clearArray(size_t index=-1);
	void combineWithArray(XPINSArray&);
	//Get Values
	DataType typeAtIndex(size_t);
	bool* boolAtIndex(size_t);
	double* numAtIndex(size_t);
	Vector* vecAtIndex(size_t);
	Quaternion* quatAtIndex(size_t);
	Matrix* matAtIndex(size_t);
	Polynomial* polyAtIndex(size_t);
	VectorField* fieldAtIndex(size_t);
	string* strAtIndex(size_t);
	void** objAtIndex(size_t);
	XPINSArray* arrAtIndex(size_t);
	//Set Values
	void setBoolAtIndex(bool,size_t);
	void setNumAtIndex(double,size_t);
	void setVecAtIndex(Vector,size_t);
	void setQuatAtIndex(Quaternion,size_t);
	void setMatAtIndex(Matrix,size_t);
	void setPolyAtIndex(Polynomial,size_t);
	void setFieldAtIndex(VectorField,size_t);
	void setStrAtIndex(string,size_t);
	void setObjAtIndex(void*,size_t);
	void setArrAtIndex(XPINSArray,size_t);
	//Set Reference Values
	void setItemAtIndex(void*,DataType,size_t);
private:
	vector<void*> values;	//Value References
	vector<DataType> dataTypes;
};

#endif /* defined(__XPINS__XPINSArray__) */
