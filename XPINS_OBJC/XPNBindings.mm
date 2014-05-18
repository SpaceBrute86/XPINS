//
//  XPNBindings.m
//  XPINS
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import "XPNBindings.h"
#include <iostream>
#include <vector>

using namespace XPINSParser;
struct objWrapper
{
	NSObject* object;
	objWrapper(NSObject*obj)
	{
		object=obj;
	}
};
void* XPINSObjCBindings::BindFunction(int fNum,XPINSParser::XPINSScriptSpace& script){
	string sc=script.instructions;
	int ind=script.index;
	NSObject* obj=[this->bindings bindFunction:fNum forScript:script];
	if([obj isKindOfClass:[NSNumber class]]){//BOOL or NUM
		while (sc[ind]!='#') --ind;
		if(sc[ind+1]=='B'){
			return new bool([((NSNumber*)obj) boolValue]);
		}
		else{
			return new double([((NSNumber*)obj) doubleValue]);
		}
	}
	else if([obj isKindOfClass:[XPNVector class]]){//Vector
		return &((XPNVector*)obj)->vector;
	}
	else if([obj isKindOfClass:[XPNMatrix class]]){//Vector
		return &((XPNMatrix*)obj)->matrix;
	}
	else if([obj isKindOfClass:[XPNPolynomial class]]){//Vector
		return &((XPNPolynomial*)obj)->polynomial;
	}
	else if([obj isKindOfClass:[NSString class]]){
		new string([((NSString*)obj) cStringUsingEncoding:NSASCIIStringEncoding]);
	}
	else if([obj isKindOfClass:[NSArray class]]){
		return NStoXPINSArray((NSArray*)obj);
	}
	else{
		return new objWrapper(obj);
	}
	return NULL;
}

NSArray*  XPINSObjCBindings::XPINStoNSArray(XPINSArray* arr){
	NSMutableArray*val=[@[] mutableCopy];
	for(int i=0;i<arr->values.size();++i){
		switch (arr->types[i]) {
			case 'B':
				[val addObject:@(*(bool*)arr->values[i])];
				break;
			case 'N':
				[val addObject:@(*(double*)arr->values[i])];
				break;
			case 'V':
				[val addObject:[XPNVector vectorWithVector:*(Vector*)arr->values[i]]];
				break;
			case 'M':
				[val addObject:[XPNMatrix matrixWithMatrix:*(Matrix*)arr->values[i]]];
				break;
			case 'P':
				[val addObject:[XPNPolynomial polynomialWithPolynomial:*(Polynomial*)arr->values[i]]];
				break;
			case 'S':
				[val addObject:[NSString stringWithCString:((string*)arr->values[i])->data() encoding:NSASCIIStringEncoding]];
				break;
			case 'O':
				[val addObject:((objWrapper*)arr->values[i])->object];
				break;
			case 'A':
				[val addObject:XPINStoNSArray((XPINSArray*)arr->values[i])];
				break;
			default:
				break;
		}
	}
	return val;
}
XPINSArray*  XPINSObjCBindings::NStoXPINSArray(NSArray* arr){
	XPINSArray*val=new XPINSArray();
	for(int i=0;i<arr.count;++i){
		NSObject*obj=arr[i];
		if([obj isKindOfClass:[NSNumber class]]){//BOOL or NUM
			double d=[((NSNumber*)obj) doubleValue];
			val->values[i]=&d;
			val->types[i]='N';
		}
		else if([obj isKindOfClass:[XPNVector class]]){//Vector
			val->values[i]= &((XPNVector*)obj)->vector;
			val->types[i]='V';
		}
		else if([obj isKindOfClass:[XPNMatrix class]]){//Matrix
			val->values[i]= &((XPNMatrix*)obj)->matrix;
			val->types[i]='M';
		}
		else if([obj isKindOfClass:[XPNPolynomial class]]){//Matrix
			val->values[i]= &((XPNPolynomial*)obj)->polynomial;
			val->types[i]='P';
		}
		else if([obj isKindOfClass:[NSString class]]){
			string str=[((NSString*)obj) cStringUsingEncoding:NSASCIIStringEncoding];
			val->values[i]=&str;
			val->types[i]='S';
		}
		else if([obj isKindOfClass:[NSArray class]]){
			val->values[i]=NStoXPINSArray((NSArray*)obj);
			val->types[i]='A';
		}
		else{
			val->values[i]=new objWrapper(obj);
			val->types[i]='O';
		}
	}
	return val;
}

@implementation XPNBindings{
	XPINSParser::XPINSScriptSpace* script;
	void* retValue;
}
-(id)init{
	self=[super init];
	return self;
}
-(id)bindFunction:(int)fNum forScript:(XPINSParser::XPINSScriptSpace&)scriptSpace
{
	script=&scriptSpace;
	switch (fNum) {
			/*Bind Your Custom Functions*/
		case 1:{
			int num=[self getNumArg:NULL];
			cout<<num<<"\t";
		}break;
		case 2:{
			cout<<"\n";
		}break;
	}
	return nil;
}

-(bool) getBoolArg:(NSValue**)ref{
	bool* val= ParseBoolArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return *val;
}
-(double) getNumArg:(NSValue**)ref{
	double* val= ParseNumArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return *val;
}
-(XPNVector*) getVecArg:(NSValue**)ref{
	Vector* val= ParseVecArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return [XPNVector vectorWithVector:*val];
}
-(XPNMatrix*) getMatArg:(NSValue**)ref{
	Matrix* val= ParseMatArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return [XPNMatrix matrixWithMatrix:*val];
}
-(XPNPolynomial*) getPolyArg:(NSValue**)ref{
	Polynomial* val= ParsePolyArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return [XPNPolynomial polynomialWithPolynomial:*val];
}
-(NSString*) getStrArg:(NSValue**)ref{
	string* val= ParseStrArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return [NSString stringWithCString:val->data() encoding:NSASCIIStringEncoding];
}
-(id) getPtrArg:(NSValue**)ref{
	objWrapper* val=((objWrapper*)ParsePointerArg(*script,','));
	*ref=[NSValue valueWithPointer:val];
	return val->object;
}
-(NSArray*) getArrArg:(NSValue**)ref{
	XPINSArray* arr=(XPINSArray*)ParsePointerArg(*script,',');
	*ref=[NSValue valueWithPointer:arr];
	return XPINSObjCBindings::XPINStoNSArray(arr);
}

-(void) setBool:(bool)val Arg:(NSValue*)ref{
	bool* addr=(bool*)[ref pointerValue];
	*addr=val;
}
-(void) setNum:(double)val Arg:(NSValue*)ref{
	double* addr=(double*)[ref pointerValue];
	*addr=val;
}
-(void) setVec:(XPNVector*)val Arg:(NSValue*)ref{
	Vector* addr=(Vector*)[ref pointerValue];
	*addr=val->vector;
}
-(void) setMat:(XPNMatrix*)val Arg:(NSValue*)ref{
	Matrix* addr=(Matrix*)[ref pointerValue];
	*addr=val->matrix;
}
-(void) setPoly:(XPNPolynomial*)val Arg:(NSValue*)ref{
	Polynomial* addr=(Polynomial*)[ref pointerValue];
	*addr=val->polynomial;
}
-(void) setStr:(NSString*)val Arg:(NSValue*)ref{
	string* addr=(string*)[ref pointerValue];
	*addr=[val cStringUsingEncoding:NSASCIIStringEncoding];
}
-(void) setPtr:(id)val Arg:(NSValue*)ref{
	void** addr=(void**)[ref pointerValue];
	*addr=new objWrapper(val);
}
-(void) setArr:(NSArray*)val Arg:(NSValue*)ref{
	void** addr=(void**)[ref pointerValue];
	*addr=XPINSObjCBindings::NStoXPINSArray(val);
}


@end
