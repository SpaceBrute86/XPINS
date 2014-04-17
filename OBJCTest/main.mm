//
//  main.m
//  OBJCTest
//
//  Created by Robbie Markwick on 4/16/14.
//
//

#import <Foundation/Foundation.h>
#import "XPNBindings.h"
#include <iostream>
#include <fstream>

const bool playground=true;


int main(int argc, const char * argv[])
{

	@autoreleasepool {
		NSURL* scriptURL=[NSURL URLWithString:playground?
			@"/Users/robbiemarkwick/Desktop/XPINS/XPINSTest/PlayGround.XPINS":
			@"/Users/robbiemarkwick/Desktop/XPINS/XPINSTest/TestScript.XPINS"];
		NSString* script=[NSString stringWithContentsOfURL:scriptURL encoding:NSASCIIStringEncoding error:nil];
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


