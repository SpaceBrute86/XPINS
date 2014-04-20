//
//  XPNParser.m
//  XPINS
//
//  Created by Robbie Markwick on 4/19/14.
//
//

#import "XPNParser.h"
#import "XPINS.h"
@implementation XPNParser
+(void)runScript:(NSString*)script withBindings:(XPNBindings*)bindings
{
	string str=[script cStringUsingEncoding:NSASCIIStringEncoding];
	XPINSObjCBindings bind=[bindings bindingsObject];
	XPINSParser::ParseScript(str, &bind);
}

@end
