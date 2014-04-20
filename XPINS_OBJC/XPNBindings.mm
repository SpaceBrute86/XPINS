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
			int num=[self getIntVar:NULL last:YES isFloat:NULL];
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

#pragma mark setting values (pass by reference)
-(void) setBoolVar:(int) index value:(bool) val{
	script->data->bVars[index]=val;
}

-(void) setIntVar:(int) index value:(int) val isFloat:(bool)fl{
	if(index>-1)
	{
		if(fl)script->data->fVars[index]=val;
		else script->data->iVars[index]=val;
	}
}
-(void) setFloatVar:(int) index value:(float) val isFloat:(bool)fl{
	if(index>-1)
	{
		if(fl)script->data->fVars[index]=val;
		else script->data->iVars[index]=val;
	}
}
-(void) setVecVar:(int) index value:(Vector) val{
	if(index>-1) script->data->vVars[index]=val;
}
-(void) setMatVar:(int) index value:(Matrix) val{
	if(index>-1) script->data->mVars[index]=val;
}
-(void) setStrVar:(int) index value:(NSString*) val{
	if(index<=-1)return;
	string str=[val cStringUsingEncoding:NSASCIIStringEncoding];
	script->data->sVars[index]=str;
}
-(void) setPtrVar:(int) index value:(id) val{
	if(index<=-1)return;
	objWrapper wrapper=objWrapper();
	wrapper.object=val;
	script->data->pVars[index]= &wrapper;
}

#pragma mark getting values
-(bool) getBoolVar:(int*)idx last:(bool) last{
	return ParseBoolArg(*script,last?')':',',idx);
}
-(int) getIntVar:(int*) idx last:(bool) last isFloat:(bool *)fl{
	if(fl)*fl=script->instructions[script->index+1]!='I';
	return ParseIntArg(*script,last?')':',',idx);
}
-(float) getFloatVar:(int*) idx last:(bool) last isFloat:(bool *)fl{
	if(fl)*fl=script->instructions[script->index+1]!='I';
	return ParseFloatArg(*script,last?')':',',idx);
}
-(Vector) getVecVar:(int*) idx last:(bool) last{
	return ParseVecArg(*script,last?')':',',idx);
}
-(Matrix) getMatVar:(int*) idx last:(bool) last{
	return ParseMatArg(*script,last?')':',',idx);
}
-(NSString*) getStrVar:(int*) idx last:(bool) last{
	string str= ParseStrArg(*script,last?')':',',idx);
	return [NSString stringWithCString:str.data() encoding:NSASCIIStringEncoding];
}
-(id) getPtrVar:(int*) idx last:(bool) last{
	objWrapper wrapper=*((objWrapper*)ParsePointerArg(*script,last?')':',',idx));
	return wrapper.object;
}
#pragma mark returning values
-(void) returnBool:(bool)val{
	*((bool*)retValue)=val;
}
-(void) returnInt:(int)val{
	*((int*)retValue)=val;
}
-(void) returnFloat:(float)val{
	*((float*)retValue)=val;
}
-(void) returnVec:(Vector)val{
	*((Vector*)retValue)=val;
}
-(void) returnMat:(Matrix)val{
	*((Matrix*)retValue)=val;
}
-(void) returnStr:(NSString*)val{
	string str=[val cStringUsingEncoding:NSASCIIStringEncoding];
	*((string*)retValue)=str;
}
-(void) returnPtr:(id)val{
	objWrapper wrapper=objWrapper();
	wrapper.object=val;
	*((objWrapper*)retValue)=wrapper;
}

@end
