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

-(bool) getBoolArg:(NSValue**)ref;
-(NSNumber*) getNumArg:(NSValue**)ref;
-(Vector) getVecArg:(NSValue**)ref;
-(Matrix) getMatArg:(NSValue**)ref;
-(NSString*) getStrArg:(NSValue**)ref;
-(id) getPtrArg:(NSValue**)ref;

-(void) setBool:(bool)val Arg:(NSValue*)ref;
-(void) setNum:(NSNumber*)val Arg:(NSValue*)ref;
-(void) setVec:(Vector)val Arg:(NSValue*)ref;
-(void) setMat:(Matrix)val Arg:(NSValue*)ref;
-(void) setStr:(NSString*)val Arg:(NSValue*)ref;
-(void) setPtr:(id)val Arg:(NSValue*)ref;

-(void) returnBool:(bool)val;
-(void) returnNum:(NSNumber*)val;
-(void) returnVec:(Vector)val;
-(void) returnMat:(Matrix)val;
-(void) returnStr:(NSString*)val;
-(void) returnPtr:(id)val;
@end

//C++ Helper
class XPINSObjCBindings: public XPINSBindings
{
public:
	void BindFunction(int,XPINSParser::XPINSScriptSpace&,void*);
	XPNBindings* bindings;
};
