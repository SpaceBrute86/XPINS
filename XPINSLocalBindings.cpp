//
//  XPINSLocalBindings.cpp
//  XPINS
//
//  Created by Robbie Markwick on 3/27/14.
//
//

#include "XPINSLocalBindings.h"
#include "XPINSScriptableMath.h"
using namespace std;
/*include your own headers as needed*/

/*Implement your custom Functions Functions*/







//PRIMARY METHOD

void* XPINSLocalBindings::BindFunction(int fNum,XPINSParser::XPINSScriptSpace& script){
	switch (fNum) {
			/*Bind Your Custom Functions*/
		case 1:{
			int num=*XPINSParser::ParseNumArg(script, ')');
			cout<<num<<"\t";
		}break;
		case 2:{
			cout<<endl;
		}break;
	}
	return NULL;
}