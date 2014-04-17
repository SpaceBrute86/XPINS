//
//  XPNBindings.m
//  XPINS
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import "XPNBindings.h"

struct objWrapper{
	NSObject* object;
};

#pragma mark Primary C++ function
void XPINSObjCBindings::BindFunction(int fNum,string script,XPINSParser::XPINSVarSpace* data,int& i,void* returnVal){
	this->scriptText=script;
	this->data=data;
	this->index=i;
	this->retValue=returnVal;
	[this->bindings bindFunction:fNum];
	i=this->index;
}

void XPINSObjCBindings::returnBool(bool val)
{
	*((bool*)retValue)=val;
}
void XPINSObjCBindings::returnInt(int val)
{
	*((int*)retValue)=val;
}
void XPINSObjCBindings::returnFloat(float val)
{
	*((float*)retValue)=val;
}
void XPINSObjCBindings::returnVec(Vector val)
{
	*((Vector*)retValue)=val;
}
void XPINSObjCBindings::returnMat(Matrix val)
{
	*((Matrix*)retValue)=val;
}
void XPINSObjCBindings::returnStr(NSString* val)
{
	string str=[val cStringUsingEncoding:NSASCIIStringEncoding];
	*((string*)retValue)=str;
}
void XPINSObjCBindings::returnPtr(id val)
{
	objWrapper wrapper=objWrapper();
	wrapper.object=val;
	*((objWrapper*)retValue)=wrapper;
}

#pragma mark getting Arguments

bool XPINSObjCBindings::getBoolArg(int &idx,bool last){
	return ParseBoolArg(scriptText,data,this,index,last?')':',',&idx);
}
int XPINSObjCBindings::getIntArg(int &idx,bool last,bool& fl){
	fl=scriptText[index+1]!='I';
	return ParseIntArg(scriptText,data,this,index,last?')':',',&idx);
}
float XPINSObjCBindings::getFloatArg(int &idx,bool last,bool& fl){
	fl=scriptText[index+1]!='I';
	return ParseFloatArg(scriptText,data,this,index,last?')':',',&idx);
}
Vector XPINSObjCBindings::getVecArg(int &idx,bool last){
	return ParseVecArg(scriptText,data,this,index,last?')':',',&idx);
}
Matrix XPINSObjCBindings::getMatArg(int &idx,bool last){
	return ParseMatArg(scriptText,data,this,index,last?')':',',&idx);
}
NSString* XPINSObjCBindings::getStrArg(int &idx,bool last){
	string str= ParseStrArg(scriptText,data,this,index,last?')':',',&idx);
	return [NSString stringWithCString:str.data() encoding:NSASCIIStringEncoding];
}
id XPINSObjCBindings::getPtrArg(int &idx,bool last){
	objWrapper wrapper=*((objWrapper*)ParsePointerArg(scriptText,data,this,index,last?')':',',&idx));
	return wrapper.object;
}

#pragma mark Passing by Reference
void XPINSObjCBindings::setBoolVar(int index, bool val){
	if(index>-1)
	data->bVars[index]=val;
}
void XPINSObjCBindings::setIntVar(int index, int val, bool fl){
	if(index>-1)
	{
		if(fl)data->fVars[index]=val;
		else data->iVars[index]=val;
	}
}
void XPINSObjCBindings::setFloatVar(int index, float val, bool fl){
	if(index>-1)
	{
		if(fl)data->fVars[index]=val;
		else data->iVars[index]=val;
	}
}
void XPINSObjCBindings::setVecVar(int index, Vector val){
	if(index>-1)
	data->vVars[index]=val;
}
void XPINSObjCBindings::setMatVar(int index, Matrix val){
	if(index>-1)
	data->mVars[index]=val;
}
void XPINSObjCBindings::setStrVar(int index, NSString* val){
	if(index<=-1)return;
	string str=[val cStringUsingEncoding:NSASCIIStringEncoding];
	data->sVars[index]=str;
}
void XPINSObjCBindings::setPtrVar(int index, id val){
	if(index<=-1)return;
	objWrapper wrapper=objWrapper();
	wrapper.object=val;
	data->pVars[index]= &wrapper;
}


@implementation XPNBindings{
	XPINSObjCBindings cppBindings;
}
-(id)init{
	self=[super init];
	cppBindings=XPINSObjCBindings();
	cppBindings.bindings=self;
	return self;
}
-(void)bindFunction:(int)fNum{
	
}
-(XPINSObjCBindings)bindingsObject{
	return cppBindings;
}


-(void) setBoolVar:(int) index value:(bool) val{
	cppBindings.setBoolVar(index, val);
}
-(void) setIntVar:(int) index value:(int) val isFloat:(bool)fl{
	cppBindings.setIntVar(index, val,fl);
}
-(void) setFloatVar:(int) index value:(float) val isFloat:(bool)fl{
	cppBindings.setFloatVar(index, val,fl);
}
-(void) setVecVar:(int) index value:(Vector) val{
	cppBindings.setVecVar(index, val);
}
-(void) setMatVar:(int) index value:(Matrix) val{
	cppBindings.setMatVar(index, val);
}
-(void) setStrVar:(int) index value:(NSString*) val{
	cppBindings.setStrVar(index, val);
}
-(void) setPtrVar:(int) index value:(id) val{
	cppBindings.setPtrVar(index, val);
}


-(bool) getBoolVar:(int) index last:(bool) last{
	return cppBindings.getBoolArg(index, last);
}
-(int) getIntVar:(int) index last:(bool) last isFloat:(bool *)fl{
	bool flo=*fl;
	int val=cppBindings.getIntArg(index, last,*fl);
	*fl=flo;
	return val;
}
-(float) getFloatVar:(int) index last:(bool) last isFloat:(bool *)fl{
	bool flo=*fl;
	float val=cppBindings.getFloatArg(index, last,*fl);
	*fl=flo;
	return val;
}
-(Vector) getVecVar:(int) index last:(bool) last{
	return cppBindings.getVecArg(index, last);
}
-(Matrix) getMatVar:(int) index last:(bool) last{
	return cppBindings.getMatArg(index, last);
}
-(NSString*) getStrVar:(int) index last:(bool) last{
	return cppBindings.getStrArg(index, last);
}
-(id) getPtrVar:(int) index last:(bool) last{
	return cppBindings.getPtrArg(index, last);
}

-(void) returnBool:(bool)val{
	cppBindings.returnBool(val);
}
-(void) returnInt:(int)val{
	cppBindings.returnInt(val);
}
-(void) returnFloat:(float)val{
	cppBindings.returnFloat(val);
}
-(void) returnVec:(Vector)val{
	cppBindings.returnVec(val);
}
-(void) returnMat:(Matrix)val{
	cppBindings.returnMat(val);
}
-(void) returnStr:(NSString*)val{
	cppBindings.returnStr(val);
}
-(void) returnPtr:(id)val{
	cppBindings.returnPtr(val);
}

@end
