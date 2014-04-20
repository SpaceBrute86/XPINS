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

class XPINSObjCBindings;
//Objective C
@interface XPNBindings : NSObject
-(void)bindFunction:(int)fNum forScript:(XPINSParser::XPINSScriptSpace&)script returnValue:(void*) returnVal;

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

//C++ Helper
class XPINSObjCBindings: public XPINSBindings
{
private:
	XPINSParser::XPINSScriptSpace* script;
	void* retValue;
public:
	XPNBindings* bindings;
	virtual void BindFunction(int,XPINSParser::XPINSScriptSpace&,void*);
};
