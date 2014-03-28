//
//  XPINSBridge.cpp
//  XPINS
//
//  Created by Robbie Markwick on 8/29/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "XPINSBindings.h"
#include "XPINSScriptableMath.h"
using namespace std;
/*include your own headers*/

/*Write your own Scriptable Functions*/







//PRIMARY METHOD

void XPINSBridge::BridgeFunction(int fNum,string script,XPINSParser::XPINSDataRef data,int& i,void* returnVal){
	/*Declare Pointers to Your Custom Functions*/

	/*Bridge Your Custom Functions*/
	switch (fNum) {
		case 1: cout<<XPINSParser::ParseIntArg(script, data, i, ')')<<"\t";
		break;
		case 2: cout<<endl;break;
	}
}