//
//  XPINSCompiler.cpp
//  XPINS
//
//  Created by Robbie Markwick on 9/7/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include "XPINSCompiler.h"
#include <vector>
#include <list>
#include "XPINSBIFMap.h"

using namespace std;

const int kMajor=0;
const int kMinor=7;


namespace XPINSCompileUtil
{
	string strRepresentationOfInt(int);
	bool stringsMatch(int,string,string);
	int readVarIndex(string,int *,char);
}
bool XPINSCompiler::compileScript(string &input)//Compile Script
{
	string scriptText=input;
	if(!checkVersion(scriptText))return false;
	if(!removeComments(scriptText))return false;
	if (!convertDotSyntax(scriptText))return false;
	if(!replaceConstants(scriptText))return false;
	if(!checkConstantSyntax(scriptText))return false;
	if(!renameFunctions(scriptText))return false;
	if(!renameVars(scriptText))return false;
	if(!cleanUp(scriptText))return false;
	input=scriptText;
	return true;
}

#pragma mak Compiler Steps

bool XPINSCompiler::checkVersion(string &script)
{
	string input=script;
	for(int i=0;i<input.length();++i)
	{
		if(XPINSCompileUtil::stringsMatch(i, input, "@COMPILER"))
		{
			while(i<input.length()&&input[i]!='[')++i;
			if(i==input.length())  return false;
			//Check Version
			int MAJOR=XPINSCompileUtil::readVarIndex(input, &i, '.');
			int MINOR=XPINSCompileUtil::readVarIndex(input, &i, ']');
			if(MAJOR!=kMajor||MINOR>kMinor)
			{
				return false;
			}
			//Copy Rest of Script
			while(i<input.length()&&input[i]!='\n')i++;
			i++;
			string output="";
			while (i<input.length())  output+=input[i++];
			script=output;
			return true;
		}
		else if(input[i]=='@')break;
	}
	return false;
}
bool XPINSCompiler::removeComments(string &text)
{
	string input=text;
	string output="";
	for(int i=0;i<input.length();++i){
		if(input[i]=='\"')//Copy Strings
		{
			output+='\"';
			while (++i<input.length()) {
				output+=input[i];
				if(input[i]=='\\'&&(input[i+1]=='\\'||input[i+1]=='\"'))
				{
					output+=input[++i];
				}
				else if(input[i]=='\"')break;
			}
		}
		else if(input[i]=='/'&&(input[i+1]=='/'||input[i+1]=='*'))//Comment
		{
			++i;
			if(input[i]=='*')//multi line comments
			{
				int commentCount=1;
				while (commentCount>0) {
					++i;
					if (XPINSCompileUtil::stringsMatch(i, input, "/*")){++commentCount;++i;}
					else if (XPINSCompileUtil::stringsMatch(i, input, "*/")){--commentCount;++i;}
				}
			}
			else // single line comments
			{
				while (input[i+1]!='\n'||i+1==input.length())++i;
			}
		}
		else if(input[i]>='a'&&input[i]<='z') output+=input[i]-('a'-'A');
		else if(input[i]!=';'&&input[i]!='\t') output+=input[i];//No semicolons or tabs in compiled script
		if(input[i]==';'&&input[i+1]!='\n')output+='\n';//Replace semicolons with newlines
	}
	text=output;
	return true;
}
bool XPINSCompiler::convertDotSyntax(string& input){
	string output="";
	int i=0;
	while(!XPINSCompileUtil::stringsMatch(i, input, "@CODE"))output+=input[i++];//Skip @PARSER
	for(;i<input.length();++i)
	{
		output+=input[i];
		if(input[i]=='\"')//Copy Strings
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
		if((input[i+1]=='.'||(input[i+1]=='-'&&input[i+2]=='>'))&&(input[i+2]<'0'||input[i+2]>'9'))//Skip completed Dot operations
		{
			i+=2;
			while (++i<input.length()) {
				if(input[i]=='('||input[i]==','||input[i]==')'||input[i]=='\n'||input[i]=='&'||input[i]=='|'||input[i]=='!'||input[i]=='<'||input[i]=='>'||input[i]=='='||input[i]=='+'||input[i]=='-'||input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%'||input[i]==']')break;
			}
			if (input[i]!='(') {
				output+=')';
				--i;
			} else if (input[i+1]!=')') output+=',';
		}
		//If it is a character that could be followed by an input
		else if((input[i]=='('||input[i]=='='||input[i]==','||input[i]=='['||(input[i]=='{'&&input[i-1]=='~')||
		   input[i]=='<'||input[i]==' '||input[i]=='\n'||input[i]=='|'||input[i]=='&'||input[i]=='>'||input[i]=='!'||(input[i]=='+'&&input[i+1]!='+')||(input[i]=='-'&&input[i+1]!='-')||input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%')&& input[i+1]!='='&&input[i+1]!=')'&& input[i+1]!='\n')
		{
			int j=i+1;
			bool isDotSyntax=false;
			bool isElementalSyntax=false;
			for (; j<input.length(); ++j) {
				if (input[j]==','||input[j]==')'||input[j]=='\n'||input[j]=='&'||input[j]=='|'||input[j]=='!'||input[j]=='<'||input[j]=='>'||input[j]=='='||input[j]=='+'||(input[j]=='-'&&input[j+1]!='>')||input[j]=='*'||input[j]=='/'||input[j]=='^'||input[j]==':'||input[j]=='%'||input[j]==']')break;
				else if ((input[j]=='-'&&input[j+1]=='>')&&(input[j+2]<'0'||input[j+2]>'9')){
					isDotSyntax=true;
					isElementalSyntax=true;
					break;
				}
				else if (input[j]=='.'&&(input[j+1]<'0'||input[j+1]>'9')){
					isDotSyntax=true;
					break;
				}
				else if(input[j]=='\"')//Skip Strings
				{
					while (++j<input.length()) {
						if(input[j]=='\\'&&(input[j+1]=='\\'||input[j+1]=='\"'))++j;
						else if(input[j]=='\"')break;
					}
				}
				else if(input[j]=='(')//Skip Parentheses
				{
					int parenCount=1;
					while (parenCount>0) {
						++j;
						if(input[j]=='\"')//Skip Strings
						{
							while (++j<input.length()) {
								if(input[j]=='\\'&&(input[j+1]=='\\'||input[j+1]=='\"'))++j;
								else if(input[j]=='\"')break;
							}
						}
						else if(input[j]=='(') ++parenCount;
						else if(input[j]==')')--parenCount;
								
						}
				}
				else if(input[j]=='[')//skip square brackets
				{
					int parenCount=1;
					while (parenCount>0) {
						++j;
						if(input[j]=='\"')//Skip Strings
						{
							while (++j<input.length()) {
								if(input[j]=='\\'&&(input[j+1]=='\\'||input[j+1]=='\"'))++j;
								else if(input[j]=='\"')break;
							}
						}
						else if(input[j]=='[') ++parenCount;
						else if(input[j]==']')--parenCount;
						
					}
				}
			}
			if (isDotSyntax) {
				
			//	vector<bool> elemental=vector<bool>(1,isElementalSyntax);
			//	vector<string> funcNames=vector<string>(1);
				list<string> funcNames=list<string>(0);
				list<bool> elemental=list<bool>(1,isElementalSyntax);
				string currentFuncName="";
				if(isElementalSyntax)++j;
				while (++j<input.length()) {
					if (input[j]=='.') {
						funcNames.push_back(currentFuncName);
						elemental.push_back(false);
						currentFuncName="";
						++j;
					}
					else if (input[j]=='-'&&input[j+1]=='>') {
						funcNames.push_back(currentFuncName);
						elemental.push_back(true);
						currentFuncName="";
						j+=2;
					}
					else if (input[j]=='('){
						int parenCount=1;
						while (parenCount>0) {
							++j;
							if(input[j]=='\"')//Skip Strings
							{
								while (++j<input.length()) {
									if(input[j]=='\\'&&(input[j+1]=='\\'||input[j+1]=='\"'))++j;
									else if(input[j]=='\"')break;
								}
							}
							else if(input[j]=='(') ++parenCount;
							else if(input[j]==')')--parenCount;
						}
						if(input[++j]=='.'){
							funcNames.push_back(currentFuncName);
							elemental.push_back(false);
							currentFuncName="";
							++j;
						}
						else if (input[j]=='-'&&input[j+1]=='>') {
							funcNames.push_back(currentFuncName);
							elemental.push_back(true);
							currentFuncName="";
							j+=2;
						}
						else break;
					}
					else if (input[j]==','||input[j]==')'||input[j]=='\n'||input[j]=='&'||input[j]=='|'||input[j]=='!'||input[j]=='<'||input[j]=='>'||input[j]=='='||input[j]=='+'||input[j]=='-'||input[j]=='*'||input[j]=='/'||input[j]=='^'||input[j]==':'||input[j]=='%'||input[j]==']')break;
					currentFuncName+=input[j];
				}
				funcNames.push_back(currentFuncName);
				while (funcNames.size()>0) {
					output+=funcNames.front();
					output+=elemental.front()?".(":"(";
					funcNames.pop_front();
					elemental.pop_front();
				}
			}
		}
	}
	input=output;
	return true;

}
bool XPINSCompiler::checkConstantSyntax(string& input)
{
	string output="";
	int i=0;
	while(!XPINSCompileUtil::stringsMatch(i, input, "@CODE"))output+=input[i++];//Skip @PARSER
	for(;i<input.length();++i)
	{
		output+=input[i];
		if(input[i]=='\"')//Copy Strings
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
		if(input[i]=='('||input[i]=='='||input[i]==','||input[i]=='['||(input[i]=='{'&&input[i-1]=='~')||
		   input[i]=='<'||input[i]==' '||input[i]=='\n'||//typical operations and Constants
		   input[i]=='|'||input[i]=='&'||input[i]=='>'||input[i]=='!'||input[i]=='+'||input[i]=='-'||
		   input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%')//Expression Specific
		{
			while (input[i+1]==' ')++i;
			//Check for following:
			if(input[i+1]!='?'&&input[i+1]!='@'&&//Already given Key character
			   input[i+1]!='='&&input[i+1]!='+'&&(input[i+1]!='-'||input[i]!='-')&&input[i+1]!=')'&&//Multi-char Expression Operators
			   input[i+1]!='\n')//end of line
			{
				bool isConstant=false;
				if (input[i]!='\n' && input [i] !=']')switch (input[i+1]) {
					case 'P':
					case 'S':
					case 'Q':
						if (input[i+2]!='<')break;
					case 'F':
					case 'T':
						if (!(input[i+2]==',' || input [i+2] == ')'|| input [i+2] == '\n'||input [i+2] == ' '||input [i+2] == '{'|| input [i+2] == '&'|| input [i+2] == '|'|| input [i+2] == '!')|| input[i+2] == '<') break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '-':
					case '.':
					case '<':
					case '[':
					case '(':
					case '\"':
					case '{':
						isConstant=true;
				}
				if (isConstant) output+='~';
				else if((input[i+1]>='A'&&input[i+1]<='Z')||(input[i+1]>='a'&&input[i+1]<='z')||input[i+1]=='_'){
					bool isVar=false;
					for (int j=i+1; j<input.length(); ++j) {
						if (input[j]=='(')break;
						if (input[j]==','||input[j]==')'||input[j]=='&'||input[j]=='|'||input[j]=='!'||input[j]=='<'||input[j]=='>'||input[j]=='='||input[j]=='+'||input[j]=='-'||input[j]=='*'||input[j]=='/'||input[j]=='^'||input[j]==':'||input[j]=='%'||input[j]==']') {
							isVar=true;
							break;
						}
					}
					if (isVar) output+="$_";
					else {
						string name="";
						for (; input[i+1]!='('; ++i) if(input[i+1]!='.')name+=input[i+1];
						if(name.compare("MARKOV_REACHABLE")==0)output+= "XB"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_REACHABLE);
						//Number Functions
						else if(name.compare("SIN")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_SIN);
						else if(name.compare("COS")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_COS);
						else if(name.compare("TAN")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_TAN);
						else if(name.compare("CSC")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_CSC);
						else if(name.compare("SEC")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_SEC);
						else if(name.compare("COT")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_COT);
						else if(name.compare("ASIN")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ASIN);
						else if(name.compare("ACOS")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ACOS);
						else if(name.compare("ATAN")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ATAN);
						else if(name.compare("ACSC")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ACSC);
						else if(name.compare("ASEC")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ASEC);
						else if(name.compare("ACOT")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ACOT);
						else if(name.compare("ATAN2")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ATAN2);
						else if(name.compare("ADDPOLAR")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ADDPOLAR);
						else if(name.compare("SQRT")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_SQRT);
						else if(name.compare("LN")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_LN);
						else if(name.compare("LOG")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_LOG);
						else if(name.compare("ABS")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ABS);
						else if(name.compare("FLOOR")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_FLOOR);
						else if(name.compare("DIST")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_DIST);
						else if(name.compare("X")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_X);
						else if(name.compare("Y")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_Y);
						else if(name.compare("Z")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_Z);
						else if(name.compare("MAGNITUDE")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MAGNITUDE);
						else if(name.compare("R")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_R);
						else if(name.compare("THETA")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_THETA);
						else if(name.compare("PHI")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_PHI);
						else if(name.compare("ANGLE_BETWEEN_VECTORS")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_ANGLE_BETWEEN_VECTORS);
						else if(name.compare("DETERMINANT")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_DETERMINANT);
						else if(name.compare("RAND")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_RAND);
						else if(name.compare("RV_BERNOUILLI")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_RV_BERNOULLI);
						else if(name.compare("RV_NORMAL")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_RV_NORMAL);
						else if(name.compare("RV_EXP")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_RV_EXP);
						else if(name.compare("RV_POISSON")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_RV_POISSON);
						else if(name.compare("COIN_FLIP")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_COIN_FLIP);
						else if(name.compare("DICE_ROLL")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_DICE_ROLL);
						else if(name.compare("MARKOV_SIM")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_SIM);
						else if(name.compare("MARKOV_PROB")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_PROB);
						else if(name.compare("MARKOV_STEADYSTATE")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_STEADYSTATE);
						else if(name.compare("MARKOV_ABSORB_PROB")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_ABSORB_PROB);
						else if(name.compare("MARKOV_ABSORB_TIME")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_ABSORB_TIME);
						else if(name.compare("MARKOV_ABSORB_SIM")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_MARKOV_ABSORB_SIM);
						else if(name.compare("SIZE")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_SIZE);
						else if(name.compare("EVALUATE")==0)output+= "XN"+XPINSCompileUtil::strRepresentationOfInt(X_EVALUATE);
						//Vector Functions
						else if(name.compare("PROJECT_ONTO_VECTOR")==0)output+= "XV"+XPINSCompileUtil::strRepresentationOfInt(X_PROJECT_ONTO_VECTOR);
						else if(name.compare("UNIT_VECTOR")==0)output+= "XV"+XPINSCompileUtil::strRepresentationOfInt(X_UNIT_VECTOR);
						else if(name.compare("V")==0)output+= "XV"+XPINSCompileUtil::strRepresentationOfInt(X_V);
						else if(name.compare("ROTATE_VECTOR")==0)output+= "XV"+XPINSCompileUtil::strRepresentationOfInt(X_ROTATE_VECTOR);
						else if(name.compare("VECTOR_EVALUATE")==0)output+= "XV"+XPINSCompileUtil::strRepresentationOfInt(X_VECTOR_EVALUATE);
						//Quaternion Functions
						else if(name.compare("CONJUGATE")==0)output+= "XQ"+XPINSCompileUtil::strRepresentationOfInt(X_CONJUGATE);
						else if(name.compare("INVERSE")==0)output+= "XQ"+XPINSCompileUtil::strRepresentationOfInt(X_INVERSE);
						else if(name.compare("UNIT_QUATERNION")==0)output+= "XQ"+XPINSCompileUtil::strRepresentationOfInt(X_UNIT_QUATERNION);
						//Matrix Functions
						else if(name.compare("ZERO_MATRIX")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_ZERO_MATRIX);
						else if(name.compare("IDENTITY_MATRIX")==0)output+="XM"+XPINSCompileUtil::strRepresentationOfInt(X_IDENTITY_MATRIX);
						else if(name.compare("ROTATION_MATRIX")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_ROTATION_MATRIX);
						else if(name.compare("EULER_ANGLE_MATRIX")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_EULER_ANGLE_MATRIX);
						else if(name.compare("QUATERNION_MATRIX")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_QUATERNION_MATRIX);
						else if(name.compare("INVERT")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_INVERT);
						else if(name.compare("TRANSPOSE")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_TRANSPOSE);
						else if(name.compare("APPEND")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_APPEND);
						else if(name.compare("ROW_ECHELON")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_ROW_ECHELON);
						else if(name.compare("REDUCED_ROW_ECHELON")==0)output+= "XM"+XPINSCompileUtil::strRepresentationOfInt(X_REDUCED_ROW_ECHELON);
						//Polynomial Functions
						else if(name.compare("DERIVE")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_DERIVE);
						else if(name.compare("INTEGRATE")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_INTEGRATE);
						else if(name.compare("DIVERGENCE")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_DIVERGENCE);
						else if(name.compare("LINE_INTEGRAL")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_LINE_INTEGRAL);
						else if(name.compare("SURFACE_INTEGRAL")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_SURFACE_INTEGRAL);
						else if(name.compare("VOLUME_INTEGRAL")==0)output+= "XP"+XPINSCompileUtil::strRepresentationOfInt(X_VOLUME_INTEGRAL);
						//Vector Field Functions
						else if(name.compare("COMPONENT_DERIVE")==0)output+= "XF"+XPINSCompileUtil::strRepresentationOfInt(X_COMPONENT_DERIVE);
						else if(name.compare("COMPONENT_INTEGRATE")==0)output+= "XF"+XPINSCompileUtil::strRepresentationOfInt(X_COMPONENT_INTEGRATE);
						else if(name.compare("GRADIENT_VECTOR")==0)output+= "XF"+XPINSCompileUtil::strRepresentationOfInt(X_GRADIENT_VECTOR);
						else if(name.compare("CURL")==0)output+= "XF"+XPINSCompileUtil::strRepresentationOfInt(X_CURL);
						//Void Functoins
						else if(name.compare("PRINT")==0)output+= "X_"+XPINSCompileUtil::strRepresentationOfInt(X_PRINT);
						else if(name.compare("PRINTF")==0)output+= "X_"+XPINSCompileUtil::strRepresentationOfInt(X_PRINTF);
						else if(name.compare("RESIZE")==0)output+= "X_"+XPINSCompileUtil::strRepresentationOfInt(X_RESIZE);
						else output+="#"+name;
						if (input[i]=='.') output+='.';
					}
				}
				
			}
		}
	}
	input=output;
	return true;
}
string renameConstant(string input, string constName, string constVal){
	string output="";
	int i=0;
	while (!XPINSCompileUtil::stringsMatch(i, input, "@CODE")) output+=input[i++];
	while(i<input.length())
	{
		//CHECK FOR @END
		if(XPINSCompileUtil::stringsMatch(i, input, "@END"))
		{
			output+="@END";
			break;
		}
		if(input[i]=='\"')//Skip String Constants
		{
			output+='\"';
			while (++i<input.length()) {
				if(input[i]=='\\'&&(input[i+1]=='\\'||input[i+1]=='\"'))
				{
					output+=input[i];
					output+=input[++i];
				}
				else if(input[i]=='\"')break;
				else output+=input[i];
			}
		}
		//Find start of Constant
		if(XPINSCompileUtil::stringsMatch(i, input, constName) && input[i+constName.length()]!='('&&(input[i+constName.length()]!='.'||input[i+constName.length()+1]!='(')
		   &&(input[i-1]=='('||input[i-1]=='='||input[i-1]==','||input[i-1]=='['||input[i-1]=='{'||
			input[i-1]=='<'||input[i-1]==' '||input[i-1]=='\n'||//typical operations and Constants
			input[i-1]=='|'||input[i-1]=='&'||input[i-1]=='>'||input[i-1]=='!'||input[i-1]=='+'||input[i-1]=='-'||
			  input[i-1]=='*'||input[i-1]=='/'||input[i-1]=='^'||input[i-1]==':'||input[i-1]=='%') &&(input[i+constName.length()]<'A'||input[i+constName.length()]>'Z')&&(input[i+constName.length()]<'0'||input[i+constName.length()]>'9')&&(input[i+constName.length()]!='_'))
		{
			output+=constVal;//Replace Constant
			i+=constName.length();
		}
		else
		{
			output+=input[i++];
		}
	}
	return output;
}

bool XPINSCompiler::replaceConstants(string &text)
{
	string input=text;
	//Initialize first intermediate
	string output="";
	int j=0;
	while (!XPINSCompileUtil::stringsMatch(j, input, "@CONST"))output+=input[j++];
	int i=j;
	while (!XPINSCompileUtil::stringsMatch(++j, input, "@FUNC"));//Skip to @Func Block
	for(;j<input.length();j++)output+=input[j];
	//Replace Constants
	while (true)
	{
		string constName="";
		while (input[i]!='\n'||input[i+1]=='\n')++i;
		if(XPINSCompileUtil::stringsMatch(i, input, "\n@END"))break;
		//Process constant declaration
		while (input[++i]!='=')constName+=input[i];
		string constVal="";
		while (input[++i]!='\n')//Get Constant Value
		{
			constVal+=input[i];
		}
		output=renameConstant(output, constName, constVal);
	}
	//Replace built in constants
	output=renameConstant(output, "TRUE", "T");
	output=renameConstant(output, "YES", "T");
	output=renameConstant(output, "FALSE", "F");
	output=renameConstant(output, "NO", "F");
	output=renameConstant(output, "PI", "3.141592653589793");
	output=renameConstant(output, "E", "2.718281828459045");

	text=output;
	return true;
}
//Does the heavy lifting for Function Renaming Function
//Input script (for reading block), intermediate (for renaming), module number (for assigning new name), and where to start reading the input
string renameFuncBlock(string input, string intermediate1, int modNum, int blockStart)
{
	int i=blockStart;
	string intermediate2="";
	int j=0;
	int x=1;
	while(!XPINSCompileUtil::stringsMatch(i, input, "\n@END"))
	{
		while(i<input.length()&&input[i++]!='\n');//Get to next line in Function Block
		if(XPINSCompileUtil::stringsMatch(i, input, "@END")||i>=input.length())break;
		//Read Function Type
		char functionType=input[i++];
		switch (functionType)
		{
			case 'B':
			case 'b':
				functionType='B';//BOOL
				break;
			case 'N':
			case 'n':
				functionType='N';//NUM
				break;
			case 'Q':
			case 'q':
				functionType='Q';//QUAT
				break;
			case 'M':
			case 'm':
				functionType='M';//MAT
				break;
			case 'P':
			case 'p':
				functionType='P';//POLY
				break;
			case 'F':
			case 'f':
				functionType='F';//FIELD
				break;
			case 'S':
			case 's':
				functionType='S';//STR
				break;
			case '*':
				functionType='O';//Object
				break;
			case 'A':
				functionType='A';//ARR
				break;
			case 'V':
			case 'v':
				if (input[i]=='E'||input[i]=='e')
				{
					functionType='V';//VEC
					break;
				}
			default:
				functionType=' ';//VOID
				break;
		}
		while (++i<input.length()&&input[i]!=' ');
		while (++i<input.length()&&input[i]==' ');
		//read function name
		string functionName="";
		for(j=i;j<input.length()&&input[j]!='('&&input[j]!='\n';j++)
		{
			functionName+=input[j];
		}
		//Replace function name
		intermediate2="";
		j=0;
		while(true){
			//Find start of function
			while(j<intermediate1.length()&&intermediate1[j]!='#'&&!XPINSCompileUtil::stringsMatch(j+1, intermediate1, "@END"))
			{
				intermediate2+=intermediate1[j++];
			}
			if(j>=intermediate1.length()||XPINSCompileUtil::stringsMatch(j+1, intermediate1, "@END"))
			{
				intermediate2+="\n@END";
				break;
			}
			if(intermediate1[j]=='#')
			{
				intermediate2+='#';
				j++;
				//Check for match
				if(XPINSCompileUtil::stringsMatch(j, intermediate1, functionName+'(')||XPINSCompileUtil::stringsMatch(j, intermediate1, functionName+".("))
				{
					if(functionType!=' ')intermediate2+=functionType;//add function types to user functions.
					intermediate2+='M';
					intermediate2+=XPINSCompileUtil::strRepresentationOfInt(modNum);//Module Number
					intermediate2+='F';
					intermediate2+=XPINSCompileUtil::strRepresentationOfInt(x);//Funciton Number
					while(intermediate1[j]!='(')++j;//Find '('
					if (intermediate1[j-1]=='.') intermediate2+='.';
				}
			}
		}
		//Get ready for next loop
		intermediate1=""+intermediate2;
		++x;
		while (i<input.length()&&input[++i]!='\n');
	}
	return intermediate1;
}
bool XPINSCompiler::renameFunctions(string &text){
	string input=text;
	//Initialize first intermediate
	string intermediate1="";
	int j=0;
	while (!XPINSCompileUtil::stringsMatch(j, input, "@FUNC"))intermediate1+=input[j++];
	while (!XPINSCompileUtil::stringsMatch(++j, input, "@CODE"));
	for(;j<input.length();j++)intermediate1+=input[j];
	//Process Local Function block
	int i=0;
	while(!XPINSCompileUtil::stringsMatch(i, input, "@FUNC"))++i;
	i+=5;
	intermediate1=renameFuncBlock(input, intermediate1,0, i);
	while(!XPINSCompileUtil::stringsMatch(++i, input, "@END"));
	//Process Module blocks
	for (int modNum=1;;++modNum)
	{
		while(!XPINSCompileUtil::stringsMatch(i, input, "@MODULE")&&
			  !XPINSCompileUtil::stringsMatch(i, input, "@CODE"))++i;
		if (XPINSCompileUtil::stringsMatch(i, input, "@CODE")) break;//Finished Module Processing
		i+=7;
		intermediate1=renameFuncBlock(input, intermediate1,modNum, i);
		while(!XPINSCompileUtil::stringsMatch(++i, input, "@END"));
	}
	text=intermediate1;
	return true;
}
bool XPINSCompiler::renameVars(string &text){
	string input=text;
	string intermediate="";
	//Rename Types
	int i=0;
	while (!XPINSCompileUtil::stringsMatch(i, input, "@CODE")) intermediate+=input[i++];
	for(;i<input.length();++i)
	{
		if((input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='Q'||input[i]=='M'||input[i]=='P'||input[i]=='F'||input[i]=='S'||input[i]=='O'||input[i]=='A')&&input[i-1]=='@')//Found Variable Declaration
		{
			intermediate+=input[i];
			while (input[++i]!=' ');
		}
		intermediate+=input[i];
	}
	input=intermediate;
	i=0;
	//Locate Code block
	while(i<input.length()&&!XPINSCompileUtil::stringsMatch(i, input, "@CODE"))++i;
	if(i>=input.length())
		return false;
	i+=5;
	//Initialize intermediates
	string intermediate1=""+input;
	string intermediate2="";
	int j=0;
	//Do the actual work
	int xb=0;
	int xn=0;
	int xv=0;
	int xq=0;
	int xm=0;
	int xs=0;
	int xp=0;
	int xf=0;
	int xa=0;
	int xo=0;
	char varType='O';
	while(i<input.length()&&!XPINSCompileUtil::stringsMatch(i, input, "@END"))
	{
		while (i<input.length()&&input[i++]!='@');//Get to next line
		if(input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='Q'||input[i]=='M'||input[i]=='S'||input[i]=='P'||input[i]=='F'||input[i]=='O'||input[i]=='A')//If we have a new variable type
		{
			//determine new variable name
			varType=input[i];
			int varNum=0;
			while (input[i++]!='$');
			if (input[i]=='_') {
				switch (varType)
				{
					case 'B':
						varNum=xb++;//BOOL
						break;
					case 'N':
						varNum=xn++;//NUM
						break;
					case 'V':
						varNum=xv++;//VEC
						break;
					case 'Q':
						varNum=xq++;//QUAT
						break;
					case 'M':
						varNum=xm++;//MAT
						break;
					case 'S':
						varNum=xs++;//STR
						break;
					case 'P':
						varNum=xp++;//POLY
						break;
					case 'F':
						varNum=xf++;//FIELD
						break;
					case 'A':
						varNum=xa++;//ARR
						break;
					default:
						varType='O';
						varNum=++xo;//Default to object
				}
				//read Var name
				string varName;
				while(i<input.length()&&input[i]!='='&&input[i]!=' ')
				{
					varName+=input[i++];
				}
				//Replace Var name
				intermediate2="";
				j=0;
				while(j<intermediate1.length()&&!XPINSCompileUtil::stringsMatch(j, intermediate1, "@END"))
				{
					//If we find a variable declaration, skip to variable
					if(intermediate1[j-1]=='\n'&&(intermediate1[j]=='B'||intermediate1[j]=='N'||intermediate1[j]=='V'||intermediate1[j]=='M'||intermediate1[j]=='S'||intermediate1[j]=='*'||intermediate1[j]=='A')&&XPINSCompileUtil::stringsMatch(j+3, intermediate1, varName))
					{
						j+=2;
					}
					if(intermediate1[j-1]=='~'&&intermediate1[j]=='\"')//Skip strings
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
						if(XPINSCompileUtil::stringsMatch(j+1, intermediate1, varName)&&
						   (intermediate1[j+1+varName.length()]=='='||intermediate1[j+1+varName.length()]==','||
							intermediate1[j+1+varName.length()]=='+'||intermediate1[j+1+varName.length()]=='-'||
							intermediate1[j+1+varName.length()]=='*'||intermediate1[j+1+varName.length()]=='/'||
							intermediate1[j+1+varName.length()]=='%'||intermediate1[j+1+varName.length()]=='^'||
							intermediate1[j+1+varName.length()]=='&'||intermediate1[j+1+varName.length()]=='|'||
							intermediate1[j+1+varName.length()]=='!'||intermediate1[j+1+varName.length()]=='<'||
							intermediate1[j+1+varName.length()]=='>'||intermediate1[j+1+varName.length()]==']'||
							intermediate1[j+1+varName.length()]=='['||intermediate1[j+1+varName.length()]==')'||
							intermediate1[j+1+varName.length()]==':'))
						{
							intermediate2+=varType;
							intermediate2+=XPINSCompileUtil::strRepresentationOfInt(varNum);
							j+=varName.length();
						}
					}
					else
					{
						intermediate2+=intermediate1[j];
					}
					++j;
				}
				intermediate2+="\n@END";
				intermediate1=""+intermediate2;
			}
		}
		++i;
	}
	//Add @VAR
	intermediate2="";
	i=0;
	while (intermediate1[i++]!='@');
	intermediate2+='@';
	while (intermediate1[i]!='@')intermediate2+=intermediate1[i++];
	string varSizes="@VAR ";
	varSizes+='B'+XPINSCompileUtil::strRepresentationOfInt(xb)+' ';
	varSizes+='N'+XPINSCompileUtil::strRepresentationOfInt(xn)+' ';
	varSizes+='V'+XPINSCompileUtil::strRepresentationOfInt(xv)+' ';
	varSizes+='Q'+XPINSCompileUtil::strRepresentationOfInt(xq)+' ';
	varSizes+='M'+XPINSCompileUtil::strRepresentationOfInt(xm)+' ';
	varSizes+='P'+XPINSCompileUtil::strRepresentationOfInt(xp)+' ';
	varSizes+='F'+XPINSCompileUtil::strRepresentationOfInt(xf)+' ';
	varSizes+='S'+XPINSCompileUtil::strRepresentationOfInt(xs)+' ';
	varSizes+='O'+XPINSCompileUtil::strRepresentationOfInt(xo)+' ';
	varSizes+='A'+XPINSCompileUtil::strRepresentationOfInt(xa)+' ';
	intermediate2+=varSizes+'\n';
	while (i<intermediate1.length()) intermediate2+=intermediate1[i++];
	intermediate1=intermediate2;
	//Remove Variable Types
	intermediate2="";
	i=0;
	while (!XPINSCompileUtil::stringsMatch(i,intermediate1,"@CODE")) {
		intermediate2+=intermediate1[i++];
	}
	for(;i<intermediate1.length();++i){
		if((intermediate1[i+1]=='B'||intermediate1[i+1]=='N'||intermediate1[i+1]=='V'||intermediate1[i+1]=='Q'||intermediate1[i+1]=='M'||intermediate1[i+1]=='P'||intermediate1[i+1]=='F'||intermediate1[i+1]=='S'||intermediate1[i+1]=='*'||intermediate1[i+1]=='A')&&intermediate1[i]=='@')
		{
			while (intermediate1[++i]!='$');
		}
		intermediate2+=intermediate1[i];
	}
	text=intermediate2;
	return true;
}
bool XPINSCompiler::cleanUp(string &text)
{
	string input=text;
	string output="";
	string intermediate1="";
	//Remove excess whitespace
	for(int i=0;i<input.length();i++)
	{
		if((input[i]!='\n'||input[i+1]!='\n')&&input[i]!='\t')intermediate1+=input[i];
	}
	//Double Check Ending (strip after @END)
	for(int i=0;i<intermediate1.length();i++)
	{
		if(XPINSCompileUtil::stringsMatch(i, intermediate1, "@END")){
			output+="@END\n";
			break;
		}
		output+=intermediate1[i];
	}
	text=output;
	return true;
}
//UTILTY FUNCTIONS
int XPINSCompileUtil::readVarIndex(string scriptText,int *startIndex,char expectedEnd)
{
	int i=*startIndex;
	int index=0;
	while(scriptText[i]!=expectedEnd)
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
	*startIndex=i;
	return index;
}
string XPINSCompileUtil::strRepresentationOfInt(int x)
{
	if(x<0)return "0";
	if(x/10==0)
	{
		switch (x)
		{
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
bool XPINSCompileUtil::stringsMatch(int start,string first, string sec)
{
	for(int i=0;i<sec.length();++i)
	{
		if(i+start>=first.length()||first[start+i]!=sec[i])
			return false;
	}
	return true;
}