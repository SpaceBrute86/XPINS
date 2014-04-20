//
//  XPNCompiler.m
//  XPINS
//
//  Created by Robbie Markwick on 4/19/14.
//
//

#import "XPNCompiler.h"
#include <string>
#include "XPINS.h"

@implementation XPNCompiler
+(NSString*) compileScript:(NSString*)script;
{
	string str=[script cStringUsingEncoding:NSASCIIStringEncoding];
	if(XPINSCompiler::compileScript(str))
		return [NSString stringWithCString:str.data() encoding:NSASCIIStringEncoding];
	return script;
}

@end
