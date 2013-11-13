//
//  main.cpp
//  XPINSTest
//
//  Created by Robbie Markwick on 11/12/13.
//
//

#include <iostream>
#include "XPINSCompiler.h"
#include "XPINSBridge.h"
#include "XPINSParser.h"

int main(int argc, const char * argv[])
{
	string script="@COMPILER[0.3]\n";
	script+="@PARSER[0.6]\n";
	script+="@FUNC\n";
	script+="VOID PRINTNUM(INT);\n";
	script+="VOID PRINTLN();\n";
	script+="@END\n";
	script+="@CODE\n";
	script+="INT $X=^1;\n";
	script+="@WHILE[?B($X<=^10)?]{\n";
	script+="#PRINTNUM($X);\n";
	script+="$X=?I($X+^1)?;\n";
	script+="}\n";
	script+="@END\n";
	cout<<"Uncompiled Script:\n"<<script;
	bool result=XPINSCompiler::compileScript(&script);
	if (result) {
		cout<<"\n\n\nCompiled Script:\n"<<script<<endl<<endl;
		cout<<"RUNNING SCRIPT:\n\n";
		XPINSParams *params=new XPINSParams();
		XPINSParser::parseScript(script, params, NULL,false,0,0);
	}

    return 0;
}

