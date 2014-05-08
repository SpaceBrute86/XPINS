//
//  XPINSCompiler.cpp
//  XPINS
//
//  Created by Robbie Markwick on 9/7/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "XPINSCompiler.h"
#include <vector>

using namespace std;

const int kMajor=0;
const int kMinor=5;

namespace XPINSCompileUtil {
	string strRepresentationOfInt(int);
	bool stringsMatch(int,string,string);
	int readVarIndex(string,int *,char);
	string nameForBuiltin(string);
}
//#define DEBUGGING

//Compile Script
bool XPINSCompiler::compileScript(string &input){
	string scriptText=input;
	if(!checkVersion(scriptText))return false;
	if(!removeComments(scriptText))return false;
	if(!checkConstants(scriptText))return false;
	if(!replaceConstants(scriptText))return false;
	if(!renameFunctions(scriptText))return false;
	if(!renameVars(scriptText))return false;
	if(!renameBuiltIns(scriptText))return false;
	if(!cleanUp(scriptText))return false;
	input=scriptText;
	return true;
}

//Compiler Steps
bool XPINSCompiler::checkVersion(string &script){
	string input=script;
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
			if(MINOR>kMinor){
				cout<<"INCOMPATIBLE VERSION. EXITING";
				return false;
			}
			while(i<input.length()&&input[i]!='\n')i++;
			i++;
			string output="";
			while (i<input.length()) {
				output+=input[i++];
			}
			script=output;
			return true;
		}
	}
	cout<<"SCRIPT MISSING VERSION. EXITING";
	return false;
}
bool XPINSCompiler::removeComments(string &text){
	string input=text;
	string output="";
	int matcount=0;
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
		else if((ch!=';'||matcount>0)&&ch!='\t') output+=ch;//No semicolons or tabs in compiled script
		else if(ch=='[')++matcount;
		else if(ch==']')--matcount;
		if(ch==';'&&matcount==0&&input[i+1]!='\n')output+='\n';
	}
	text=output;
	return true;
}
bool XPINSCompiler::replaceConstants(string &text){
	string input=text;
	//Initialize first intermediate
	string intermediate1="";
	string intermediate2="";
	int j=0;
	while (!XPINSCompileUtil::stringsMatch(j, input, "@CONST"))intermediate1+=input[j++];
	int i=j;
	while (!XPINSCompileUtil::stringsMatch(++j, input, "@FUNC"));
	for(;j<input.length();j++)intermediate1+=input[j];
	
	while (true) {
		string constName="~";
		while (input[i]!='\n'||input[i+1]=='\n')++i;
		if(XPINSCompileUtil::stringsMatch(i, input, "\n@END"))break;
		//process constant declaration
		while (input[++i]!='=')constName+=input[i];
		string constVal="";
		while (input[++i]!='\n')
		{
			constVal+=input[i];
			if(input[i]=='\"')
			{
				while (++i<input.length()) {
					constVal+=input[i];
					if(input[i]=='\\'&&(input[i+1]=='\\'||input[i+1]=='\"'))
					{
						constVal+=input[++i];
					}
					else if(input[i]=='\"')break;
				}
			}
		}
		//replace constant in script
			//Replace Var name
			intermediate2="";
			j=0;
			while (!XPINSCompileUtil::stringsMatch(j, intermediate1, "@CODE"))intermediate2+=intermediate1[j++];

			while(j<intermediate1.length()){
				//CHECK FOR @END
				if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
					intermediate2+="@END";
					break;
				}
				if(intermediate1[j]=='\"')
				{
					intermediate2+='\"';
					while (++j<intermediate1.length()) {
						intermediate2+=intermediate1[j];
						if(intermediate1[j]=='\\'&&(intermediate1[j+1]=='\\'||intermediate1[j+1]=='\"'))
						{
							intermediate2+=intermediate1[++j];
						}
						else if(intermediate1[j]=='\"')break;
					}
				}
				//Find start of Var
					//Check for match
				else if(XPINSCompileUtil::stringsMatch(j, intermediate1, constName)){
					intermediate2+=constVal;
					//Skip Var Name
					j+=constName.length();
				}
				else
				{
					intermediate2+=intermediate1[j];
					j++;
				}
			}
			intermediate1=""+intermediate2;
	}
	text=intermediate1;
	return true;
}

string renameFuncBlock(string input, string intermediate1, int modNum, int blockStart)
{
	int i=blockStart;
	string intermediate2="";
	int j=0;
	//Do the actual work
	int x=1;
	while(!XPINSCompileUtil::stringsMatch(i, input, "\n@END")){
		while(i<input.length()&&input[i++]!='\n');
		if(XPINSCompileUtil::stringsMatch(i, input, "@END")||i>=input.length())break;
		char functionType=input[i++];
		switch (functionType) {
			case 'B':
			case 'b':
				functionType='B';
				break;
			case 'N':
			case 'n':
				functionType='N';
				break;
			case 'M':
			case 'm':
				functionType='M';
				break;
			case 'S':
			case 's':
				functionType='S';
				break;
			case '*':
				functionType='P';
				break;
			case 'A':
				functionType='A';
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
		while (i<input.length()&&input[i++]!=' ');
		//read function name
		string functionName;
		for(j=i;j<input.length()&&input[j]!='(';j++){
			functionName+=input[j];
		}
		//if (functionName.length()>0) {
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
			while(intermediate1.length()>j&&intermediate1[j]!='#'){
				intermediate2+=intermediate1[j];
				j++;
			}
			intermediate2+=intermediate1[j];
			j++;
			//Check for match
			if(XPINSCompileUtil::stringsMatch(j, intermediate1, functionName+'(')){
				if(functionType!=' ')intermediate2+=functionType;//add function types to user functions.
				intermediate2+='M';
				intermediate2+=XPINSCompileUtil::strRepresentationOfInt(modNum);
				intermediate2+='F';
				intermediate2+=XPINSCompileUtil::strRepresentationOfInt(x);
				while(intermediate1[j]!='('){//Find '('
					j++;
				}
			}
		}
		//Get ready for next loop
		intermediate1=""+intermediate2;
		//	}
		++x;
		while (i<input.length()&&input[++i]!='\n');
	}
	return intermediate1;
}


bool XPINSCompiler::renameFunctions(string &text){
	string input=text;
	string output="";
	//Initialize first intermediate
	string intermediate1="";
	int j=0;
	while (!XPINSCompileUtil::stringsMatch(j, input, "@FUNC"))intermediate1+=input[j++];
	while (!XPINSCompileUtil::stringsMatch(++j, input, "@CODE"));
	for(;j<input.length();j++)intermediate1+=input[j];
	
	//Process Local Function block
	int i=0;
	while(!XPINSCompileUtil::stringsMatch(++i, input, "@FUNC"));
	i+=5;
	intermediate1=renameFuncBlock(input, intermediate1,0, i);
	while(!XPINSCompileUtil::stringsMatch(++i, input, "@END"));
	
	//Process Module blocks
	for (int modNum=1;;++modNum) {
		while(!XPINSCompileUtil::stringsMatch(i, input, "@MODULE")&&
			  !XPINSCompileUtil::stringsMatch(i, input, "@CODE"))++i;
		if (XPINSCompileUtil::stringsMatch(i, input, "@CODE")) break;
		i+=7;
		intermediate1=renameFuncBlock(input, intermediate1,modNum, i);
		while(!XPINSCompileUtil::stringsMatch(++i, input, "@END"));
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
	text=output;
	return true;
}
bool XPINSCompiler::renameBuiltIns(string &text){
	string input=text;
	string output="";
	for(int i=0;i<input.length();++i){
		output+=input[i];
		if(input[i]=='X'&&input[i+1]=='_'){//found built-in
			i+=2;
			string name="";
			while (input[i]!='(') name+=input[i++];
			output+=XPINSCompileUtil::nameForBuiltin(name);
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
	text=output;
	return true;
}
bool XPINSCompiler::renameVars(string &text){
	string input=text;
	string intermediate="";
	//Rename Types
	for(int i=0;i<input.length();i++){
		if((input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='M'||input[i]=='S'||input[i]=='*')&&input[i-1]=='\n'){
			intermediate+=input[i];
			while (input[++i]!=' ');
		}
		intermediate+=input[i];
	}
	input=intermediate;
	string output="";
	int i=0;
	//Locate Code block
	while(input[i]!='@'||input[i+1]!='C'){
		i++;
	}
	if(input[i+1]!='C'||input[i+2]!='O'||input[i+3]!='D'||input[i+4]!='E'){
		cerr<<"Invalid Script: Missing @CODE.\nEXITING";
		return false;
	}
	i+=5;
	//Initialize intermediates
	string intermediate1=""+input;
	string intermediate2="";
	int j=0;
	//Do the actual work
	int xb=0;
	int xn=0;
	int xv=0;
	int xm=0;
	int xs=0;
	int xp=0;
	int xa=0;
	char varType='P';
	while(i<intermediate1.length()&&(intermediate1[i]!='@'||intermediate1[i+1]!='E')){
		while (i<intermediate1.length()&&intermediate1[i++]!='\n');
		if(intermediate1[i]=='B'||intermediate1[i]=='N'||intermediate1[i]=='V'||intermediate1[i]=='M'||intermediate1[i]=='S'||intermediate1[i]=='*'||intermediate1[i]=='M')
		{
			//determine new variable name
			varType=intermediate1[i]=='*'?'P':intermediate1[i];
			int varNum=0;
			switch (varType) {
				case 'B':
					varNum=xb++;
					break;
				case 'N':
					varNum=xn++;
					break;
				case 'V':
					varNum=xv++;
					break;
				case 'M':
					varNum=xm++;
					break;
				case 'S':
					varNum=xs++;
					break;
				case 'P':
					varNum=xp++;
					break;
				case 'A':
					varNum=xa++;
					break;
				default: varType='P';varNum=++xp;
			}
			while (intermediate1[i++]!='$');
			//read Var name
			string varName;
			while(i<intermediate1.length()&&intermediate1[i]!='='&&intermediate1[i]!=' '){
				varName+=intermediate1[i++];
			}
			//Replace Var name
			intermediate2="";
		//	cout<<"\n\n\n\n"<<intermediate1;
			j=-1;
			while(true){
				//CHECK FOR @END
				if(++j>=intermediate1.length()||(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'))
				{
					intermediate2+="@END";
					break;
				}
				if(intermediate1[j-1]=='\n'&&(intermediate1[j]=='B'||intermediate1[j]=='N'||intermediate1[j]=='V'||intermediate1[j]=='M'||intermediate1[j]=='S'||intermediate1[j]=='*'||intermediate1[j]=='A')&&XPINSCompileUtil::stringsMatch(j+3, intermediate1, varName))
				{
					j+=2;
				}
				if(intermediate1[j-1]=='~'&&intermediate1[j]=='\"')
				{
					intermediate2+='\"';
					while (++j<intermediate1.length()) {
						intermediate2+=intermediate1[j];
						if(intermediate1[j]=='\\'&&(intermediate1[j+1]=='\\'||intermediate1[j+1]=='\"'))
						{
							intermediate2+=intermediate1[++j];
						}
						else if(intermediate1[j]=='\"')break;
					}
				}
				//Find start of Var
				else if(intermediate1[j]=='$')
				{
					intermediate2+='$';
					//Check for match
					if(XPINSCompileUtil::stringsMatch(j+1, intermediate1, varName)){
						intermediate2+=varType;
						intermediate2+=XPINSCompileUtil::strRepresentationOfInt(varNum);
						//Skip Var Name
						j+=varName.length();
					}
				}
				else
				{
					intermediate2+=intermediate1[j];
				}
			}
			intermediate1=""+intermediate2;
		}
		i++;
	}
	//Add VarSize Statements
	intermediate2="";
	i=0;
	while (intermediate1[i++]!='@');
	intermediate2+='@';
	while (intermediate1[i]!='@')intermediate2+=intermediate1[i++];
	string varSizes="@VAR ";
	varSizes+='B'+XPINSCompileUtil::strRepresentationOfInt(xb)+' ';
	varSizes+='N'+XPINSCompileUtil::strRepresentationOfInt(xn)+' ';
	varSizes+='V'+XPINSCompileUtil::strRepresentationOfInt(xv)+' ';
	varSizes+='M'+XPINSCompileUtil::strRepresentationOfInt(xm)+' ';
	varSizes+='S'+XPINSCompileUtil::strRepresentationOfInt(xs)+' ';
	varSizes+='P'+XPINSCompileUtil::strRepresentationOfInt(xp)+' ';
	varSizes+='A'+XPINSCompileUtil::strRepresentationOfInt(xa)+' ';
	intermediate2+=varSizes+'\n';
	while (i<intermediate1.length()) intermediate2+=intermediate1[i++];
	intermediate1=intermediate2;
	//Remove Types
	intermediate2="";
	for(int i=0;i<intermediate1.length();++i){
		if((intermediate1[i]=='B'||intermediate1[i]=='N'||intermediate1[i]=='V'||intermediate1[i]=='M'||intermediate1[i]=='S'||intermediate1[i]=='*'||intermediate1[i]=='A')&&intermediate1[i-1]=='\n'){
			while (intermediate1[++i]!='$');
		}
		intermediate2+=intermediate1[i];
	}
	intermediate1=intermediate2;
	//Double Check Ending (strip after @END)
	for(j=0;j<intermediate1.length();j++){
		if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
			output+="@END\n";
			break;
		}
		output+=intermediate1[j];
	}
	text=output;
	return true;
}
bool XPINSCompiler::checkConstants(string& input)
{
	string output="";
	for(int i=0;i<input.length();++i)
	{
		output+=input[i];
		if(input[i]=='\"')
		{
			while (++i<input.length()) {
				output+=input[i];
				if(input[i]=='\\'&&(input[i+1]=='\\'||input[i+1]=='\"'))
				{
					output+=input[++i];
				}
				else if(input[i]=='\"')break;
			}
		}
		//If it is a character that could be followed by an input
		if(input[i]=='('||input[i]=='='||input[i]==','||input[i]=='['||(input[i]=='{'&&input[i-1]=='~')||input[i]=='<'||//typical operations
		   input[i]=='|'||input[i]=='&'||input[i]=='>'||input[i]=='!'||input[i]=='+'||input[i]=='-'||input[i]=='*'||input[i]=='/'||input[i]=='%')//Expression Specific
			
		{
			while (input[i+1]==' ')++i;
			if(input[i+1]!='$'&&input[i+1]!='#'&&input[i+1]!='?'&&input[i+1]!='X'&&input[i+1]!='~'
			   &&input[i+1]!='='&&input[i+1]!='+'&&input[i+1]!='-'&&input[i+1]!=')'
			   &&(input[i-1]!='R'||input[i]!='[')&&input[i+1]!='\n')
				output+='~';
		}
	}
	input=output;
	return true;
}

bool XPINSCompiler::cleanUp(string &text){
	string input=text;
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
	text=output;
	return true;
}
//UTILTY FUNCTIONS
string XPINSCompileUtil::nameForBuiltin(string name){
	if(name.compare("AND")==0)return "B0";
	if(name.compare("OR")==0)return "B1";
	if(name.compare("NOT")==0)return "B2";
	if(name.compare("LESS")==0)return "B3";
	if(name.compare("MORE")==0)return "B4";
	if(name.compare("EQUAL")==0)return "B5";
	
	if(name.compare("ADD")==0)return "N0";
	if(name.compare("SUB")==0)return "N1";
	if(name.compare("MULT")==0)return "N2";
	if(name.compare("DIV")==0)return "N3";
	if(name.compare("TSIN")==0)return "N4";
	if(name.compare("TCOS")==0)return "N5";
	if(name.compare("TTAN")==0)return "N6";
	if(name.compare("TATAN")==0)return "N7";
	if(name.compare("POW")==0)return "N8";
	if(name.compare("VADDPOLAR")==0)return "N9";
	if(name.compare("VDIST")==0)return "N10";
	if(name.compare("VX")==0)return "N11";
	if(name.compare("VY")==0)return "N12";
	if(name.compare("VZ")==0)return "N13";
	if(name.compare("VR")==0)return "N14";
	if(name.compare("VTHETA")==0)return "N15";
	if(name.compare("VRHO")==0)return "N16";
	if(name.compare("VPHI")==0)return "N17";
	if(name.compare("VDOT")==0)return "N18";
	if(name.compare("VANG")==0)return "N19";
	if(name.compare("MGET")==0)return "N20";
	if(name.compare("MDET")==0)return "N21";
	if(name.compare("MOD")==0)return "N22";
	if(name.compare("LN")==0)return "N23";
	if(name.compare("LOG")==0)return "N24";
	if(name.compare("ABS")==0)return "N25";
	if(name.compare("FLOOR")==0)return "N26";
	if(name.compare("PRAND")==0)return "N27";
	if(name.compare("PBERN")==0)return "N28";
	if(name.compare("PNORMAL")==0)return "N29";
	if(name.compare("PEXP")==0)return "N30";
	if(name.compare("PPOISSON")==0)return "N31";

	if(name.compare("VREC")==0)return "V0";
	if(name.compare("VPOL")==0)return "V1";
	if(name.compare("VSHPERE")==0)return "V2";
	if(name.compare("VADD")==0)return "V3";
	if(name.compare("VSUB")==0)return "V4";
	if(name.compare("VSCALE")==0)return "V5";
	if(name.compare("VPROJ")==0)return "V6";
	if(name.compare("VCROSS")==0)return "V7";
	if(name.compare("MMTV")==0)return "V8";
	if(name.compare("MVMULT")==0)return "V9";
	
	if(name.compare("MMAKE")==0)return "M0";
	if(name.compare("MID")==0)return "M1";
	if(name.compare("MROT")==0)return "M2";
	if(name.compare("MADD")==0)return "M3";
	if(name.compare("MSUB")==0)return "M4";
	if(name.compare("MSCALE")==0)return "M5";
	if(name.compare("MMULT")==0)return "M6";
	if(name.compare("MINV")==0)return "M7";
	if(name.compare("MTRANS")==0)return "M8";
	if(name.compare("MVTM")==0)return "M9";
	
	if(name.compare("PRINT")==0)return "_0";
	if(name.compare("MSET")==0)return "_1";
	
	return "_"+name;
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
	if(x<0)return "0";
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
	for(int i=0;i<sec.length();++i)
		if(i+start>=first.length()||
		   first[start+i]!=sec[i])
			return false;
	return true;
}
