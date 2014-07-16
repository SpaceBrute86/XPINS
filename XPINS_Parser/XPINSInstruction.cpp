//
//  XPINSInstruction.cpp
//  XPINS
//
//  Created by Robbie Markwick on 7/13/14.
//
//

#include "XPINSInstruction.h"
#include <math.h>
#include "XPINSScriptableMath.h"

const int kPMajor=0;
const int kPMinor=12;
using namespace XPINSInstructions;
namespace XPINSInstructionsHelper {
	
	
	bool stringsMatch(int start,string first, string sec)
	{
		for(int i=0;i<sec.length();++i)
		{
			if(i+start>=first.length()||first[start+i]!=sec[i])
				return false;
		}
		return true;
	}
	int readInt(string script,int& i,char expectedEnd)//Read an Integer from the script
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
	
	bool checkVersion(string script)//Check to make sure script is compatible
	{
		for(int i=0;i<script.length();++i)
		{
			if(stringsMatch(i, script, "@PARSER"))
			{
				i+=8;
				int MAJOR=readInt(script,i, '.');
				int MINOR=readInt(script,i, ']');
				return (MAJOR==kPMajor&&MINOR<=kPMinor);
			}
		}
		return false;
	}
	int skipBlock(string script,int start)
	{
		int i=start;
		while(i<script.length()&&script[i++]!='{');
		int num=1;
		//skip block
		while(num>0){
			if(++i>=script.length())break;
			if(script[i]=='{')
				++num;
			else if(script[i]=='}')
				--num;
			else if(script[i]=='\"'){
				while (script[++i]!='\"') {
					if(script[i]=='\\')++i;
				}
			}
		}
		return i;
	}
	opCode FindOp(string script,int i, bool* assign,char& opChar,DataType type)
	{
		for (; i<script.length(); ++i) {
			if(script[i]=='(')
			{
				for (int i=1; i>0;) {
					++i;
					if(script[i]=='(')
						++i;
					else if(script[i]==')')
						--i;
				}
				++i;
			}
			opChar=script[i];
			switch (opChar) {
				case '|':
					if(type==BOOLEAN)return OR;
					break;
				case '&':
					if(type==BOOLEAN)return AND;
					if(type==POLYNOMIAL|type==FIELD){
						if(assign&&script[i+1]=='=')*assign=true;
						return COMPOSITION;
					}break;
				case '!':
					if(type==BOOLEAN)return script[i+1]=='='?NOTEQUAL:NOT;
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
						return MODULUS;
					}break;
				case ':':
					if(type==NUMBER||type==VECTOR) return EVALUATE;
					if(type==POLYNOMIAL||type==FIELD){
						if(assign&&script[i+1]=='=')*assign=true;
						return EVALUATE;
					}break;
			}
		}
		return INVALID;
	}

	Argument parseArgument(string scriptText,int start,char expectedEnd)
	{
		int i=start;
		Argument arg=Argument();
		while (scriptText[i]!='$'&&scriptText[i]!='~'&&scriptText[i]!='?'&&scriptText[i]!='#'&&scriptText[i]!='X')++i;
		switch (scriptText[i++]) {
			case '$':
				arg.type=VAR;
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
					case 'S':
						arg.dataType=STRING;
						break;
					case 'A':
						arg.dataType=ARRAY;
						break;
					default:
						arg.dataType=OBJECT;
						break;
				}
				arg.number=readInt(scriptText, i, expectedEnd);
				while (scriptText[i++]=='[') {
					arg.arguments.resize(arg.arguments.size()+1);
					arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ']');
					++i;
				}
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
						while(i<scriptText.length()&&scriptText[i]!=')'&&scriptText[i]!='e'&&scriptText[i]!='E')
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
						while(i<scriptText.length()&&scriptText[i]!=')')
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
								arg.number=1;
								break;
							case 'S':
								arg.number=2;
								break;
							default:
								arg.number=0;
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
						int temp=++i;
						for (;scriptText[i]!='|'&&scriptText[i]!=']'; ++i)//Find Column Count
						{
							if(scriptText[i]=='(')//Skip Parenthesis blocks
							{
								++i;
								for (int count=1; count>0; ++i) {
									if(scriptText[i]=='(')++count;
									else if(scriptText[i]==')')--count;
								}
							}
							if(scriptText[i]==',')++arg.number;
						}
						for (; scriptText[i]!=']'; ++i)//Find Row Count
						{
							if(scriptText[i]=='(')//Skip Parenthesis blocks
							{
								++i;
								for (int count=1; count>0; ++i) {
									if(scriptText[i]=='(')++count;
									else if(scriptText[i]==')')--count;
								}
							}
							if(scriptText[i]=='|')++arg.modNumber;
						}
						i=temp;
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
						for (arg.number=0;true;++arg.number){
							arg.arguments.resize(arg.number+1);
							arg.arguments[arg.number]=parseArgument(scriptText, i, '_');
							mons[arg.number].exponents=vector<unsigned int>();
							while (scriptText[i]=='_')
							{
								++i;
								unsigned int expIndex=0;
								switch (scriptText[i])
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
								if(scriptText[i]=='+'||scriptText[i]=='-'||scriptText[i]==')')
								{
									if(expIndex>mons[arg.number].exponents.size())mons[arg.number].exponents.resize(expIndex);
									mons[arg.number].exponents[expIndex-1]+=1;
									break;
								}
								if(scriptText[i]=='_')
								{
									if(expIndex>mons[arg.number].exponents.size())mons[arg.number].exponents.resize(expIndex);
									mons[arg.number].exponents[expIndex-1]+=1;
								}
								if(expIndex!=0)
								{
									if(expIndex>mons[arg.number].exponents.size())mons[arg.number].exponents.resize(expIndex);
									mons[arg.number].exponents[expIndex-1]+=readInt(scriptText,i, '_');
								}
							}
							if(scriptText[i]==')')break;
						}
						arg.literalValue=new vector<XPINSScriptableMath::Polynomial::Monomial>(mons);
					}break;
					default:
						break;
				}
				break;
			case '#':
				arg.type=FUNC;
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
					case 'S':
						arg.dataType=STRING;
						break;
					case 'A':
						arg.dataType=ARRAY;
						break;
					default:
						arg.dataType=OBJECT;
						break;
				}
				++i;
				arg.modNumber=readInt(scriptText, i, 'F');
				arg.number=readInt(scriptText, i, '(');
				arg.arguments.resize(0);
				while (scriptText[i]!=')'&& scriptText[++i]!=')') {
					arg.arguments.resize(arg.arguments.size()+1);
					arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ',');
					++i;
				}
				break;
			case 'X':
				arg.type=BIF;
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
				arg.number=readInt(scriptText, i, '(');
				arg.arguments.resize(0);
				while (scriptText[i]!=')'&& scriptText[++i]!=')') {
					arg.arguments.resize(arg.arguments.size()+1);
					arg.arguments[arg.arguments.size()-1]=parseArgument(scriptText, i, ',');
					++i;
				}
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
				++i;
				bool assign=false;
				char opChar=' ';
				opCode op=FindOp(scriptText, i, &assign, opChar, arg.dataType);
				arg.number=assign?1:0;
				if(op==NOT||op==PREINCREMENT||op==PREDECREMENT){
					arg.arguments.resize(1);
					arg.arguments[0]=parseArgument(scriptText, i, ')');
				} else if(op==POSTINCREMENT||op==POSTDECREMENT){
					arg.arguments.resize(1);
					arg.arguments[0]=parseArgument(scriptText, i, opChar);
				}
				else {
					arg.arguments.resize(2);
					arg.arguments[0]=parseArgument(scriptText, i, opChar);
					arg.arguments[1]=parseArgument(scriptText, i, ')');
				}
				arg.modNumber=op;
			}break;
			default:
				break;
		}
		while (scriptText[i]!=expectedEnd&&scriptText[i]!=')'&&scriptText[i]!=',') ++i;
		return Argument();
	}

	vector<Instruction> parseInstructionsForScript(string scriptText,int start,int stop)
	{
		vector<Instruction> instructions=vector<Instruction>();
		for(int i=start;i<stop;++i){
			while(i<scriptText.length()&&scriptText[i++]!='\n');
			if(i>scriptText.length()||i>=stop||stringsMatch(i, scriptText, "@END"))break;
			Instruction instruction=Instruction();
			switch (scriptText[i++]) {
				case '$'://Assignment
					instruction.type=ASSIGN;
					--i;
					instruction.left=parseArgument(scriptText, i, '=');
					instruction.right=parseArgument(scriptText, i, '\n');
					break;
				case '#':
				case '?':
				case 'X':
					instruction.type=VOIDFUNC;
					--i;
					instruction.right=parseArgument(scriptText, start, '\n');
					break;
				case '@':
					switch (scriptText[i++]) {
						case 'I':
							instruction.type=IF;
							while (scriptText[i++]!='[');
							instruction.right=parseArgument(scriptText, i, ']');
							instruction.block=parseInstructionsForScript(scriptText, i, skipBlock(scriptText, i));
							break;
						case 'E':
							instruction.type=ELSE;
							while (scriptText[i]!='{'&&scriptText[i]!='[')++i;
							if (scriptText[i]=='[') instruction.right=parseArgument(scriptText, i, ']');
							instruction.block=parseInstructionsForScript(scriptText, i, skipBlock(scriptText, i));
							break;
						case 'W':
							instruction.type=WHILE;
							while (scriptText[i++]!='[');
							instruction.right=parseArgument(scriptText, i, ']');
							instruction.block=parseInstructionsForScript(scriptText, i, skipBlock(scriptText, i));
							break;
						case 'L':
							instruction.type=LOOP;
							while (scriptText[i++]!='[');
							instruction.right=parseArgument(scriptText, i, ']');
							instruction.block=parseInstructionsForScript(scriptText, i, skipBlock(scriptText, i));
							break;
						case 'R':
							instruction.type=RETURN;
							while (scriptText[i++]!='[');
							instruction.right=parseArgument(scriptText, i, ']');
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
XPINSInstructions::InstructionSet XPINSInstructions::instructionsForScriptText(string script){
	int i=0;
	InstructionSet instructions=InstructionSet();
	instructions.varSizes.resize(10);
	//Check Version Compatibility
	for(;i<script.length()&&!XPINSInstructionsHelper::stringsMatch(i, script, "@PARSER");++i);
	if (i>=script.length())return InstructionSet();
	i+=8;
	int MAJOR=XPINSInstructionsHelper::readInt(script,i, '.');
	int MINOR=XPINSInstructionsHelper::readInt(script,i, ']');
	if (MAJOR!=kPMajor||MINOR>kPMinor)return InstructionSet();
	//Determine Variable Allocations
	while(i<script.length()&&!XPINSInstructionsHelper::stringsMatch(i, script, "@VARS"))++i;
	while(script[i++]!='B');
	instructions.varSizes[0]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='N');
	instructions.varSizes[1]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='V');
	instructions.varSizes[2]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='Q');
	instructions.varSizes[3]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='M');
	instructions.varSizes[4]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='P');
	instructions.varSizes[5]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='F');
	instructions.varSizes[6]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='S');
	instructions.varSizes[7]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='O');
	instructions.varSizes[8]=XPINSInstructionsHelper::readInt(script, i, ' ');
	while(script[i++]!='A');
	instructions.varSizes[9]=XPINSInstructionsHelper::readInt(script, i, ' ');
	//Parse Instructions
	while(i<script.length()&&!XPINSInstructionsHelper::stringsMatch(i, script, "@CODE"))++i;
	i+=5;
	instructions.instructions=XPINSInstructionsHelper::parseInstructionsForScript(script, i, script.length());
	return instructions;
}











