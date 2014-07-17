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

void* XPINSLocalBindings::BindFunction(int fNum,XPINSParser::XPINSScriptSpace& script,vector<Argument>args){
	switch (fNum) {
			/*Bind Your Custom Functions*/
		case 1:{
			int num=*XPINSParser::ParseNumArg(script, args[0]);
			cout<<num<<"\t\t";
		}break;
		case 2:{
			cout<<endl;
		}break;
		case 3:{
			XPINSScriptableMath::Matrix m=*XPINSParser::ParseMatArg(script, args[0]);
			for (int i=0; i<m.GetRows(); ++i) {
				for (int j=0; j<m.GetCols(); ++j) {
					cout<<m.ValueAtPosition(i, j)<<"\t\t";
				}
				cout<<'\n';
			}
		}break;
	}
	return NULL;
}