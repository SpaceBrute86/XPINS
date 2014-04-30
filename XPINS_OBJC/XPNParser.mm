//
//  XPNParser.m
//  XPINS
//
//  Created by Robbie Markwick on 4/19/14.
//
//

#import "XPNParser.h"
#import "XPINS.h"
#import "XPNBindings.h"

@implementation XPNParser
+(void)runScript:(NSString*)script withBindings:(NSArray*)bindings
{
	string str=[script cStringUsingEncoding:NSASCIIStringEncoding];
	vector<XPINSBindings*> bind=vector<XPINSBindings*>(bindings.count);
	for (NSUInteger i=0; i<bindings.count; ++i) {
		XPINSObjCBindings objBind=[((XPNBindings*)bindings[i]) bindingsObject];
		bind[i]=&objBind;
	}
	XPINSParser::ParseScript(str, bind);
}

@end
