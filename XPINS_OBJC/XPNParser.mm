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
#import "XPNMathModule.h"

NSOperationQueue *XPINSQueue;
@implementation XPNParser
+(void)runScript:(NSString*)script withBindings:(NSArray*)bindings
{
	//Set up C++ script string
	string str=[script cStringUsingEncoding:NSASCIIStringEncoding];
	//Set up Bindings
	vector<XPINSBindings*> bind=vector<XPINSBindings*>(bindings.count);
	for (NSUInteger i=0; i<bindings.count; ++i) {
		bind[i]=new XPINSObjCBindings((XPNBindings*)bindings[i]);
	}
	//Set up Math module
	XPINSScriptableMath::mathMod=new XPNMathModule();
	//Set up Operation Queue
	if(!XPINSQueue)
	{
		XPINSQueue=[[NSOperationQueue alloc] init];
		XPINSQueue.maxConcurrentOperationCount=NSOperationQueueDefaultMaxConcurrentOperationCount;
	}
	//Run script
	[XPINSQueue addOperationWithBlock:^{
		XPINSParser::ParseScript(str, bind);
	}];
}
+(void)emptyTrash
{
	[XPINSQueue setSuspended:YES];
	XPINSParser::EmptyAllGarbage();
	[XPINSQueue setSuspended:NO];
}
@end
