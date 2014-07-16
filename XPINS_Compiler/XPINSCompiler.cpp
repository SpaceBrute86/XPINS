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
const int kMinor=6;

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
	if(!checkConstantSyntax(scriptText))return false;
	if(!replaceConstants(scriptText))return false;
	if(!renameFunctions(scriptText))return false;
	if(!renameBuiltIns(scriptText))return false;
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
		if(input[i]=='/'&&(input[i+1]=='/'||input[i+1]=='*'))//Comment
		{
			++i;
			if(input[i]=='*')//multi line comments
			{
				while (input[i]!='*'||input[i+1]!='/')++i;
				++i;
			}
			else // single line comments
			{
				while (input[i+1]!='\n'||i+1==input.length())++i;
			}
		}
		else if(input[i]!=';'&&input[i]!='\t') output+=input[i];//No semicolons or tabs in compiled script
		if(input[i]==';'&&input[i+1]!='\n')output+='\n';//Replace semicolons with newlines
	}
	text=output;
	return true;
}
bool XPINSCompiler::checkConstantSyntax(string& input)
{
	string output="";
	int i=0;
	while(!XPINSCompileUtil::stringsMatch(i, input, "@CONST"))output+=input[i++];//Skip @PARSER
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
		   input[i]=='<'||input[i]==' '||//typical operations and Constants
		   input[i]=='|'||input[i]=='&'||input[i]=='>'||input[i]=='!'||input[i]=='+'||input[i]=='-'||
		   input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%')//Expression Specific
		{
			while (input[i+1]==' ')++i;
			//Check for following:
			if(input[i+1]!='$'&&input[i+1]!='#'&&input[i+1]!='?'&&input[i+1]!='X'&&input[i+1]!='~'&&input[i+1]!='@'&&//Already given Key character
			   input[i+1]!='='&&input[i+1]!='+'&&(input[i+1]!='-'||input[i]!='-')&&input[i+1]!=')'&&//Multi-char Expression Operators
			   input[i+1]!='\n')//end of line
			{
				output+='~';
			}
		}
	}
	input=output;
	return true;
}
bool XPINSCompiler::replaceConstants(string &text)
{
	string input=text;
	//Initialize first intermediate
	string intermediate1="";
	string intermediate2="";
	int j=0;
	while (!XPINSCompileUtil::stringsMatch(j, input, "@CONST"))intermediate1+=input[j++];
	int i=j;
	while (!XPINSCompileUtil::stringsMatch(++j, input, "@FUNC"));//Skip to @Func Block
	for(;j<input.length();j++)intermediate1+=input[j];
	//Replace Constants
	while (true)
	{
		string constName="~";
		while (input[i]!='\n'||input[i+1]=='\n')++i;
		if(XPINSCompileUtil::stringsMatch(i, input, "\n@END"))break;
		//Process constant declaration
		while (input[++i]!='=')constName+=input[i];
		string constVal="";
		while (input[++i]!='\n')//Get Constant Value
		{
			constVal+=input[i];
		}
		//Replace constant in script
		intermediate2="";
		j=0;
		while (!XPINSCompileUtil::stringsMatch(j, intermediate1, "@CODE")) intermediate2+=intermediate1[j++];
		while(j<intermediate1.length())
		{
			//CHECK FOR @END
			if(XPINSCompileUtil::stringsMatch(j, intermediate1, "@END"))
			{
				intermediate2+="@END";
				break;
			}
			if(intermediate1[j]=='\"'&&intermediate1[j-1]=='~')//Skip String Constants
			{
				intermediate2+='\"';
				while (++j<intermediate1.length()) {
					if(intermediate1[j]=='\\'&&(intermediate1[j+1]=='\\'||intermediate1[j+1]=='\"'))
					{
						intermediate2+=intermediate1[j];
						intermediate2+=intermediate1[++j];
					}
					else if(intermediate1[j]=='\"')break;
					else intermediate2+=intermediate1[j];
				}
			}
			//Find start of Constant
			if(XPINSCompileUtil::stringsMatch(j, intermediate1, constName))
			{
				intermediate2+=constVal;//Replace Constant
				j+=constName.length();
			}
			else
			{
				intermediate2+=intermediate1[j++];
			}
		}
		intermediate1=""+intermediate2;
	}
	text=intermediate1;
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
		while (i<input.length()&&input[i++]!=' ');
		//read function name
		string functionName;
		for(j=i;j<input.length()&&input[j]!='(';j++)
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
				if(XPINSCompileUtil::stringsMatch(j, intermediate1, functionName+'('))
				{
					if(functionType!=' ')intermediate2+=functionType;//add function types to user functions.
					intermediate2+='M';
					intermediate2+=XPINSCompileUtil::strRepresentationOfInt(modNum);//Module Number
					intermediate2+='F';
					intermediate2+=XPINSCompileUtil::strRepresentationOfInt(x);//Funciton Number
					while(intermediate1[j]!='(')++j;//Find '('
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
bool XPINSCompiler::renameBuiltIns(string &text)
{
	string input=text;
	string output="";
	for(int i=0;i<input.length();++i)
	{
		output+=input[i];
		if(input[i]=='X'&&input[i+1]=='_')//found built-in
		{
			i+=2;
			string name="";
			while (input[i]!='(') name+=input[i++];
			//Rename Function
			//Bool Functions
			if(name.compare("MARKOV_REACHABLE")==0)return "B1";
			//Number Functions
			else if(name.compare("SIN")==0)output+= "N1";
			else if(name.compare("COS")==0)output+= "N2";
			else if(name.compare("TAN")==0)output+= "N3";
			else if(name.compare("ASIN")==0)output+= "N4";
			else if(name.compare("ACOS")==0)output+= "N5";
			else if(name.compare("ATAN")==0)output+= "N6";
			else if(name.compare("SQRT")==0)output+= "N7";
			else if(name.compare("LN")==0)output+= "N8";
			else if(name.compare("LOG")==0)output+= "N9";
			else if(name.compare("ABS")==0)output+= "N10";
			else if(name.compare("FLOOR")==0)output+= "N11";
			else if(name.compare("ADDPOLAR")==0)output+= "N12";
			else if(name.compare("DIST")==0)output+= "N13";
			else if(name.compare("VX")==0)output+= "N14";
			else if(name.compare("VY")==0)output+= "N15";
			else if(name.compare("VZ")==0)output+= "N16";
			else if(name.compare("VR")==0)output+= "N17";
			else if(name.compare("VTHETA")==0)output+= "N18";
			else if(name.compare("VMAG")==0)output+= "N19";
			else if(name.compare("VPHI")==0)output+= "N20";
			else if(name.compare("VECTOR_ANGLE")==0)output+= "N21";
			else if(name.compare("MATRIX_GET")==0)output+= "N22";
			else if(name.compare("DETERMINANT")==0)output+= "N23";
			else if(name.compare("RAND")==0)output+= "N24";
			else if(name.compare("RV_BERNOUILLI")==0)output+= "N25";
			else if(name.compare("RV_NORMAL")==0)output+= "N26";
			else if(name.compare("RV_EXP")==0)output+= "N27";
			else if(name.compare("RV_POISSON")==0)output+= "N28";
			else if(name.compare("COIN_FLIP")==0)output+= "N29";
			else if(name.compare("DICE_ROLL")==0)output+= "N30";
			else if(name.compare("MARKOV_SIM")==0)output+= "N31";
			else if(name.compare("MARKOV_PROB")==0)output+= "N32";
			else if(name.compare("MARKOV_STEADYSTATE")==0)output+= "N33";
			else if(name.compare("MARKOV_ABSORB_PROB")==0)output+= "N34";
			else if(name.compare("MARKOV_ABSORB_TIME")==0)output+= "N35";
			else if(name.compare("MARKOV_ABSORB_SIM")==0)output+= "N36";
			else if(name.compare("QR")==0)output+= "N37";
			else if(name.compare("QMAG")==0)output+= "N38";
			//Vector Functions
			else if(name.compare("VPROJECT")==0)output+= "V1";
			else if(name.compare("UNIT_VECTOR")==0)output+= "V2";
			else if(name.compare("QV")==0)output+= "V3";
			else if(name.compare("QUATERNION_ROTATE")==0)output+= "V4";
			//Quaternion Functions
			else if(name.compare("QUATERNION_CONJUGATE")==0)output+= "Q1";
			else if(name.compare("QUATERNION_INVERSE")==0)output+= "Q2";
			else if(name.compare("UNIT_QUATERNION")==0)output+= "Q3";
			//Matrix Functions
			else if(name.compare("ZERO_MATRIX")==0)output+= "M1";
			else if(name.compare("IDENTITY_MATRIX")==0)output+= "M2";
			else if(name.compare("ROTATION_MATRIX")==0)output+= "M3";
			else if(name.compare("EULER_ANGLE_MATRIX")==0)output+= "M4";
			else if(name.compare("QUATERNION_MATRIX")==0)output+= "M5";
			else if(name.compare("INVERT")==0)output+= "M6";
			else if(name.compare("TRANSPOSE")==0)output+= "M7";
			else if(name.compare("APPEND")==0)output+= "M8";
			else if(name.compare("ROW_ECHELON")==0)output+= "M9";
			else if(name.compare("REDUCED_ROW_ECHELON")==0)output+= "M10";
			//Polynomial Functions
			else if(name.compare("DERIVE")==0)output+= "P1";
			else if(name.compare("INTEGRATE")==0)output+= "P2";
			else if(name.compare("DIVERGENCE")==0)output+= "P3";
			else if(name.compare("SCALAR_LINE_INTEGRAL")==0)output+= "P4";
			else if(name.compare("VECTOR__LINE_INTEGRAL")==0)output+= "P5";
			else if(name.compare("SCALAR_SURFACE_INTEGRAL")==0)output+= "P6";
			else if(name.compare("VECTOR_SURFACE_INTEGRAL")==0)output+= "P7";
			else if(name.compare("VOLUME_INTEGRAL")==0)output+= "P8";
			//Vector Field Functions
			else if(name.compare("VECTOR_DERIVE")==0)output+= "F1";
			else if(name.compare("VECTOR_INTEGRATE")==0)output+= "F2";
			else if(name.compare("GRADIENT_VECTOR")==0)output+= "F3";
			else if(name.compare("CURL")==0)output+= "F4";
			//Void Functoins
			else if(name.compare("PRINT")==0)output+= "_1";
			else if(name.compare("MATRIX_SET")==0)output+= "_2";
			else return false;
			output+='(';
		}
	}
	text=output;
	return true;
}
bool XPINSCompiler::renameVars(string &text){
	string input=text;
	string intermediate="";
	//Rename Types
	for(int i=0;i<input.length();++i)
	{
		if((input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='Q'||input[i]=='M'||input[i]=='P'||input[i]=='F'||input[i]=='S'||input[i]=='*')&&input[i-1]=='\n')//Found Variable Declaration
		{
			intermediate+=input[i];
			while (input[++i]!=' ');
		}
		intermediate+=input[i];
	}
	input=intermediate;
	int i=0;
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
		while (i<input.length()&&input[i++]!='\n');//Get to next line
		if(input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='Q'||input[i]=='M'||input[i]=='S'||input[i]=='P'||input[i]=='F'||input[i]=='*'||input[i]=='A')//If we have a new variable type
		{
			//determine new variable name
			varType=input[i];
			int varNum=0;
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
			while (input[i++]!='$');
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
	for(int i=0;i<intermediate1.length();++i){
		if((intermediate1[i]=='B'||intermediate1[i]=='N'||intermediate1[i]=='V'||intermediate1[i]=='Q'||intermediate1[i]=='M'||intermediate1[i]=='P'||intermediate1[i]=='F'||intermediate1[i]=='S'||intermediate1[i]=='*'||intermediate1[i]=='A')&&intermediate1[i-1]=='\n')
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