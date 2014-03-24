//
//  XPINSBridge.cpp
//  XPINS
//
//  Created by Robbie Markwick on 8/29/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "XPINSBridge.h"
#include "XPINSScriptableMath.h"
using namespace std;
/*include your own headers*/

/*Write your own Scriptable Functions*/







//PRIMARY METHOD

void XPINSBridge::BridgeFunction(int fNum,string script,XPINSParams* parameters,XPINSParser::varSpace* vars,int* index,void* rett){
	/*Declare Pointers to Your Custom Functions*/

	/*Bridge Your Custom Functions*/
	switch (fNum) {
		case 1: cout<<XPINSParser::ParseIntArg(script, parameters, vars, index, ')')<<"\t";
		break;
		case 2: cout<<endl;break;
	}
}