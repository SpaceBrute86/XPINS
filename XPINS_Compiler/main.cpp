//Copyright (c) 2013 Robert Markwick
//See the file license.txt for copying permission

//THIS IS THE COMPILER
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
const int kMajor=0;
const int kMinor=1;

void checkVersion(string*);
string removeComments(string);
string renameFunctions(string);
string renameTypes(string);
string renameVars(string);
string cleanUp(string);

string strRepresentationOfInt(int);
bool stringsMatch(int,string,string);
int readVarIndex(string,int *,char);

int main(int argc, const char * argv[])
{
	string fileName="";
	cout<<"Please enter the full path minus extension for the script you would like to compile"<<endl;
	cout<<"Make sure that the file extension is .XPINS"<<endl;
	cin>>fileName;
	string fileExt=".XPINS";//File extension for uncompiled XPINS: XPINS
	string outExt=".XPINSX";//File extension for compiled XPINS: XPINSX
	//read in file
	ifstream inFile;
	inFile.open(fileName+fileExt);
	if (inFile.fail()) {
		cerr << "unable to open file for reading" << endl;
		exit(1);
	}
	string scriptText="";
	char ch;
	while (inFile.get(ch)) {
		scriptText+=ch;
	}
	inFile.close();
	//Compile Script
	cout<<endl<<"Validating Version..."<<endl;
	checkVersion(&scriptText);
	cout<<"Version Compatilbe!";
	cout<<endl<<"About To Compile Script:\n"<<scriptText<<endl;
	scriptText=removeComments(scriptText);
	cout<<endl<<"Comments and ';'s Removed:\n"<<scriptText<<endl;
	scriptText=renameFunctions(scriptText);
	cout<<endl<<"Functions Renamed:\n"<<scriptText<<endl;
	scriptText=renameTypes(scriptText);
	cout<<endl<<"Variable Types Renamed:\n"<<scriptText<<endl;
	scriptText=renameVars(scriptText);
	cout<<endl<<"Variables Renamed:\n"<<scriptText<<endl;
	scriptText=cleanUp(scriptText);
	cout<<endl<<"Cleaned Up:\n"<<scriptText;
	//Write Output to file

	ofstream outFile;
	outFile.open(fileName+outExt);
	if (outFile.fail()) {
		cerr << "unable to open file: "<<fileName<<outExt<<" for writing" << endl;
		exit(1);
	}
	cout<<endl<<endl<<endl<<"Writing To File:\n"<<fileName<<outExt;
	for (int i=0;i<scriptText.length();i++) {
		ch=scriptText[i];
		outFile<<ch;
	}
	outFile.close();
    return 0;
}

void checkVersion(string* script){
	string input=*script;
	for(int i=0;i<input.length();i++){
		if(input[i]=='@'&&input[i+1]=='C'&&input[i+2]=='O'&&input[i+3]=='M'&&input[i+4]=='P'&&input[i+5]=='I'&&input[i+6]=='L'&&input[i+7]=='E'&&input[i+8]=='R'){
			while(i<input.length()&&input[i]!='[')i++;
			if(i==input.length()){
				cout<<"SCRIPT MISSING VERSION. EXITING";
				exit(1);
			}
			int MAJOR=readVarIndex(input, &i, '.');
			int MINOR=readVarIndex(input, &i, ']');
			if(MAJOR!=kMajor){
				cout<<"INCOMPATIBLE VERSION. EXITING";
				exit(1);
			}
			if(MINOR<kMinor){
				cout<<"INCOMPATIBLE VERSION. EXITING";
				exit(1);
			}
			while(i<input.length()&&input[i]!='\n')i++;
			i++;
			string output="";
			while (i<input.length()) {
				output+=input[i++];
			}
			*script=output;
			return;
		}
	}
	cout<<"SCRIPT MISSING VERSION. EXITING";
	exit(1);
}
//COMPILE STEPS
string removeComments(string input){
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
	return output;
}

string renameFunctions(string input){
	string output="";
	char ch;
	int i=0;
	//Locate Function block
	while(input[i]!='@'||input[i+1]!='F'){
		i++;
	}
	if(input[i+1]!='F'||input[i+2]!='U'||input[i+3]!='N'||input[i+4]!='C'){
		cerr<<"Invalid Script: Missing @FUNC.\nEXITING";
		exit(1);
	}
	i+=5;
	string intermediate1="";
	string intermediate2="";
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
			exit(1);
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
			if(stringsMatch(j, intermediate1, functionName)){
				intermediate2+='F';
				intermediate2+=strRepresentationOfInt(x);
				while(intermediate1[j]!='('){//Find '('
					j++;
				}
			}
		}
		//Get ready for next loop
		intermediate1=""+intermediate2;
		x++;
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
	return output;
}
string renameTypes(string input){
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
	return output;
}
string renameVars(string input){
	string output="";
	char ch;
	int i=0;
	//Locate Code block
	while(input[i]!='@'||input[i+1]!='C'){
		i++;
	}
	if(input[i+1]!='C'||input[i+2]!='O'||input[i+3]!='D'||input[i+4]!='E'){
		cerr<<"Invalid Script: Missing @CODE.\nEXITING";
		exit(1);
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
			while(j<intermediate1.length()){
				//CHECK FOR @END
				if(intermediate1[j]=='@'&&intermediate1[j+1]=='E'&&intermediate1[j+2]=='N'&&intermediate1[j+3]=='D'){
					intermediate2+="@END";
					break;
				}
				//Find start of Var
				if(intermediate1[j]=='$'){
					intermediate2+='$';
					j++;
					//Check for match
					if(stringsMatch(j, intermediate1, varName)){
						intermediate2+=varType;
						intermediate2+=strRepresentationOfInt(varNum);
						while(intermediate1[j]!=')'&&intermediate1[j]!='='&&intermediate1[j]!=','&&intermediate1[j]!=']'){//Find '('
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
	return output;
}
string cleanUp(string input){
	string output="";
	string intermediate1="";
	//Remove excess whitespace
	for(int i=0;i<input.length();i++){
		if(input[i]!='\n'||input[i+1]!='\n')intermediate1+=input[i];
	}
	//Double Check Ending (strip after @END)
	for(int i=0;i<intermediate1.length();i++){
		if(intermediate1[i]=='@'&&intermediate1[i+1]=='E'&&intermediate1[i+2]=='N'&&intermediate1[i+3]=='D'){
			output+="@END\n";
			break;
		}
		output+=intermediate1[i];
	}
	return output;
}
//UTILTY FUNCTIONS
int readVarIndex(string scriptText,int *startIndex,char expectedEnd){
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
string strRepresentationOfInt(int x){
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
bool stringsMatch(int start,string first, string sec){
	for(int i=start;i<start+sec.length();i++){
		if(i==first.length()) return false;
		if(first[i]!=sec[i-start])return false;
	}
	return true;
}