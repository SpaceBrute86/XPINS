//
//  XPNBindings.h
//  XPINS
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import <Foundation/Foundation.h>
#include "XPINS.h"
#import "XPNMath.h"
using namespace XPINSScriptableMath;
using namespace XPINSParser;


//Objective C
@interface XPNBindings : NSObject
-(id)bindFunction:(int)fNum forScript:(XPINSParser::XPINSScriptSpace&)script;

-(bool) getBoolArg:(NSValue**)ref;
-(double) getNumArg:(NSValue**)ref;
-(XPNVector*) getVecArg:(NSValue**)ref;
-(XPNMatrix*) getMatArg:(NSValue**)ref;
-(NSString*) getStrArg:(NSValue**)ref;
-(id) getPtrArg:(NSValue**)ref;
-(NSArray*) getArrArg:(NSValue**)ref;


-(void) setBool:(bool)val Arg:(NSValue*)ref;
-(void) setNum:(double)val Arg:(NSValue*)ref;
-(void) setVec:(XPNVector*)val Arg:(NSValue*)ref;
-(void) setMat:(XPNMatrix*)val Arg:(NSValue*)ref;
-(void) setStr:(NSString*)val Arg:(NSValue*)ref;
-(void) setPtr:(id)val Arg:(NSValue*)ref;
-(void) setArr:(NSArray*)val Arg:(NSValue*)ref;

@end

//C++ Helper
class XPINSObjCBindings: public XPINSBindings
{
public:
	XPNBindings* bindings;
	XPINSObjCBindings(XPNBindings*bind){
		bindings=bind;
	}
	static NSArray* XPINStoNSArray(XPINSArray*);
	static XPINSArray* NStoXPINSArray(NSArray*);
	void* BindFunction(int fNum,XPINSParser::XPINSScriptSpace& script);
};


