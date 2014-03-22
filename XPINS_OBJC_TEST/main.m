//
//  main.m
//  XPINS_OBJC_TEST
//
//  Created by Robbie Markwick on 11/22/13.
//
//

#import <Foundation/Foundation.h>
#import "XPNCompiler.h"
int main(int argc, const char * argv[])
{

	@autoreleasepool {
	    
	    // insert code here...
	 //   NSLog(@"Hello, World!");
	    NSString *script=@"@COMPILER[0.3]\n@PARSER[0.6]\n@FUNC\nVOID PRINTNUM(INT);\nVOID PRINTLN();\n@END\n@CODE\nINT $X=^1;\n@WHILE[?B($X<=^10)?]{\n\tINT $Y=^1;\n\t@WHILE[?B($Y<=^10)?]{\n\t\tPRINTNUM(?I($X*$Y)?);\n\t\t$Y=?I($Y+^1)?;\n\t}\n\tPRINTLN();\n\t$X=?I($X+^1)?;\n}\n@END\n";
		NSLog(@"%@\n\n\n\n",script);
		NSString *runScript=[XPNCompiler compileScript:script];
		NSLog(@"%@\n\n\n\n",runScript);

	}
    return 0;
}

