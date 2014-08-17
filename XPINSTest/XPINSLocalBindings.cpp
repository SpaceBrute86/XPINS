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
using namespace XPINSScriptableMath;
/*include your own headers as needed*/

/*Implement your custom Functions Functions*/







//PRIMARY METHOD

void* XPINSLocalBindings::BindFunction(int fNum,XPINSArray args){
	switch (fNum) {
			/*Bind Your Custom Functions*/
		case 1:{
			int num=*args.numAtIndex(0);
			cout<<num<<"\t\t";
		}break;
		case 2:{
			cout<<endl;
		}break;
		case 3:{
			Matrix m=*args.matAtIndex(0);
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