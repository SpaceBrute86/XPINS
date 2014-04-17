//
//  XPNBindings.h
//  XPINS
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import <Foundation/Foundation.h>
#include "XPINS.h"
using namespace XPINSScriptableMath;

@class XPNBindings;

class XPINSObjCBindings : public XPINSBindings
{
public:
	virtual void BindFunction(int,string,XPINSParser::XPINSVarSpace* data,int&,void*);
	XPNBindings* bindings;
	
	
	void setBoolVar(int index, bool val);
	void setIntVar(int index, int val,bool fl);
	void setFloatVar(int index, float val,bool fl);
	void setVecVar(int index, Vector val);
	void setMatVar(int index, Matrix val);
	void setStrVar(int index, NSString* val);
	void setPtrVar(int index, id val);
	
	bool getBoolArg(int& index,bool last);
	int getIntArg(int& ,bool last,bool&);
	float getFloatArg(int& ,bool last,bool&);
	Vector getVecArg(int& ,bool last);
	Matrix getMatArg(int& ,bool last);
	NSString* getStrArg(int& ,bool last);
	id getPtrArg(int&,bool last);
	
	void returnBool(bool);
	void returnInt(int);
	void returnFloat(float);
	void returnVec(Vector);
	void returnMat(Matrix);
	void returnStr(NSString*);
	void returnPtr(id);


private:
	string scriptText;
	int index;
	void* retValue;
	XPINSParser::XPINSVarSpace* data;
};


@interface XPNBindings : NSObject
-(void)bindFunction:(int)fNum;
-(XPINSObjCBindings)bindingsObject;

-(void) setBoolVar:(int) index value:(bool) val;
-(void) setIntVar:(int) index value:(int) val isFloat:(bool)fl;
-(void) setFloatVar:(int) index value:(float) val isFloat:(bool)fl;
-(void) setVecVar:(int) index value:(Vector) val;
-(void) setMatVar:(int) index value:(Matrix) val;
-(void) setStrVar:(int) index value:(NSString*) val;
-(void) setPtrVar:(int) index value:(id) val;

-(bool) getBoolVar:(int) index last:(bool) last;
-(int) getIntVar:(int) index last:(bool) last isFloat:(bool*)fl;
-(float) getFloatVar:(int) index last:(bool) last isFloat:(bool*)fl;
-(Vector) getVecVar:(int) index last:(bool) last;
-(Matrix) getMatVar:(int) index last:(bool) last;
-(NSString*) getStrVar:(int) index last:(bool) last;
-(id) getPtrVar:(int) index last:(bool) last;

-(void) returnBool:(bool)val;
-(void) returnInt:(int)val;
-(void) returnFloat:(float)val;
-(void) returnVec:(Vector)val;
-(void) returnMat:(Matrix)val;
-(void) returnStr:(NSString*)val;
-(void) returnPtr:(id)val;
@end
