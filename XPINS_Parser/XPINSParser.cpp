//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission


#include "XPINSParser.h"
#include "XPINSBridge.h"
#include <math.h>


using namespace std;
using namespace XPINSParser;
using namespace XPINSScriptableMath;
const int kPMajor=0;
const int kPMinor=9;


//Helper functions

//Read an int constant
//Can be used to parse script as well (accessing variable indexes)
// scriptText: the script
// startIndex: the starting index of the INT
// expectedEnd: the character expected immediately after the INT
int readInt(string scriptText,int *startIndex,char expectedEnd)
{
	int i=*startIndex;
	int index=0;
	bool isNeg=scriptText[i]=='-';//Make negative if approrpriate
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd)
	{
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	if(isNeg)index*=-1;
	*startIndex=i;
	return index;
}
//read a float constant
// scriptText: the script
// startIndex: the starting index of the FLOAT
// expectedEnd: the character expected immediately after the FLOAT
float readFloat(string scriptText,int *startIndex,char expectedEnd)
{
	int i=*startIndex;
	int index=0;
	int fpartDig=0;
	bool fpart=false;
	bool isNeg=scriptText[i]=='-';
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd){
		if(fpart)fpartDig++;//record decimal place
		index*=10;
		if(scriptText[i]=='1')index+=1;
		else if(scriptText[i]=='2')index+=2;
		else if(scriptText[i]=='3')index+=3;
		else if(scriptText[i]=='4')index+=4;
		else if(scriptText[i]=='5')index+=5;
		else if(scriptText[i]=='6')index+=6;
		else if(scriptText[i]=='7')index+=7;
		else if(scriptText[i]=='8')index+=8;
		else if(scriptText[i]=='9')index+=9;
		else if(scriptText[i]=='.')fpart=true;//Start recording decimal places
		else if(scriptText[i]!='0')index/=10;
		i++;
	}
	while (fpartDig) {//put decimal point in correct place
		index/=10;
		fpartDig--;
	}
	if(isNeg)index*=-1;
	*startIndex=i;
	return index;
}
//Read Variable Index for Function Parameters
int readFuncParameter(string scriptText,int *startIndex,char varType,char expectedEnd){
	int i=*startIndex;
	++i;
	if (scriptText[i]!='$'||scriptText[i+1]!=varType) {
		// printf("\nERROR:INVALID SCRIPT:INVALID PARAMETER FORMAT!\n");
		return 0;
	}
	i+=2;
	int index=readInt(scriptText, &i, expectedEnd);
	*startIndex=i;
	return index;
}



//parameter parsing
bool XPINSParser::ParseBoolArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	bool retVal=false;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if (i>=scriptText.length()) {
		return false;
	}
	if(scriptText[i]=='$'){//Variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->bVars[index];
	}
	else if(scriptText[i]=='^'){//constant
		retVal=(scriptText[i+1]=='T');
	}
	else if(scriptText[i]=='?'){//expression
		retVal=XPINSBuiltIn::ParseBoolExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='B'||scriptText[i+2]!='F'){
			retVal=false;
		}
		else{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText, params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='B'){
			retVal=false;
		}
		else retVal=XPINSBuiltIn::ParseBoolBIF(scriptText, params, vars, &i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
int XPINSParser::ParseIntArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	int retVal=0;
	while (i<scriptText.length()&&scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->iVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readInt(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=XPINSBuiltIn::ParseIntExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='I'||scriptText[i+2]!='F'){
			retVal=NULL;
		}
		else{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='I'){
			retVal=0;
		}
		else retVal=XPINSBuiltIn::ParseIntBIF(scriptText, params, vars, &i);		
	}
	while(i<scriptText.length()&&scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
float XPINSParser::ParseFloatArg(string scriptText,XPINSParams* params, varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	float retVal=0;
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->fVars[index];
	}
	else if(scriptText[i]=='^'){
		++i;
		retVal=readFloat(scriptText, &i, expectedEnd);
	}
	else if(scriptText[i]=='?'){
		retVal=XPINSBuiltIn::ParseFloatExp(scriptText, params,vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='F'||scriptText[i+2]!='F'){
			retVal=NULL;
		}
		else{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='F'){
			retVal=0;
		}
		else retVal=XPINSBuiltIn::ParseFloatBIF(scriptText, params, vars, &i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
Vector XPINSParser::ParseVecArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){

	int i=*start;
	Vector retVal=Vector();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->vVars[index];
	}
	else if(scriptText[i]=='^'){//constant (can contain varialbes, though)
		++i;
		if(scriptText[i]=='P'&&scriptText[i+1]=='<'){//Polar Vector
				i+=2;
				float r=ParseFloatArg(scriptText, params,vars, &i, ',');
				float t=ParseFloatArg(scriptText, params,vars, &i, ',');
				float z=ParseFloatArg(scriptText, params,vars, &i, '>');
				retVal=Vector::PolarVector(r, t, z);
		}
		else if(scriptText[i]=='S'&&scriptText[i+1]=='<'){//Polar Vector
			i+=2;
			float r=ParseFloatArg(scriptText, params,vars, &i, ',');
			float t=ParseFloatArg(scriptText, params,vars, &i, ',');
			float p=ParseFloatArg(scriptText, params,vars, &i, '>');
			retVal=Vector::PolarVector(r, t ,p);
		}
		else if(scriptText[i]=='<'){//rectangular vector
			++i;
			float x=ParseFloatArg(scriptText, params,vars, &i, ',');
			float y=ParseFloatArg(scriptText, params,vars, &i, ',');
			float z=ParseFloatArg(scriptText, params,vars, &i, '>');
			retVal=Vector(x, y,z);
		}
		else{
			retVal=Vector();
		}
	}
	else if(scriptText[i]=='?'){
		retVal=XPINSBuiltIn::ParseVecExp(scriptText, params, vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]=='V'&&scriptText[i+2]=='F')
		{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='V'){
			retVal=Vector();
		}
		else retVal=XPINSBuiltIn::ParseVecBIF(scriptText, params, vars, &i);

		
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}

Matrix XPINSParser::ParseMatArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	
	int i=*start;
	Matrix retVal=Matrix();
	while (scriptText[i]!='$'&&scriptText[i]!='^'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->mVars[index];
	}
	else if(scriptText[i]=='^'){//constant (can contain varialbes, though)
		while (scriptText[i]!='[') ++i;
		size_t cols=1;
		for (int j=i; scriptText[j]!=';'&&scriptText[j]!=']'; ++j) {
			if(scriptText[j]==',')++cols;
		}
		size_t rows=1;
		for (int j=i; scriptText[j]!=']'; ++j) {
			if(scriptText[j]==';')++cols;
		}
		Matrix m=Matrix(rows,cols);
		int r=0,c=0;
		while (scriptText[i]!=']')
		{
			if(c==cols-1){
				if(r==rows-1)m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ']'),r,c);
				else m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ';'),r,c);
				++r;
				c=-1;
			}
			else m.SetValueAtPosition(ParseFloatArg(scriptText, params, vars, &i, ','),r,c);
			++c;
			++i;
		}
		return m;
	}
	else if(scriptText[i]=='?'){
		retVal=XPINSBuiltIn::ParseMatExp(scriptText, params, vars, &i);
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]=='M'&&scriptText[i+2]=='F')
		{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	else if(scriptText[i]=='X'){//Built-in Function
		i+=3;
		if(scriptText[i-1]!='_'||scriptText[i-2]!='M'){
			retVal=Matrix();
		}
		else retVal=XPINSBuiltIn::ParseMatBIF(scriptText, params, vars, &i);
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}

void* XPINSParser::ParsePointerArg(string scriptText,XPINSParams* params,varSpace* vars,int* start,char expectedEnd){
	int i=*start;
	void* retVal=NULL;
	while (scriptText[i]!='$'&&scriptText[i]!='#') ++i;
	if(scriptText[i]=='$'){//variable
		i+=2;
		int index=readInt(scriptText, &i, expectedEnd);
		retVal=vars->pVars[index];
	}
	else if(scriptText[i]=='#'){//User-defined Function
		if(scriptText[i+1]!='P'||scriptText[i+2]!='F'){
		}
		else{
			i+=3;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,params, vars,  &i, &retVal);
		}
	}
	while(scriptText[i]!=expectedEnd)++i;
	*start=i;
	return retVal;
}
//Check to make sure script is compatible
// script: the script
bool checkVersion(string script){
	for(int i=0;i<script.length();i++){
		if(script[i]=='@'&&script[i+1]=='P'&&script[i+2]=='A'&&script[i+3]=='R'&&script[i+4]=='S'&&script[i+5]=='E'&&script[i+6]=='R'){
			while(script[i]!='[')i++;
			int MAJOR=readInt(script, &i, '.');
			int MINOR=readInt(script, &i, ']');
			if(MAJOR!=kPMajor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			if(MINOR>kPMinor){
				cout<<"INCOMPATIBLE VERSION. FAILING";
				return false;
			}
			return true;
		}
	}
	cout<<"SCRIPT MISSING VERSION. FAILING";
	return false;
}
//Primary Function
//See header for parameter descriptions
void XPINSParser::ParseScript(string scriptText,XPINSParams *parameters,varSpace *vars,bool isRECURSIVE,int start,int stop){
//	cout<<scriptText.length()<<endl<<endl;
	//SET UP VAR SPACE
	bool initialized_varSpace=false;
	if(vars==NULL){
		vars=new varSpace;
		vars->bVars=vector<bool>();
		vars->iVars=vector<int>();
		vars->fVars=vector<float>();
		vars->vVars=vector<Vector>();
		vars->pVars=vector<void*>();
		initialized_varSpace=true;
	}
	//RUN SCRIPT
	int i=0;//index of char in script
	if(isRECURSIVE)i=start;
	//Validate start of script
	if(!isRECURSIVE){
		if(!checkVersion(scriptText))return;
		while (scriptText[i]!='\n')i++;
		while(scriptText[i]!='@')i++;
		if(scriptText[i+1]!='C'||
		   scriptText[i+2]!='O'||
		   scriptText[i+3]!='D'||
		   scriptText[i+4]!='E'){
			printf("\nERROR:INVALID SCRIPT:MISSING @CODE!\n");
			return;
		}
		++i;
	}
	while(true){
		//get to next line
		while(i<scriptText.length()&&scriptText[i++]!='\n'){}
		if (i>=scriptText.length())break;
		//cout<<scriptText[i];
		//reaching the end of the Script
		if(!(scriptText[i]!='@'||
			scriptText[i+1]!='E'||
			scriptText[i+2]!='N'||
			 scriptText[i+3]!='D')||
		   (isRECURSIVE&&i>=stop)){
			break;
		}
		//Declaring new vars
		if(scriptText[i]=='B'){
			vars->bVars.resize(vars->bVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if(scriptText[i]=='I'){
			vars->iVars.resize(vars->iVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='F'){
			vars->fVars.resize(vars->fVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='V'){
			vars->vVars.resize(vars->vVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		else if( scriptText[i]=='*'){
			vars->pVars.resize(vars->pVars.size()+1);
			while(scriptText[i]!='$')i++;
		}
		if(scriptText[i]=='$'){
			i++;
			if(scriptText[i]=='B'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->bVars[index]=ParseBoolArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='I'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->iVars[index]=ParseIntArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='F'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->fVars[index]=ParseFloatArg(scriptText, parameters,vars, &i, '\n');
			}
			else if(scriptText[i]=='V'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->vVars[index]=ParseVecArg(scriptText, parameters,vars, &i,'\n');
			}
			else if(scriptText[i]=='P'){
				i++;
				int index=readInt(scriptText, &i, '=');
				i++;
				vars->pVars[index]=ParsePointerArg(scriptText, parameters, vars, &i,'\n');
			}
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='#'){
			if (scriptText[i+1]!='F') {
				printf("\nERROR:INVALID SCRIPT:NOT A FUNCTION NAME!\n");
			}
			i+=2;
			int fNum=readInt(scriptText, &i, '(');
			XPINSBridge::BridgeFunction(fNum, scriptText,parameters, vars,  &i, 0);
			if(i>=scriptText.length())break;
		}
		else if(scriptText[i]=='X'){
			i+=3;
			if(scriptText[i-1]!='_'||scriptText[i-2]!='N')
				XPINSBuiltIn::ParseVoidBIF(scriptText, parameters, vars, &i);
		}
		else if(scriptText[i]=='@'){
			i++;
			//IF STATEMENT
			if(scriptText[i]=='I'&&scriptText[i+1]=='F'){
				while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') i++;
				if(scriptText[i+1]!='B'&&scriptText[i]!='^'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING IF/ELSE\n");
					while (scriptText[i]!='{')i++;
					--i;
					int num=0;
					bool found=false;
					//skip if block
					while(!found||num>0){
						if(i>=scriptText.length())break;
						if(scriptText[i]=='{'){
							num++;found=true;}
						else if(scriptText[i]=='}')
							num--;
						i++;
					}
					while (scriptText[i]!='\n')i++;
					//skiip Else
					if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
						while (scriptText[i]!='{')i++;
						--i;
						int num=0;
						bool found=false;
						//skip if block
						while(!found||num>0){
							if(i>=scriptText.length())break;
							if(scriptText[i]=='{'){
								num++;found=true;}
							else if(scriptText[i]=='}')
								num--;
							i++;
						}
					}
				}
				else{
					if(!ParseBoolArg(scriptText, parameters, vars, &i, ']')){
						while (scriptText[i]!='{')i++;
						--i;
						int num=0;
						bool found=false;
						//skip if block
						while(!found||num>0){
							if(i>=scriptText.length())break;
							if(scriptText[i]=='{'){
								num++;found=true;}
							else if(scriptText[i]=='}')
								num--;
							i++;
						}
						while (scriptText[i]!='\n')i++;
						++i;
						//execute else if applicable
						if(scriptText[i]=='@'&&scriptText[i+1]=='E'&&scriptText[i+2]=='L'&&scriptText[i+3]=='S'&&scriptText[i+4]=='E'){
							while (scriptText[i]!='{')i++;
						}
					}
					else{
						while (scriptText[i]!='{')i++;
					}
				}
			}
			//WHILE LOOP
			else if(scriptText[i]=='W'&&scriptText[i+1]=='H'&&scriptText[i+2]=='I'&&scriptText[i+3]=='L'&&scriptText[i+4]=='E'){
				while (scriptText[i]!='$'&&scriptText[i]!='?'&&scriptText[i]!='^'&&scriptText[i]!='#'&&scriptText[i]!='X') i++;
				if(scriptText[i+1]!='B'&&scriptText[i]!='^'){
					printf("WARNING:VARIABLE NOT BOOL, SKIPPING WHILE LOOP");
				}
				else{
					int expIndex=i--;
					while (scriptText[++i]!='{'){};
					int loopStart=i;
					int num=0;
					bool found=false;
					//skip if block
					while(!found||num>0){
						if(i>=scriptText.length())break;
						if(scriptText[i]=='{'){
							num++;found=true;}
						else if(scriptText[i]=='}')
							num--;
						i++;
					}
					int loopStop=i;
				//	for(int k=loopStart;k<=loopStop;++k)cout<<scriptText[k];
					int temp=expIndex;
					while (ParseBoolArg(scriptText, parameters, vars, &expIndex, ']')) {
						expIndex=temp;
						ParseScript(scriptText, parameters, vars, true, loopStart, loopStop);
					}
				}
			}
			//ELSE (BYPASS because IF was executed)
			else if(scriptText[i]=='E'&&scriptText[i+1]=='L'&&scriptText[i+2]=='S'&&scriptText[i+3]=='E'){
				while (scriptText[i]!='{')i++;
				--i;
				int num=0;
				bool found=false;
				//skip if block
				while(!found||num>0){
					if(i>=scriptText.length())break;
					if(scriptText[i]=='{'){
						num++;found=true;}
					else if(scriptText[i]=='}')
						num--;
					i++;
				}
				
			}
			if(i>=scriptText.length())break;
		}
	}
	//wipe declared variables
	if(initialized_varSpace) {
		vars->bVars.resize(0);
		vars->iVars.resize(0);
		vars->fVars.resize(0);
		vars->vVars.resize(0);
		vars->pVars.resize(0);
		delete vars;
	}
}