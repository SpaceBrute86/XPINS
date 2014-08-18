//
//  XPINSCompiler.cpp
//  XPINS
//
//  Created by Robbie Markwick on 9/7/13.
//  Copyright (c) 2013 Robbie Markwick. All rights reserved.
//

#include <math.h>
#include <list>
#include "XPINSBIFMap.h"
#include "XPINSScriptableMath.h"
#include "XPINSCompiler.h"

using namespace std;
using namespace XPINSInstructions;

const int kMajor=1;
const int kMinor=0;


namespace XPINSCompileStringTools
{
	bool stringsMatch(int start,string first, string sec)
	{
		for(int i=0;i<sec.length();++i)
		{
			if(i+start>=first.length()||first[start+i]!=sec[i])
				return false;
		}
		return true;
	}
	int readInt(string script,int&i,char expectedEnd)
	{
		int index=0;
		for(;i<script.length()&&script[i]!=expectedEnd&&script[i]!='['&&script[i]!='('&&script[i]!='+'&&script[i]!='-'&&script[i]!=')';++i)
		{
			index*=10;
			if(script[i]=='1')index+=1;
			else if(script[i]=='2')index+=2;
			else if(script[i]=='3')index+=3;
			else if(script[i]=='4')index+=4;
			else if(script[i]=='5')index+=5;
			else if(script[i]=='6')index+=6;
			else if(script[i]=='7')index+=7;
			else if(script[i]=='8')index+=8;
			else if(script[i]=='9')index+=9;
			else if(script[i]!='0')index/=10;
		}
		return index;
	}
	string strRepresentationOfInt(int x)
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
	string copyStringConst(string script,int &i)
	{
		string stringConst="";
		stringConst+=script[i];
		while (++i<script.length()) {
			stringConst+=script[i];
			if(script[i]=='\\'&&(script[i+1]=='\\'||script[i+1]=='\"'))stringConst+=script[++i];
			else if(script[i]=='\"')break;
		}
		return stringConst;
	}
	void skipBlock(string script,int &i,string blockStart)
	{
		while(i<script.length()&&!stringsMatch(i, script, blockStart))++i;
		string blockEnd = "}";
		if (blockStart=="(")blockEnd=")";
		else if (blockStart=="[")blockEnd="]";
		else if (blockStart=="/*")blockEnd="*/";
		int count=1;
		while (count>0)
		{
			if(script[++i]=='\"')copyStringConst(script, i);
			else if(stringsMatch(i, script, blockStart)) ++count;
			else if(stringsMatch(i, script, blockEnd))--count;
		}
		i+=blockEnd.length()-1;
	}
}

namespace XPINSPrecompiler
{
	bool checkVersion(string script)
	{
		for(int i=0;i<script.length();++i)
		{
			if(XPINSCompileStringTools::stringsMatch(i, script, "@XPINS"))
			{
				while(i<script.length()&&script[i]!=' ')++i;
				//Check Version
				int MAJOR=XPINSCompileStringTools::readInt(script, i, '.');
				int MINOR=XPINSCompileStringTools::readInt(script, i, '\n');
				return MAJOR==kMajor&&MINOR<=kMinor;
			}
		}
		return false;
	}
	void removeComments(string &script)
	{
		string input=script;
		script="";
		int i=0;
		while(!XPINSCompileStringTools::stringsMatch(i, input, "@XPINS"))++i;
		while (input[i++]!='\n');
		for(;i<input.length();++i)
		{
			if(input[i]=='\"')script+=XPINSCompileStringTools::copyStringConst(input, i);
			else if(input[i]=='/'&&(input[i+1]=='/'||input[i+1]=='*'))//Comment
			{
				if(input[i+1]=='*')	XPINSCompileStringTools::skipBlock(input, i, "/*");//multi line comments
				else while (input[i+1]!='\n'||i+1==input.length())++i; // single line comments
			}
			else if(input[i]==';'&&input[i+1]!='\n')script+='\n';//Replace semicolons with newlines
			else if(input[i]>='a'&&input[i]<='z') script+=input[i]-('a'-'A');
			else if(input[i]!=';'&&input[i]!='\t') script+=input[i];//No semicolons or tabs in compiled script
		}
	}
	void cleanup(string &script)
	{
		string input=script;
		script="";
		for(int i=0;i<input.length();++i)
		{
			if(input[i]!='\n'||input[i+1]!='\n')script+=input[i];//Condense newlines
		}
	}
}

namespace XPINSSyntaxProcesser
{
	void convertDotSyntax(string& script)
	{
		string input=script;
		script="";
		int i=0;
		while(!XPINSCompileStringTools::stringsMatch(i, input, "@CODE"))script+=input[i++];
		for(;i<input.length();++i)
		{
			script+=input[i];
			if(input[i]=='\"')script+=XPINSCompileStringTools::copyStringConst(input, ++i);
			if((input[i+1]=='.'||(input[i+1]=='-'&&input[i+2]=='>'))&&(input[i+2]<'0'||input[i+2]>'9'))//Skip completed Dot operations
			{
				i+=2;
				while (++i<input.length())
				{
					if(input[i]=='('||input[i]==','||input[i]==')'||input[i]=='\n'||input[i]=='&'||input[i]=='|'||input[i]=='!'||input[i]=='<'||input[i]=='>'||input[i]=='='||input[i]=='+'||input[i]=='-'||input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%'||input[i]==']')break;
				}
				if (input[i]!='(')
				{
					script+=')';
					--i;
				}
				else if (input[i+1]!=')') script+=',';
			}
			//If it is a character that could be followed by an input
			else if((input[i]=='('||input[i]=='='||input[i]==','||input[i]=='['||(input[i]=='{'&&input[i-1]=='~')||
					 input[i]=='<'||input[i]==' '||input[i]=='\n'||input[i]=='|'||input[i]=='&'||input[i]=='>'||input[i]=='!'||(input[i]=='+'&&input[i+1]!='+')||(input[i]=='-'&&input[i+1]!='-')||input[i]=='*'||input[i]=='/'||input[i]=='^'||input[i]==':'||input[i]=='%')&& input[i+1]!='='&&input[i+1]!=')'&& input[i+1]!='\n')
			{
				int j=i+1;
				bool isDotSyntax=false;
				bool isElementalSyntax=false;
				for (; j<input.length(); ++j) {
					if ((input[j]=='-'&&input[j+1]=='>')&&(input[j+2]<'0'||input[j+2]>'9'))
					{
						isDotSyntax=true;
						isElementalSyntax=true;
						break;
					}
					else if (input[j]=='.'&&(input[j+1]<'0'||input[j+1]>'9'))
					{
						isDotSyntax=true;
						break;
					}
					else if (input[j]==','||input[j]==')'||input[j]=='\n'||input[j]=='&'||input[j]=='|'||input[j]=='!'||input[j]=='<'||input[j]=='>'||input[j]=='='||input[j]=='+'||input[j]=='-'||input[j]=='*'||input[j]=='/'||input[j]=='^'||input[j]==':'||input[j]=='%'||input[j]==']')break;
					else if(input[j]=='\"')XPINSCompileStringTools::copyStringConst(input, j);
					else if(input[j]=='(')XPINSCompileStringTools::skipBlock(input, j, "(");
					else if(input[j]=='[')XPINSCompileStringTools::skipBlock(input, j, "[");
				}
				if (isDotSyntax) {
					list<string> funcNames=list<string>(0);
					list<bool> elemental=list<bool>(1,isElementalSyntax);
					string currentFuncName="";
					if(isElementalSyntax)++j;
					while (++j<input.length())
					{
						if (input[j]=='.')
						{
							funcNames.push_back(currentFuncName);
							elemental.push_back(false);
							currentFuncName="";
							++j;
						}
						else if (input[j]=='-'&&input[j+1]=='>')
						{
							funcNames.push_back(currentFuncName);
							elemental.push_back(true);
							currentFuncName="";
							j+=2;
						}
						else if (input[j]=='(')
						{
							XPINSCompileStringTools::skipBlock(input, j, "(");
							if(input[++j]=='.')
							{
								funcNames.push_back(currentFuncName);
								elemental.push_back(false);
								currentFuncName="";
								++j;
							}
							else if (input[j]=='-'&&input[j+1]=='>')
							{
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
					while (funcNames.size()>0)
					{
						script+=funcNames.front();
						script+=elemental.front()?".(":"(";
						funcNames.pop_front();
						elemental.pop_front();
					}
				}
			}
		}
	}
	void classifyComponents(string& input)
	{
		string output="";
		int i=0;
		while(!XPINSCompileStringTools::stringsMatch(i, input, "@CODE"))output+=input[i++];//Skip @PARSER
		for(;i<input.length();++i)
		{
			output+=input[i];
			if(input[i]=='\"'){++i;output+=XPINSCompileStringTools::copyStringConst(input, i);}
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
					if (input[i]!='\n' && input [i] !=']')
						switch (input[i+1]) {
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
					else if((input[i+1]>='A'&&input[i+1]<='Z')||(input[i+1]>='a'&&input[i+1]<='z')||input[i+1]=='_')
					{
						bool isVar=false;
						for (int j=i+1; j<input.length(); ++j)
						{
							if (input[j]=='(')break;
							if (input[j]==','||input[j]==')'||input[j]=='&'||input[j]=='|'||input[j]=='!'||input[j]=='<'||input[j]=='>'||input[j]=='='||input[j]=='+'||input[j]=='-'||input[j]=='*'||input[j]=='/'||input[j]=='^'||input[j]==':'||input[j]=='%'||input[j]==']')
							{
								isVar=true;
								break;
							}
						}
						if (isVar) output+="$_";
						else
						{
							string name="";
							for (; input[i+1]!='('; ++i) if(input[i+1]!='.')name+=input[i+1];
							if(name.compare("MARKOV_REACHABLE")==0)output+= "XB"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_REACHABLE);
							//Number Functions
							else if(name.compare("SIN")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_SIN);
							else if(name.compare("COS")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_COS);
							else if(name.compare("TAN")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_TAN);
							else if(name.compare("CSC")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_CSC);
							else if(name.compare("SEC")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_SEC);
							else if(name.compare("COT")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_COT);
							else if(name.compare("ASIN")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ASIN);
							else if(name.compare("ACOS")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ACOS);
							else if(name.compare("ATAN")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ATAN);
							else if(name.compare("ACSC")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ACSC);
							else if(name.compare("ASEC")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ASEC);
							else if(name.compare("ACOT")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ACOT);
							else if(name.compare("ATAN2")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ATAN2);
							else if(name.compare("ADDPOLAR")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ADDPOLAR);
							else if(name.compare("SQRT")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_SQRT);
							else if(name.compare("LN")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_LN);
							else if(name.compare("LOG")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_LOG);
							else if(name.compare("ABS")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ABS);
							else if(name.compare("FLOOR")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_FLOOR);
							else if(name.compare("DIST")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_DIST);
							else if(name.compare("X")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_X);
							else if(name.compare("Y")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_Y);
							else if(name.compare("Z")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_Z);
							else if(name.compare("MAGNITUDE")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MAGNITUDE);
							else if(name.compare("R")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_R);
							else if(name.compare("THETA")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_THETA);
							else if(name.compare("PHI")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_PHI);
							else if(name.compare("ANGLE_BETWEEN_VECTORS")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_ANGLE_BETWEEN_VECTORS);
							else if(name.compare("DETERMINANT")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_DETERMINANT);
							else if(name.compare("RAND")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_RAND);
							else if(name.compare("RV_BERNOUILLI")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_RV_BERNOULLI);
							else if(name.compare("RV_NORMAL")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_RV_NORMAL);
							else if(name.compare("RV_EXP")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_RV_EXP);
							else if(name.compare("RV_POISSON")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_RV_POISSON);
							else if(name.compare("COIN_FLIP")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_COIN_FLIP);
							else if(name.compare("DICE_ROLL")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_DICE_ROLL);
							else if(name.compare("MARKOV_SIM")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_SIM);
							else if(name.compare("MARKOV_PROB")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_PROB);
							else if(name.compare("MARKOV_STEADYSTATE")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_STEADYSTATE);
							else if(name.compare("MARKOV_ABSORB_PROB")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_ABSORB_PROB);
							else if(name.compare("MARKOV_ABSORB_TIME")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_ABSORB_TIME);
							else if(name.compare("MARKOV_ABSORB_SIM")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_MARKOV_ABSORB_SIM);
							else if(name.compare("SIZE")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_SIZE);
							else if(name.compare("EVALUATE")==0)output+= "XN"+XPINSCompileStringTools::strRepresentationOfInt(X_EVALUATE);
							//Vector Functions
							else if(name.compare("PROJECT_ONTO_VECTOR")==0)output+= "XV"+XPINSCompileStringTools::strRepresentationOfInt(X_PROJECT_ONTO_VECTOR);
							else if(name.compare("UNIT_VECTOR")==0)output+= "XV"+XPINSCompileStringTools::strRepresentationOfInt(X_UNIT_VECTOR);
							else if(name.compare("V")==0)output+= "XV"+XPINSCompileStringTools::strRepresentationOfInt(X_V);
							else if(name.compare("ROTATE_VECTOR")==0)output+= "XV"+XPINSCompileStringTools::strRepresentationOfInt(X_ROTATE_VECTOR);
							else if(name.compare("VECTOR_EVALUATE")==0)output+= "XV"+XPINSCompileStringTools::strRepresentationOfInt(X_VECTOR_EVALUATE);
							//Quaternion Functions
							else if(name.compare("CONJUGATE")==0)output+= "XQ"+XPINSCompileStringTools::strRepresentationOfInt(X_CONJUGATE);
							else if(name.compare("INVERSE")==0)output+= "XQ"+XPINSCompileStringTools::strRepresentationOfInt(X_INVERSE);
							else if(name.compare("UNIT_QUATERNION")==0)output+= "XQ"+XPINSCompileStringTools::strRepresentationOfInt(X_UNIT_QUATERNION);
							//Matrix Functions
							else if(name.compare("ZERO_MATRIX")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_ZERO_MATRIX);
							else if(name.compare("IDENTITY_MATRIX")==0)output+="XM"+XPINSCompileStringTools::strRepresentationOfInt(X_IDENTITY_MATRIX);
							else if(name.compare("ROTATION_MATRIX")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_ROTATION_MATRIX);
							else if(name.compare("EULER_ANGLE_MATRIX")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_EULER_ANGLE_MATRIX);
							else if(name.compare("QUATERNION_MATRIX")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_QUATERNION_MATRIX);
							else if(name.compare("INVERT")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_INVERT);
							else if(name.compare("TRANSPOSE")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_TRANSPOSE);
							else if(name.compare("APPEND")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_APPEND);
							else if(name.compare("ROW_ECHELON")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_ROW_ECHELON);
							else if(name.compare("REDUCED_ROW_ECHELON")==0)output+= "XM"+XPINSCompileStringTools::strRepresentationOfInt(X_REDUCED_ROW_ECHELON);
							//Polynomial Functions
							else if(name.compare("DERIVE")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_DERIVE);
							else if(name.compare("INTEGRATE")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_INTEGRATE);
							else if(name.compare("DIVERGENCE")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_DIVERGENCE);
							else if(name.compare("LINE_INTEGRAL")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_LINE_INTEGRAL);
							else if(name.compare("SURFACE_INTEGRAL")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_SURFACE_INTEGRAL);
							else if(name.compare("VOLUME_INTEGRAL")==0)output+= "XP"+XPINSCompileStringTools::strRepresentationOfInt(X_VOLUME_INTEGRAL);
							//Vector Field Functions
							else if(name.compare("COMPONENT_DERIVE")==0)output+= "XF"+XPINSCompileStringTools::strRepresentationOfInt(X_COMPONENT_DERIVE);
							else if(name.compare("COMPONENT_INTEGRATE")==0)output+= "XF"+XPINSCompileStringTools::strRepresentationOfInt(X_COMPONENT_INTEGRATE);
							else if(name.compare("GRADIENT_VECTOR")==0)output+= "XF"+XPINSCompileStringTools::strRepresentationOfInt(X_GRADIENT_VECTOR);
							else if(name.compare("CURL")==0)output+= "XF"+XPINSCompileStringTools::strRepresentationOfInt(X_CURL);
							//Void Functoins
							else if(name.compare("PRINT")==0)output+= "X_"+XPINSCompileStringTools::strRepresentationOfInt(X_PRINT);
							else if(name.compare("PRINTF")==0)output+= "X_"+XPINSCompileStringTools::strRepresentationOfInt(X_PRINTF);
							else if(name.compare("RESIZE")==0)output+= "X_"+XPINSCompileStringTools::strRepresentationOfInt(X_RESIZE);
							else output+="#"+name;
							if (input[i]=='.') output+='.';
						}
					}
					
				}
			}
		}
		input=output;
	}
	
}

namespace XPINSComponentRenaming {
	
	string replaceComponent(string input, string constName, string constVal){
		string output="";
		int i=0;
		while(i<input.length())
		{
			//Find start of Constant
			if(XPINSCompileStringTools::stringsMatch(i, input, constName) && input[i+constName.length()]!='('&&(input[i+constName.length()]!='.'||input[i+constName.length()+1]!='(')
			   &&(input[i-1]=='('||input[i-1]=='='||input[i-1]==','||input[i-1]=='['||input[i-1]=='{'||
				  input[i-1]=='<'||input[i-1]==' '||input[i-1]=='\n'||//typical operations and Constants
				  input[i-1]=='|'||input[i-1]=='&'||input[i-1]=='>'||input[i-1]=='!'||input[i-1]=='+'||input[i-1]=='-'||
				  input[i-1]=='*'||input[i-1]=='/'||input[i-1]=='^'||input[i-1]==':'||input[i-1]=='%') &&(input[i+constName.length()-1]=='('||((input[i+constName.length()]<'A'||input[i+constName.length()]>'Z')&&(input[i+constName.length()]<'0'||input[i+constName.length()]>'9')))&&(input[i+constName.length()]!='_'))
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
	
	void replaceConstants(string &text)
	{
		string input=text;
		//Initialize first intermediate
		string output="";
		int j=0;
		while (!XPINSCompileStringTools::stringsMatch(j, input, "@CONST"))++j;
		int i=j;
		while (!XPINSCompileStringTools::stringsMatch(++j, input, "@FUNC"));//Skip to @Func Block
		while (!XPINSCompileStringTools::stringsMatch(j, input, "@CODE"))output+=input[j++];//Copy to @CODE
		string codeBlock="";
		j+=6;
		while (!XPINSCompileStringTools::stringsMatch(++j, input, "@END"))codeBlock+=input[j];
		//Replace Constants
		while (true)
		{
			string constName="";
			while (input[i]!='\n'||input[i+1]=='\n')++i;
			if(XPINSCompileStringTools::stringsMatch(i, input, "\n@END"))break;
			//Process constant declaration
			while (input[++i]!='=')constName+=input[i];
			string constVal="";
			while (input[++i]!='\n')//Get Constant Value
			{
				constVal+=input[i];
			}
			codeBlock=replaceComponent(codeBlock, constName, constVal);
		}
		//Replace built in constants
		codeBlock=replaceComponent(codeBlock, "TRUE", "T");
		codeBlock=replaceComponent(codeBlock, "YES", "T");
		codeBlock=replaceComponent(codeBlock, "FALSE", "F");
		codeBlock=replaceComponent(codeBlock, "NO", "F");
		codeBlock=replaceComponent(codeBlock, "PI", "3.141592653589793");
		codeBlock=replaceComponent(codeBlock, "E", "2.718281828459045");
		text=output+"@CODE\n"+codeBlock+"@END";
	}
	//Does the heavy lifting for Function Renaming Function
	//Input script (for reading block), intermediate (for renaming), module number (for assigning new name), and where to start reading the input
	string renameFuncBlock(string input, string intermediate, int modNum, int blockStart)
	{
		int i=blockStart;
		int j=0;
		int x=1;
		while(!XPINSCompileStringTools::stringsMatch(i, input, "\n@END"))
		{
			while(i<input.length()&&input[i++]!='\n');//Get to next line in Function Block
			if(XPINSCompileStringTools::stringsMatch(i, input, "@END")||i>=input.length())break;
			//Read Function Type
			char functionType=input[i++];
			if(functionType=='V'&&input[i]!='E')functionType='_';
			while (++i<input.length()&&input[i]!=' ');
			while (++i<input.length()&&input[i]==' ');
			//read function name
			string functionName="";
			for(j=i;j<input.length()&&input[j]!='('&&input[j]!='\n';j++)
			{
				functionName+=input[j];
			}
			//Replace function name
			string newName="#";
			newName+=functionType;
			newName+="M"+XPINSCompileStringTools::strRepresentationOfInt(modNum);
			newName+="F"+XPINSCompileStringTools::strRepresentationOfInt(x);
			intermediate=replaceComponent(intermediate, "#"+functionName+"(", newName+"(");
			intermediate=replaceComponent(intermediate, "#"+functionName+".(", newName+".(");
			++x;
			while (i<input.length()&&input[++i]!='\n');
		}
		return intermediate;
	}
	void renameFunctions(string &text){
		string input=text;
		//Initialize first intermediate
		string intermediate1="";
		int j=0;
		while (!XPINSCompileStringTools::stringsMatch(j, input, "@FUNC"))intermediate1+=input[j++];
		while (!XPINSCompileStringTools::stringsMatch(++j, input, "@CODE"));
		j+=6;
		for(;j<input.length();j++)intermediate1+=input[j];
		//Process Local Function block
		int i=0;
		while(!XPINSCompileStringTools::stringsMatch(i, input, "@FUNC"))++i;
		i+=5;
		intermediate1=renameFuncBlock(input, intermediate1,0, i);
		while(!XPINSCompileStringTools::stringsMatch(++i, input, "@END"));
		//Process Module blocks
		for (int modNum=1;;++modNum)
		{
			while(!XPINSCompileStringTools::stringsMatch(i, input, "@MODULE")&&
				  !XPINSCompileStringTools::stringsMatch(i, input, "@CODE"))++i;
			if (XPINSCompileStringTools::stringsMatch(i, input, "@CODE")) break;//Finished Module Processing
			i+=7;
			intermediate1=renameFuncBlock(input, intermediate1,modNum, i);
			while(!XPINSCompileStringTools::stringsMatch(++i, input, "@END"));
		}
		text=intermediate1;
	}
	vector<int> renameVars(string &text){
		string input=text;
		string intermediate="";
		//Rename Types
		int i=0;
		for(;i<input.length();++i)
		{
			if(XPINSCompileStringTools::stringsMatch(i, input, "\n@END"))break;
			if(input[i]=='\"'){intermediate+=XPINSCompileStringTools::copyStringConst(input, i);++i;}
			if((input[i]=='B'||input[i]=='N'||input[i]=='V'||input[i]=='Q'||input[i]=='M'||input[i]=='P'||input[i]=='F'||input[i]=='S'||input[i]=='O'||input[i]=='A')&&input[i-1]=='@')//Found Variable Declaration
			{
				intermediate+=input[i];
				while (input[++i]!=' ');
			}
			intermediate+=input[i];
		}
		input=intermediate;
		//Initialize intermediates
		string intermediate1=""+input;
		string intermediate2="";
		int j=0;
		i=0;
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
		while(i<input.length())
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
						case 'B':	varNum=xb++; break; //BOOL
						case 'N':	varNum=xn++; break; //NUM
						case 'V':	varNum=xv++; break; //VEC
						case 'Q':	varNum=xq++; break; //QUAT
						case 'M':	varNum=xm++; break; //MAT
						case 'S':	varNum=xs++; break; //STR
						case 'P':	varNum=xp++; break; //POLY
						case 'F':	varNum=xf++; break; //FIELD
						case 'A':	varNum=xa++; break; //ARR
						default:	varNum=++xo; varType='O'; //Default to object
					}
					//read Var name
					string varName;
					while(i<input.length()&&input[i]!='='&&input[i]!=' ')
					{
						varName+=input[i++];
					}
					//Replace Var name
					string newName="$";
					newName+=varType+XPINSCompileStringTools::strRepresentationOfInt(varNum);
					intermediate1=replaceComponent(intermediate1, "$"+varName, newName);
				}
			}
			++i;
		}
		//Remove Variable Types
		intermediate2="";
		for(i=0;i<intermediate1.length();++i){
			if((intermediate1[i+1]=='B'||intermediate1[i+1]=='N'||intermediate1[i+1]=='V'||intermediate1[i+1]=='Q'||intermediate1[i+1]=='M'||intermediate1[i+1]=='P'||intermediate1[i+1]=='F'||intermediate1[i+1]=='S'||intermediate1[i+1]=='*'||intermediate1[i+1]=='A')&&intermediate1[i]=='@')
			{
				while (intermediate1[++i]!='$');
			}
			intermediate2+=intermediate1[i];
		}
		text=intermediate2;
		//Calculate Variable sizes
		vector<int> varSizes=vector<int>(10);
		varSizes[0]=xb;
		varSizes[1]=xn;
		varSizes[2]=xv;
		varSizes[3]=xq;
		varSizes[4]=xm;
		varSizes[5]=xp;
		varSizes[6]=xf;
		varSizes[7]=xs;
		varSizes[8]=xo;
		varSizes[9]=xa;
		return varSizes;
	}
}

namespace XPINSInstructionAssembler{
	opCode FindOp(string script,int i, bool* assign,char& opChar,DataType type)
	{
		for (; i<script.length(); ++i) {
			if(script[i]=='(')
			{
				for (int x=1; x>0;) {
					++i;
					if(script[i]=='(')
						++x;
					else if(script[i]==')')
						--x;
				}
				++i;
			}
			opChar=script[i];
			switch (opChar) {
				case '|':
					if(type==BOOLEAN){
						*assign = script[i+1]=='=';
						return OR;
					}
					break;
				case '&':
					if(type==BOOLEAN){
						*assign = script[i+1]=='=';
						return AND;
					}
					if(type==POLYNOMIAL|type==FIELD){
						if(assign&&script[i+1]=='=')*assign=true;
						return COMPOSITION;
					}break;
				case '!':
					if(type==BOOLEAN){
						if (script[i-1]=='('){
							*assign = script[i+1]=='=';
							return NOT;
						}
						else return NOTEQUAL;
					}
					break;
				case '<':
					if(type==BOOLEAN)return script[i+1]=='='?LESSEQ:LESS;
					break;
				case '>':
					if(type==BOOLEAN)return script[i+1]=='='?GREATEREQ:GREATER;
					break;
				case '=':
					if(type==BOOLEAN) return EQAUAL;
					break;
				case '+':
					if(type==NUMBER||type==VECTOR||type==QUATERNION||type==MATRIX||type==POLYNOMIAL||type==FIELD){
						if(script[i+1]=='+')
							return script[i-1]=='('?PREINCREMENT:POSTINCREMENT;
						if(assign&&script[i+1]=='=')*assign=true;
						return ADD;
					}break;
				case '-':
					if(type==NUMBER||type==VECTOR||type==QUATERNION||type==MATRIX||type==POLYNOMIAL||type==FIELD){
						if(script[i+1]=='-')
							return script[i-1]=='('?PREDECREMENT:POSTDECREMENT;
						if(assign&&script[i+1]=='=')*assign=true;
						return SUBTRACT;
					}break;
				case '*':
					if(type==NUMBER||type==VECTOR||type==QUATERNION||type==MATRIX||type==POLYNOMIAL||type==FIELD){
						if(assign&&script[i+1]=='=')*assign=true;
						return MULTIPLY;
					}break;
				case '/':
					if(type==NUMBER||type==VECTOR||type==QUATERNION||type==MATRIX||type==POLYNOMIAL||type==FIELD){
						if(assign&&script[i+1]=='=')*assign=true;
						return DIVIDE;
					}break;
				case '^':
					if(type==NUMBER||type==POLYNOMIAL){
						if(assign&&script[i+1]=='=')*assign=true;
						return POWER;
					}break;
				case '%':
					if(type==NUMBER){
						if(assign&&script[i+1]=='=')*assign=true;
						return REMAINDER;
					}break;
			}
		}
		return INVALID;
	}
	
	Argument parseArgument(string scriptText,int &i,char expectedEnd)
	{
		Argument arg=Argument();
		while (scriptText[i]!='$'&&scriptText[i]!='~'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X')++i;
		switch (scriptText[i++]) {
			case '$':
				arg.type=VAR;
				switch (scriptText[i++]) {
					case 'B':	arg.dataType=BOOLEAN; break;
					case 'N':	arg.dataType=NUMBER; break;
					case 'V':	arg.dataType=VECTOR; break;
					case 'Q':	arg.dataType=QUATERNION; break;
					case 'M':	arg.dataType=MATRIX; break;
					case 'P':	arg.dataType=POLYNOMIAL; break;
					case 'F':	arg.dataType=FIELD; break;
					case 'S':	arg.dataType=STRING; break;
					case 'A':	arg.dataType=ARRAY; break;
					default:	arg.dataType=OBJECT; break;
				}
				arg.number=XPINSCompileStringTools::readInt(scriptText, i, expectedEnd);
				break;
			case '~':
				arg.type=CONST;
				switch (scriptText[i]) { //Determine type
					case 'F':
						if(scriptText[i+1]=='<')arg.dataType=FIELD;
					case 'T':
						arg.dataType=BOOLEAN;
						break;
					case 'N':
						arg.dataType=NUMBER;
						break;
					case '<':
					case 'P':
					case 'S':
						arg.dataType=VECTOR;
						break;
					case 'Q':
						arg.dataType=QUATERNION;
						break;
					case '[':
						arg.dataType=MATRIX;
						break;
					case '(':
						arg.dataType=POLYNOMIAL;
						break;
					case '\"':
						arg.dataType=STRING;
						break;
					case '{':
						arg.dataType=ARRAY;
						
						break;
					default:
						arg.dataType=NUMBER;
						break;
				}
				switch (arg.dataType) { //Determine value
					case BOOLEAN:
						arg.literalValue=new bool(scriptText[i]=='T');
						break;
					case NUMBER:{
						double val=0;
						int exp=0;
						bool fpart=false;
						bool isNeg=scriptText[i]=='-';
						while(i<scriptText.length()&&scriptText[i]!=')'&&scriptText[i]!='e'&&scriptText[i]!='E'&&scriptText[i]!='+'&&scriptText[i]!='-'&&scriptText[i]!=expectedEnd)
						{
							if(fpart)--exp;//record decimal place
							val*=10;
							if(scriptText[i]=='1')val+=1;
							else if(scriptText[i]=='2')val+=2;
							else if(scriptText[i]=='3')val+=3;
							else if(scriptText[i]=='4')val+=4;
							else if(scriptText[i]=='5')val+=5;
							else if(scriptText[i]=='6')val+=6;
							else if(scriptText[i]=='7')val+=7;
							else if(scriptText[i]=='8')val+=8;
							else if(scriptText[i]=='9')val+=9;
							else if(scriptText[i]!='0')val/=10;
							if(scriptText[i]=='.')fpart=true;//Start recording decimal places
							++i;
						}
						bool isENeg=false;
						while(i<scriptText.length()&&scriptText[i]!=')'&&scriptText[i]!='+'&&scriptText[i]!='-'&&scriptText[i]!=expectedEnd)
						{
							exp*=10;
							if(scriptText[i]=='1')exp+=1;
							else if(scriptText[i]=='2')exp+=2;
							else if(scriptText[i]=='3')exp+=3;
							else if(scriptText[i]=='4')exp+=4;
							else if(scriptText[i]=='5')exp+=5;
							else if(scriptText[i]=='6')exp+=6;
							else if(scriptText[i]=='7')exp+=7;
							else if(scriptText[i]=='8')exp+=8;
							else if(scriptText[i]=='9')exp+=9;
							else if(scriptText[i]!='0')exp/=10;
							if(scriptText[i]=='-')isENeg=true;
							++i;
						}
						val=(isNeg?-1:1) * val * pow(10, (isENeg?-1:1)*exp);
						arg.literalValue=new double(val);
					}break;
					case VECTOR:
						switch (scriptText[i]) {
							case 'P':
								arg.number=XPINSScriptableMath::Vector::coordSystem::Polar;
								break;
							case 'S':
								arg.number=XPINSScriptableMath::Vector::coordSystem::Spherical;
								break;
							default:
								arg.number=XPINSScriptableMath::Vector::coordSystem::Cartesian;
								break;
						}
					case FIELD:
						while (scriptText[i++]!='<');
						arg.arguments.resize(3);
						arg.arguments[0]=parseArgument(scriptText, i, ',');
						arg.arguments[1]=parseArgument(scriptText, i, ',');
						arg.arguments[2]=parseArgument(scriptText, i, '>');
						break;
					case QUATERNION:
						while (scriptText[i++]!='<');
						arg.arguments.resize(2);
						arg.arguments[0]=parseArgument(scriptText, i, ',');
						arg.arguments[1]=parseArgument(scriptText, i, '>');
						if (scriptText[i]==','){
							Argument xArg=arg.arguments[1];
							arg.arguments[1].dataType=VECTOR;
							arg.arguments[1].number=0;
							arg.arguments[1].arguments.resize(3);
							arg.arguments[1].arguments[0]=xArg;
							arg.arguments[1].arguments[1]=parseArgument(scriptText, i, ',');
							arg.arguments[1].arguments[2]=parseArgument(scriptText, i, '>');
						}
						break;
					case MATRIX:{
						arg.modNumber=1;
						arg.number=1;
						int sizeCounter=++i;
						for (;scriptText[sizeCounter]!='|'&&scriptText[sizeCounter]!=']'; ++sizeCounter)//Find Column Count
						{
							if(scriptText[sizeCounter]=='(')//Skip Parenthesis blocks
							{
								++sizeCounter;
								for (int count=1; count>0; ++sizeCounter) {
									if(scriptText[sizeCounter]=='(')++count;
									else if(scriptText[sizeCounter]==')')--count;
								}
							}
							if(scriptText[sizeCounter]==',')++arg.number;
						}
						for (; scriptText[sizeCounter]!=']'; ++sizeCounter)//Find Row Count
						{
							if(scriptText[sizeCounter]=='(')//Skip Parenthesis blocks
							{
								++sizeCounter;
								for (int count=1; count>0; ++sizeCounter) {
									if(scriptText[sizeCounter]=='(')++count;
									else if(scriptText[sizeCounter]==')')--count;
								}
							}
							if(scriptText[sizeCounter]=='|')++arg.modNumber;
						}
						arg.arguments.resize(arg.modNumber*arg.number);
						for (int r=0;r<arg.modNumber;++r){
							for (int c=0;c<arg.number;++c){
								char endChar=',';
								if(c==arg.number-1)
								{
									endChar = r == arg.modNumber-1 ? ']' : '|';
								}
								arg.arguments[r*arg.number+c]=parseArgument(scriptText, i, endChar);
							}
						}
					}break;
					case POLYNOMIAL:{
						++i;
						vector<XPINSScriptableMath::Polynomial::Monomial> mons=vector<XPINSScriptableMath::Polynomial::Monomial>();
						for (arg.number=1;true;++arg.number){
							arg.arguments.resize(arg.number);
							arg.arguments[arg.number-1]=parseArgument(scriptText, i, '_');
							mons.resize(arg.number);
							mons[arg.number-1].exponents=vector<unsigned int>();
							while (scriptText[i]=='_')
							{
								++i;
								unsigned int expIndex=0;
								switch (scriptText[i++])
								{
									case 'X':
									case 'x':
										expIndex=1;
										break;
									case 'Y':
									case 'y':
										expIndex=2;
										break;
									case 'Z':
									case 'z':
										expIndex=3;
										break;
									case 'T':
									case 't':
										expIndex=4;
										break;
									case 'U':
									case 'u':
										expIndex=5;
										break;
									case 'V':
									case 'v':
										expIndex=6;
										break;
									case 'W':
									case 'w':
										expIndex=7;
										break;
								}
								if(scriptText[i]=='+'||scriptText[i]=='-'||scriptText[i]==')'||scriptText[i]=='_')
								{
									if(expIndex>mons[arg.number-1].exponents.size())mons[arg.number-1].exponents.resize(expIndex,0);
									mons[arg.number-1].exponents[expIndex-1]+=1;
								}
								else if(expIndex!=0)
								{
									if(expIndex>mons[arg.number-1].exponents.size())mons[arg.number-1].exponents.resize(expIndex,0);
									mons[arg.number-1].exponents[expIndex-1]+=XPINSCompileStringTools::readInt(scriptText,i, '_');
								}
								if(scriptText[i]=='+'||scriptText[i]=='-'||scriptText[i]==')')break;
							}
							if(scriptText[i]==')')break;
						}
						arg.literalValue=new vector<XPINSScriptableMath::Polynomial::Monomial>(mons);
					}break;
					case STRING:{
						string str="";
						while (++i<scriptText.length())
						{
							if(scriptText[i]=='\\')
							{
								++i;
								switch (scriptText[i]) {
									case 'n':str+='\n';
										break;
									case '\t':str+='\t';
										break;
									case '\\':str+='\\';
										break;
									case '\'':str+='\'';
										break;
									case '\"':str+='\"';
										break;
									case '\r':str+='\r';
										break;
									case 'a':str+='\a';
										break;
									case 'b':str+='\b';
										break;
									case 'f':str+='\f';
										break;
									case 'v':str+='\v';
										break;
									case 'e':str+='\e';
										break;
									default:
										break;
								}
							}
							else if(scriptText[i]=='\"')break;
							else str+=scriptText[i];
						}
						arg.literalValue=new string(str);
					}break;
					case ARRAY:{
						int temp=i;
						int size=1;
						for (;scriptText[temp]!='}'; ++temp)//Find Array Count
							if(scriptText[temp]==',')++size;
						arg.arguments.resize(size);
						for (int item=0;item<size;++item) {
							arg.arguments[item]=parseArgument(scriptText, i, item==size-1?'}':',');
						}
					}break;
					default:
						break;
				}
				break;
			case '#':
				arg.type=FUNC;
				switch (scriptText[i++]) {
					case 'B':	arg.dataType=BOOLEAN; break;
					case 'N':	arg.dataType=NUMBER; break;
					case 'V':	arg.dataType=VECTOR; break;
					case 'Q':	arg.dataType=QUATERNION; break;
					case 'M':	arg.dataType=MATRIX; break;
					case 'P':	arg.dataType=POLYNOMIAL; break;
					case 'F':	arg.dataType=FIELD; break;
					case 'S':	arg.dataType=STRING; break;
					case 'O':	arg.dataType=OBJECT; break;
					case 'A':	arg.dataType=ARRAY; break;
					default:	arg.dataType=VOID; break;
				}
				++i;
				arg.modNumber=XPINSCompileStringTools::readInt(scriptText, i, 'F');
				arg.number=XPINSCompileStringTools::readInt(scriptText, i, '(');
				arg.isElemental = scriptText[i-1]=='.';
				++i;
				arg.arguments.resize(0);
				while (scriptText[i]!=')') {
					arg.arguments.resize(arg.arguments.size()+1);
					arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ',');
				}
				++i;
				break;
			case 'X':
				arg.type=BIF;
				switch (scriptText[i++]) {
					case 'B':	arg.dataType=BOOLEAN; break;
					case 'N':	arg.dataType=NUMBER; break;
					case 'V':	arg.dataType=VECTOR; break;
					case 'Q':	arg.dataType=QUATERNION; break;
					case 'M':	arg.dataType=MATRIX; break;
					case 'P':	arg.dataType=POLYNOMIAL; break;
					case 'F':	arg.dataType=FIELD; break;
					default:	arg.dataType=VOID; break;
				}
				arg.number=XPINSCompileStringTools::readInt(scriptText, i, '(');
				arg.isElemental = scriptText[i-1]=='.';
				++i;
				arg.arguments.resize(0);
				while (scriptText[i]!=')') {
					arg.arguments.resize(arg.arguments.size()+1);
					arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ',');
				}
				++i;
				break;
			case '?':{
				arg.type=EXP;
				switch (scriptText[i++]) {
					case 'B':
						arg.dataType=BOOLEAN;
						break;
					case 'N':
						arg.dataType=NUMBER;
						break;
					case 'V':
						arg.dataType=VECTOR;
						break;
					case 'Q':
						arg.dataType=QUATERNION;
						break;
					case 'M':
						arg.dataType=MATRIX;
						break;
					case 'P':
						arg.dataType=POLYNOMIAL;
						break;
					case 'F':
						arg.dataType=FIELD;
						break;
				}
				while (scriptText[i++]!='(');
				bool assign=false;
				char opChar=' ';
				opCode op=FindOp(scriptText, i, &assign, opChar, arg.dataType);
				arg.number=assign?1:0;
				if (op==INVALID) break;
				if(op==NOT||op==PREINCREMENT||op==PREDECREMENT){
					arg.arguments.resize(1);
					arg.arguments[0]=parseArgument(scriptText, i, ')');
				} else if(op==POSTINCREMENT||op==POSTDECREMENT){
					arg.arguments.resize(1);
					arg.arguments[0]=parseArgument(scriptText, i, opChar);
				} else if(op==COMPOSITION){
					arg.arguments.resize(1);
					arg.arguments[0]=parseArgument(scriptText, i, opChar);
					while (scriptText[i]!=')') {
						arg.arguments.resize(arg.arguments.size()+1);
						arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ',');
					}
				} else {
					arg.arguments.resize(2);
					arg.arguments[0]=parseArgument(scriptText, i, opChar);
					arg.arguments[1]=parseArgument(scriptText, i, ')');
				}
				while (scriptText[i++]!='?');
				arg.modNumber=op;
			}break;
			default:
				break;
		}
		while (scriptText[i]=='[') {
			++i;
			arg.subscripts.resize(arg.subscripts.size()+1);
			arg.subscripts[arg.subscripts.size()-1]=parseArgument(scriptText, i, ']');
			++i;
		}
		while (scriptText[i]!=expectedEnd&&scriptText[i]!=')'&&scriptText[i]!=',') ++i;
		return arg;
	}
	
	vector<Instruction> parseInstructionsForScript(string scriptText,int start,int stop)
	{
		vector<Instruction> instructions=vector<Instruction>();
		for(int i=start;i<stop;){
			while(i<scriptText.length()&&i!=0&&scriptText[i++]!='\n');
			if(i>scriptText.length()||i>=stop)break;
			Instruction instruction=Instruction();
			switch (scriptText[i]) {
				case '$'://Assignment
					instruction.type=ASSIGN;
					instruction.left=parseArgument(scriptText, i, '=');
					instruction.right=parseArgument(scriptText, i, '\n');
					break;
				case '#'://Void Function Call
				case '?':
				case 'X':
					instruction.type=VOIDFUNC;
					instruction.right=parseArgument(scriptText, i, '\n');
					break;
				case '@'://Control Flow
					++i;
					switch (scriptText[i++]) {
						case 'I':{
							instruction.type=IF;
							while (scriptText[i++]!=' ');
							instruction.right=parseArgument(scriptText, i, '{');
							int temp=i;
							XPINSCompileStringTools::skipBlock(scriptText, i,"{");
							instruction.block=parseInstructionsForScript(scriptText, temp, i);
						}break;
						case 'E':{
							if (scriptText[i]=='N')break;
							instruction.type=ELSE;
							bool elif=scriptText[i+1]=='I';
							while (scriptText[i]!='{'&&scriptText[i]!=' ')++i;
							if (elif) instruction.right=parseArgument(scriptText, i, '{');
							else {
								instruction.right.literalValue=new bool(true);
								instruction.right.type=CONST;
								instruction.right.dataType=BOOLEAN;
							}
							int temp=i;
							XPINSCompileStringTools::skipBlock(scriptText, i,"{");
							instruction.block=parseInstructionsForScript(scriptText, temp, i);
						}break;
						case 'W':{
							instruction.type=WHILE;
							while (scriptText[i++]!=' ');
							instruction.right=parseArgument(scriptText, i, '{');
							int temp=i;
							XPINSCompileStringTools::skipBlock(scriptText, i,"{");
							instruction.block=parseInstructionsForScript(scriptText, temp, i);
						}break;
						case 'L':{
							instruction.type=LOOP;
							while (scriptText[i++]!=' ');
							instruction.right=parseArgument(scriptText, i, '{');
							int temp=i;
							XPINSCompileStringTools::skipBlock(scriptText, i,"{");
							instruction.block=parseInstructionsForScript(scriptText, temp, i);
						}break;
						case 'R':
							instruction.type=RETURN;
							while (scriptText[i++]!=' ');
							instruction.right=parseArgument(scriptText, i, '\n');
							break;
						case 'B':
							instruction.type=BREAK;
							break;
						default:
							instruction.type=NULLTYPE;
							break;
					}
					break;
				default:
					instruction.type=NULLTYPE;
					break;
			}
			instructions.resize(instructions.size()+1);
			instructions[instructions.size()-1]=instruction;
		}
		return instructions;
	}
}

XPINSInstructions::InstructionSet XPINSCompiler::compileScript(string script)//Compile Script
{
	InstructionSet instructions=InstructionSet();
	if(!XPINSPrecompiler::checkVersion(script))return instructions;
	XPINSPrecompiler::removeComments(script);
	//cout<<script<<"\n\n";
	XPINSSyntaxProcesser::convertDotSyntax(script);
	XPINSComponentRenaming::replaceConstants(script);
	XPINSSyntaxProcesser::classifyComponents(script);
	XPINSComponentRenaming::renameFunctions(script);
	instructions.varSizes=XPINSComponentRenaming::renameVars(script);
	XPINSPrecompiler::cleanup(script);
	cout<<script<<"\n\n";
	//Parse Instructions
	instructions.instructions=XPINSInstructionAssembler::parseInstructionsForScript(script, 0, script.length());
	return instructions;
	
}