//
//  XPINSCompiler.cpp
//  XPINS
//
//  Created by Robbie Markwick on 9/7/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "XPINSCompiler.h"
using namespace std;

const int kMajor=0;
const int kMinor=3;

namespace XPINSCompileUtil {
	string strRepresentationOfInt(int);
	bool stringsMatch(int,string,string);
	int readVarIndex(string,int *,char);
	char charForBuiltin(string);
}

//Compile Script
bool XPINSCompiler::compileScript(string* input){
	string scriptText=*input;
//	cout<<endl<<"Validating Version..."<<endl;
	if(!checkVersion(&scriptText))return false;
//	cout<<"Version Compatilbe!";
//	cout<<endl<<"About To Compile Script:\n"<<scriptText<<endl;
	if(!removeComments(&scriptText))return false;
//	cout<<endl<<"Comments and ';'s Removed:\n"<<scriptText<<endl;
	if(!renameFunctions(&scriptText))return false;
//	cout<<endl<<"User Functions Renamed:\n"<<scriptText<<endl;
	if(!renameTypes(&scriptText))return false;
//	cout<<endl<<"Variable Types Renamed:\n"<<scriptText<<endl;
	if(!renameVars(&scriptText))return false;
//	cout<<endl<<"Variables Renamed:\n"<<scriptText<<endl;
	if(!renameBuiltIns(&scriptText))return false;
//	cout<<endl<<"Built-in Functions Renamed:\n"<<scriptText<<endl;
	if(!cleanUp(&scriptText))return false;
//	cout<<endl<<"Cleaned Up:\n"<<scriptText;
	*input=scriptText;
	return true;
}

//Compiler Steps
bool XPINSCompiler::checkVersion(string* script){
	string input=*script;
	for(int i=0;i<input.length();i++){
		if(input[i]=='@'&&input[i+1]=='C'&&input[i+2]=='O'&&input[i+3]=='M'&&input[i+4]=='P'&&input[i+5]=='I'&&input[i+6]=='L'&&input[i+7]=='E'&&input[i+8]=='R'){
			while(i<input.length()&&input[i]!='[')i++;
			if(i==input.length()){
				cout<<"SCRIPT MISSING VERSION. EXITING";
				return false;;
			}
			int MAJOR=XPINSCompileUtil::readVarIndex(input, &i, '.');
			int MINOR=XPINSCompileUtil::readVarIndex(input, &i, ']');
			if(MAJOR!=kMajor){
				cout<<"INCOMPATIBLE VERSION. EXITING";
				return false;
			}
			if(MINOR<kMinor){
				cout<<"INCOMPATIBLE VERSION. EXITING";
				return false;
			}
			while(i<input.length()&&input[i]!='\n')i++;
			i++;
			string output="";
			while (i<input.length()) {
				output+=input[i++];
			}
			*script=output;
			return true;
		}
	}
	cout<<"SCRIPT MISSING VERSION. EXITING";
	return false;
}
bool XPINSCompiler::removeComments(string* text){
	string input=*text;
	string output="";
	char ch;
	for(int i=0;i<input.length();i++){
		ch=input[i];
		if(ch=='/'){// all '/'s will be removed, any meaningful ones denote a comment
			i++;
			ch=input[i];
			if(ch=='*'){//multi line comments
				while (input[i]!='*'||input[i+1]!='/')i++;
				i++;
			}
			else if(ch=='/'){// single line comments
				while (input[i+1]!='\n'||i+1==input.length())i++;
			}
		}
		else if(ch!=';') output+=ch;//No semicolons :(
	}
	bool hitFirstEND=false;
	string intermediate1=""+output;
	output="";
	//Double Check Ending (strip after 2nd @END)
	for(int j=0;j<intermediate1.length();j++){
		if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
			if(hitFirstEND){
				output+="@END\n";
				break;
			}
			else hitFirstEND=true;
		}
		output+=intermediate1[j];
	}
	*text=output;
	return true;
}

bool XPINSCompiler::renameFunctions(string *text){
	string input=*text;
	string output="";
	char ch;
	string intermediate1="";
	string intermediate2="";
	//Locate Function block
	int i=0;
	while(input[i]!='@'||input[i+1]!='F'){
		intermediate1+=input[i++];
	}
	if(input[i+1]!='F'||input[i+2]!='U'||input[i+3]!='N'||input[i+4]!='C'){
		cerr<<"Invalid Script: Missing @FUNC.\nEXITING";
		return false;
	}
	i+=5;

	//Initialize first intermediate
	int j=0;
	while (input[j]!='@'||input[j+1]!='C') {
		j++;
		
	}
	for(;j<input.length();j++){
		ch=input[j];
		intermediate1+=ch;
	}
	//Do the actual work
	int x=1;
	while(input[i+1]!='@'){
		if(i==input.length()){
			cerr<<"Invalid Script: Missing @END.\nEXITING";
			return false;;
		}
		while(input[i++]!='\n'){}
		char functionType=input[i++];
		switch (functionType) {
			case 'B':
			case 'b':
				functionType='B';
				break;
			case 'I':
			case 'i':
				functionType='I';
				break;
			case 'F':
			case 'f':
				functionType='F';
				break;
			case '*':
				functionType='P';
				break;
			case 'V':
			case 'v':
				if (input[i]=='E'||input[i]=='e') {
					functionType='V';
					break;
				}
			default:
				functionType=' ';
				break;
		}
		while (input[i]!=' '){
			i++;
		}
		i++;
		//read function name
		string functionName;
		for(j=i;j<input.length()&&input[j]!='(';j++){
			functionName+=input[j];
		}
		//Replace function name
		intermediate2="";
		j=0;
		while(true){
			//Get to next line/END
			while(j<intermediate1.length()&&intermediate1[j]!='\n'){
				intermediate2+=intermediate1[j];
				j++;
			}
			//IF END CLEAN UP
			if(j>=intermediate1.length()){
				intermediate2+="@END";
				break;
			}
			intermediate2+=intermediate1[j];
			j++;
			//CHECK FOR @END
			if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
				intermediate2+="@END";
				break;
			}
			//Find start of function
			while(intermediate1[j]!='#'){
				intermediate2+=intermediate1[j];
				j++;
			}
			intermediate2+=intermediate1[j];
			j++;
			//Check for match
			if(XPINSCompileUtil::stringsMatch(j, intermediate1, functionName)){
				if(functionType!=' ')intermediate2+=functionType;//add function types to user functions.
				intermediate2+='F';
				intermediate2+=XPINSCompileUtil::strRepresentationOfInt(x);
				while(intermediate1[j]!='('){//Find '('
					j++;
				}
			}
		}
		//Get ready for next loop
		intermediate1=""+intermediate2;
		++x;
		while (input[i]!='\n')i++;
	}
	//Double Check Ending (strip after @END)
	for(j=0;j<intermediate1.length();j++){
		if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
			output+="@END";
			break;
		}
		output+=intermediate1[j];
	}
	//output=intermediate1;
	*text=output;
	return true;
}
bool XPINSCompiler::renameBuiltIns(string *text){
	string input=*text;
	string output="";
	for(int i=0;i<input.length();++i){
		output+=input[i];
		if(input[i]=='X'&&input[i+1]=='_'){//found built-in
			i+=2;
			string name="";
			while (input[i]!='(') name+=input[i++];
			output+=XPINSCompileUtil::charForBuiltin(name);
			output+='_';
			output+=name;
			output+='(';
		}
	}
	string inter=""+output;
	output="";
	//Double Check Ending (strip after @END)
	for(int j=0;j<inter.length();j++){
		if(inter[j]=='@'&&inter[j+1]=='E'&&inter[j+2]=='N'&&inter[j+3]=='D'){
			output+="@END";
			break;
		}
		output+=inter[j];
	}
	*text=output;
	return true;
}
bool XPINSCompiler::renameTypes(string *text){
	string input=*text;
	string output="";
	string intermediate1="";
	//Rename Types
	for(int i=0;i<input.length();i++){
		if((input[i]=='B'||input[i]=='I'||input[i]=='F'||input[i]=='V'||input[i]=='*')&&input[i-1]=='\n'){
			intermediate1+=input[i];
			while (input[i]!=' ')i++;
		}
		intermediate1+=input[i];
	}
	//Double Check Ending (strip after @END)
	for(int i=0;i<intermediate1.length();i++){
		if(intermediate1[i]=='@'&&intermediate1[i+1]=='E'&&intermediate1[i+2]=='N'&&intermediate1[i+3]=='D'){
			output+="@END\n";
			break;
		}
		output+=intermediate1[i];
	}
	*text=output;
	return true;
}
bool XPINSCompiler::renameVars(string *text){
	string input=*text;
	string output="";
	int i=0;
	//Locate Code block
	while(input[i]!='@'||input[i+1]!='C'){
		i++;
	}
	if(input[i+1]!='C'||input[i+2]!='O'||input[i+3]!='D'||input[i+4]!='E'){
		cerr<<"Invalid Script: Missing @CODE.\nEXITING";
		return false;;
	}
	i+=5;
	//Initialize intermediates
	string intermediate1=""+input;
	string intermediate2="";
	int j=0;
	//Do the actual work
	int xb=-1;
	int xi=-1;
	int xf=-1;
	int xv=-1;
	int xp=-1;
	char varType='P';
	while(i<input.length()&&(input[i]!='@'||input[i+1]!='E')){
		if(input[i-1]=='\n'&&(input[i]=='B'||input[i]=='I'||input[i]=='F'||input[i]=='V'||input[i]=='*'))
		{
			//determine new variable name
			varType=input[i]=='*'?'P':input[i];
			int varNum=0;
			switch (varType) {
				case 'B':
					varNum=++xb;
					break;
				case 'I':
					varNum=++xi;
					break;
				case 'F':
					varNum=++xf;
					break;
				case 'V':
					varNum=++xv;
					break;
				case 'P':
					varNum=++xp;
					break;
				default: varType='P';varNum=++xp;
			}
			//read Var name
			string varName;
			for(i+=3;i<input.length()&&input[i]!='=';i++){
				varName+=input[i];
			}
			//Replace Var name
			intermediate2="";
			j=0;
			int blockCount=0;
			while(j<intermediate1.length()){
				//CHECK FOR @END
				if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
					intermediate2+="@END";
					break;
				}
				//Check for block end
				if(intermediate1[j]=='{')blockCount++;
				else if(intermediate1[j]=='}'){
					if(blockCount==0){
						while(!(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D')&&j+3<intermediate1.length())intermediate2+=intermediate1[j];
						intermediate2+="@END";
						break;
					}
					else blockCount--;
					
				}
				//Find start of Var
				if(intermediate1[j]=='$'){
					intermediate2+='$';
					j++;
					//Check for match
					if(XPINSCompileUtil::stringsMatch(j, intermediate1, varName)){
						intermediate2+=varType;
						intermediate2+=XPINSCompileUtil::strRepresentationOfInt(varNum);
						while(intermediate1[j]!=')'&&intermediate1[j]!='='&&intermediate1[j]!=','&&intermediate1[j]!='<'&&intermediate1[j]!='>'&&intermediate1[j]!='|'&&intermediate1[j]!='&'&&intermediate1[j]!='!'&&intermediate1[j]!='+'&&intermediate1[j]!='-'&&intermediate1[j]!='*'&&intermediate1[j]!='/'&&intermediate1[j]!='%'&&intermediate1[j]!=']'){//Find '('
							j++;
						}
					}
				}
				else{
					intermediate2+=intermediate1[j];
					j++;
				}
			}
			intermediate1=""+intermediate2;
		}
		i++;
	}
	//Double Check Ending (strip after @END)
	for(j=0;j<intermediate1.length();j++){
		if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
			output+="@END\n";
			break;
		}
		output+=intermediate1[j];
	}
	*text=output;
	return true;
}
bool XPINSCompiler::cleanUp(string *text){
	string input=*text;
	string output="";
	string intermediate1="";
	//Remove excess whitespace
	for(int i=0;i<input.length();i++){
		if((input[i]!='\n'||input[i+1]!='\n')&&input[i]!='\t')intermediate1+=input[i];
	}
	//Double Check Ending (strip after @END)
	for(int i=0;i<intermediate1.length();i++){
		if(intermediate1[i]=='@'&&intermediate1[i+1]=='E'&&intermediate1[i+2]=='N'&&intermediate1[i+3]=='D'){
			output+="@END\n";
			break;
		}
		output+=intermediate1[i];
	}
	*text=output;
	return true;
}
//UTILTY FUNCTIONS
char XPINSCompileUtil::charForBuiltin(string name){
	if(name.compare("AND")==0)return 'B';
	if(name.compare("OR")==0)return 'B';
	if(name.compare("NOT")==0)return 'B';
	if(name.compare("ILESS")==0)return 'B';
	if(name.compare("IMORE")==0)return 'B';
	if(name.compare("FLESS")==0)return 'B';
	if(name.compare("FMORE")==0)return 'B';
	if(name.compare("IEQUAL")==0)return 'B';
	if(name.compare("FEQUAL")==0)return 'B';
	if(name.compare("IADD")==0)return 'I';
	if(name.compare("ISUB")==0)return 'I';
	if(name.compare("IMULT")==0)return 'I';
	if(name.compare("IDIV")==0)return 'I';
	if(name.compare("IMOD")==0)return 'I';
	if(name.compare("FADD")==0)return 'F';
	if(name.compare("FSUB")==0)return 'F';
	if(name.compare("FMULT")==0)return 'F';
	if(name.compare("FDIV")==0)return 'F';
	if(name.compare("TSIN")==0)return 'F';
	if(name.compare("TCOS")==0)return 'F';
	if(name.compare("TTAN")==0)return 'F';
	if(name.compare("TATAN")==0)return 'F';
	if(name.compare("VADDPOLAR")==0)return 'F';
	if(name.compare("POW")==0)return 'F';
	if(name.compare("VDIST")==0)return 'F';
	if(name.compare("VX")==0)return 'F';
	if(name.compare("VY")==0)return 'F';
	if(name.compare("VMAG")==0)return 'F';
	if(name.compare("VDIR")==0)return 'F';
	if(name.compare("VANG")==0)return 'F';
	if(name.compare("VDOT")==0)return 'F';
	if(name.compare("VREC")==0)return 'V';
	if(name.compare("VPOL")==0)return 'V';
	if(name.compare("VADD")==0)return 'V';
	if(name.compare("VSUB")==0)return 'V';
	if(name.compare("VSCALE")==0)return 'V';
	if(name.compare("VPROJ")==0)return 'V';
	return 'K';
}
int XPINSCompileUtil::readVarIndex(string scriptText,int *startIndex,char expectedEnd){
	int i=*startIndex;
	int index=0;
	while(scriptText[i]!=expectedEnd){
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
	*startIndex=i;
	return index;
}
string XPINSCompileUtil::strRepresentationOfInt(int x){
	if(x/10==0){
		switch (x) {
			case 1:return "1";
			case 2:return "2";
			case 3:return "3";
			case 4:return "4";
			case 5:return "5";
			case 6:return "6";
			case 7:return "7";
			case 8:return "8";
			case 9:return "9";
			case 0:return "0";
		}
	}
	else return strRepresentationOfInt(x/10)+strRepresentationOfInt(x%10);
	return "";
}
bool XPINSCompileUtil::stringsMatch(int start,string first, string sec){
	for(int i=start;i<start+sec.length();i++){
		if(i==first.length()) return false;
		if(first[i]!=sec[i-start])return false;
	}
	return true;
}
