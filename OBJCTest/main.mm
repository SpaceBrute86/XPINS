//
//  main.m
//  OBJCTest
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import <Foundation/Foundation.h>
#import "XPNBindings.h"
#import "XPNCompiler.h"
#import "XPNParser.h"
#include <iostream>
#include <fstream>

const bool playground=false;


int main(int argc, const char * argv[])
{

	@autoreleasepool {
		NSURL* scriptURL=[NSURL fileURLWithPath:playground?
			@"/Users/robbiemarkwick/Desktop/XPINS/XPINSTest/PlayGround.XPINS":
			@"/Users/robbiemarkwick/Desktop/XPINS/XPINSTest/TestScript.XPINS"];
		NSError *err;
		NSString* script=[NSString stringWithContentsOfURL:scriptURL encoding:NSASCIIStringEncoding error:&err];
		script=[XPNCompiler compileScript:script];
		NSLog(@"Script Compiled:");
		NSLog(script);
		XPNBindings* bindings=[[XPNBindings alloc] init];
		[XPNParser runScript:script withBindings:bindings];
		
	//	bool result=XPINSCompiler::compileScript(script);
	//	if (result) {
	//		cout<<"\n\n\nCompiled Script:\n"<<script<<endl<<endl;
	//		cout<<"RUNNING SCRIPT:\n\n";
	//		XPINSLocalBindings *bindings=new XPINSLocalBindings();
	//		XPINSParser::ParseScript(script, bindings);
	//	}
	}
    return 0;
}


