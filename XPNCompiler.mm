//
//  XPNCompiler.m
//  XPINS
//
//  Created by Robbie Markwick on 11/22/13.
//
//

#import "XPNCompiler.h"
#import "XPINSCompiler.h"
#include <string>
using namespace std;
@implementation XPNCompiler
+(NSString*)compileScript:(NSString *)script{
	string scriptString=[script cStringUsingEncoding:NSASCIIStringEncoding];
	XPINSCompiler::compileScript(&scriptString);
	NSString* compiledScript= [NSString stringWithCString:scriptString.data() encoding:NSASCIIStringEncoding];
	return compiledScript;
}
@end
