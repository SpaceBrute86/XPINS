//
//  XPNBindings.m
//  XPINS
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import "XPNBindings.h"
#include <iostream>

using namespace XPINSParser;
struct objWrapper{
	NSObject* object;
	objWrapper(NSObject*obj){
		object=obj;
	}
};

void XPINSObjCBindings::BindFunction(int fNum,XPINSParser::XPINSScriptSpace& script, void* returnVal){
	[this->bindings bindFunction:fNum forScript:script returnValue:returnVal];
}


@implementation XPNBindings{
	XPINSObjCBindings cppBindings;
	XPINSParser::XPINSScriptSpace* script;
	void* retValue;
}
-(id)init{
	self=[super init];
	cppBindings=*(new XPINSObjCBindings());
	cppBindings.bindings=self;
	return self;
}
-(void)bindFunction:(int)fNum forScript:(XPINSParser::XPINSScriptSpace&)scriptSpace returnValue:(void*) returnVal
{
	script=&scriptSpace;
	retValue=returnVal;
	switch (fNum) {
			/*Bind Your Custom Functions*/
		case 1:{
			int num=[[self getNumArg:NULL] intValue];
			cout<<num<<"\t";
		}break;
		case 2:{
			cout<<"\n";
		}break;
	}
}
-(XPINSObjCBindings)bindingsObject{
	return cppBindings;
}
-(bool) getBoolArg:(NSValue**)ref{
	bool* val= ParseBoolArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return *val;
}
-(NSNumber*) getNumArg:(NSValue**)ref{
	double* val= ParseNumArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return @(*val);
}
-(Vector) getVecArg:(NSValue**)ref{
	Vector* val= ParseVecArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return *val;
}
-(Matrix) getMatArg:(NSValue**)ref{
	Matrix* val= ParseMatArg(*script,',');
	if(ref)*ref=[NSValue valueWithPointer:val];
	return *val;
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

-(void) setBool:(bool)val Arg:(NSValue*)ref{
	bool* addr=(bool*)[ref pointerValue];
	*addr=val;
}
-(void) setNum:(NSNumber*)val Arg:(NSValue*)ref{
	double* addr=(double*)[ref pointerValue];
	*addr=[val doubleValue];
}
-(void) setVec:(Vector)val Arg:(NSValue*)ref{
	Vector* addr=(Vector*)[ref pointerValue];
	*addr=val;
}
-(void) setMat:(Matrix)val Arg:(NSValue*)ref{
	Matrix* addr=(Matrix*)[ref pointerValue];
	*addr=val;
}
-(void) setStr:(NSString*)val Arg:(NSValue*)ref{
	string* addr=(string*)[ref pointerValue];
	*addr=[val cStringUsingEncoding:NSASCIIStringEncoding];
}
-(void) setPtr:(id)val Arg:(NSValue*)ref{
	void** addr=(void**)[ref pointerValue];
	*addr=new objWrapper(val);
}

-(void) returnBool:(bool)val{
	*((bool*)retValue)=val;
}
-(void) returnNum:(NSNumber*)val{
	*((double*)retValue)=[val doubleValue];
}
-(void) returnVec:(Vector)val{
	*((Vector*)retValue)=val;
}
-(void) returnMat:(Matrix)val{
	*((Matrix*)retValue)=val;
}
-(void) returnStr:(NSString*)val{
	*((string*)retValue)=[val cStringUsingEncoding:NSASCIIStringEncoding];
}
-(void) returnPtr:(id)val{
	*((void**)retValue)=new objWrapper(val);
}

@end
