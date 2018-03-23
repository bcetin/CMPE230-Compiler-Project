#include <cstdio>
#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
using namespace std;
/*void split1(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
              istream_iterator<string>(),
              back_push_backer(cont));
}*/

// YOU CANT HAVE VARIABLES STARTING WITH _
// X = x assemblyde bunun cozulmesi lazim, MAPE CEVIR HEPSINI YENIDEN ADLANDIR
int createdVariableCount=0,createdSpaceCount=0; // Variable = Original variables from comp code. Space = Temporary variables for our calculations
map<string,string> varmap; // WE REPLACE VARS WITH $1 and go up.
stack<string> freeSpaces;
void BAD()
{
	
}
void DebugOutputVector(vector<string> v)
{
    cout << (int)v.size() + "BEFORE I WAS: ";
    for(int i=0;i<v.size();i++)
        cout<< "\""+ v[i] +"\" ";
    cout << endl;
}
vector<string> infixToPostfix(vector<string> infix)
{ // + * , pow( ( )
    cout << "INFIX POSTFIX" << "\n";
	vector<string> postfix;
	stack<string> st;
	for(int i=0;i<(int)infix.size();i++)
	{
		if(infix[i] == ",")
		{
			while(st.top()!="pow(")
			{
				postfix.push_back(st.top()+"");
				st.pop();
				if(st.empty()) BAD();
			}
		}
		if(infix[i] == "*")
		{
			if(st.empty() || st.top()=="+" || st.top()=="*" || st.top()=="(" || st.top()=="pow(")
				st.push("*");
		}
		if(infix[i] == "+")
		{
			// empty = BAD
			while(st.top()=="*")
			{
				postfix.push_back("*");
				st.pop();
			}
			if(st.empty() || st.top()=="+" || st.top()=="(" || st.top()=="pow(")
				st.push("+");
		}
		if(infix[i] == "(" || infix[i] == "pow(")
		{
			st.push(infix[i]);
		}
		if(infix[i] == ")")
		{
			//BAD
			while(st.top()!="(" && st.top()!="pow(")
			{
				postfix.push_back(st.top()+"");
				st.pop();
			}
			if(st.top()=="pow("){
				st.push("^");
			}
		}
	}
    cout << "INFIX POSTFIX OVER" << "\n";
	return postfix;
}

vector<string> tokenizeExpression(string expr) // Tokenize the expression for parsing.
{
    cout << "TOKENOZE" << "\n";
	vector<string> tokens; // Generated tokens go here.
    tokens.push_back("TEST");
	string temp = ""; // Current token that's being produced.
	for(int i=0;i<(int)expr.length();i++)
	{
        DebugOutputVector(tokens);
        if(expr[i] == ' ') // If there is a space, skip it and finish the current token. BUT if it is pow maybe there will be a ( so keep it there buddy.
        {
            if(temp.length() > 0 && temp!="pow") {
                tokens.push_back(temp);
                temp = "";
            }
        }
		if(expr[i] == '+' || expr[i] == '*' || expr[i] == '(' || expr[i] == ')' || expr[i] == ',') // Symbols, if something in temp finish that token.
		{
			if(temp == "pow" && expr[i] == '(')
			{
				tokens.push_back(temp+"(");
				temp = "";
			}
			else if(temp.length() > 0)
			{
				tokens.push_back(temp);
				temp = "";
			}
			else
				tokens.push_back(expr[i]+"");
		}
		if(('a' <= expr[i] && 'z' >= expr[i]) || ('A' <= expr[i] && 'Z' >= expr[i]) || ('0' <= expr[i] && '9' >= expr[i])) // Alphanumeric, add to temp.
		{
			temp = temp + expr[i];
		}
	}
    cout << "TOKENOZE OVER" << "\n";
    return tokens;
}
void printMov(ofstream& g,string dest,string source){
	g<<"mov "+dest+","+source;
}
void printAdd(ofstream& g,string dest,string source){
	g<<"add "+dest+","+source;
}
void printMul(ofstream& g,string source){
	g<<"mul "+source;
}
void printPow(ofstream& g,string dest,string source){
	// TODO
}
string createNewSpace() // Underscore for stack spaces.
{
		string newstr = "_" + ++createdSpaceCount;
		return newstr;
}
string createNewVar() // Dollar for actual variables from comp code.
{
    string newstr = "$" + ++createdVariableCount;
    return newstr;
}
string giveSpace()
{
	if(freeSpaces.empty()){
		return createNewSpace();
	}
	else{
		string temp = freeSpaces.top();
		freeSpaces.pop();
		return temp;
	}
}
void freeSpace(string name)
{
	if(name[0]!='_') return;
	freeSpaces.push(name);
}
bool IsNumber(string st)
{
    return '0' <= st[0] && '9' >= st[0];
}
// TODO ADD LINE COUNT FOR COMPILER ERROR

int main(int argc,char* argv[]){
 	string file=(string(argv[1]));
	string newfilename=(file.substr(0,file.find_last_of('.'))+".asm");
	ofstream g(newfilename);
    ifstream f(argv[1]);
    string line;
    cout << "BEFORE WHILE" << "\n";
    while (getline(f, line)){
        cout << "Line i got: "<< line << "\n";
        string found;
		unsigned long tem=line.find("=");
		if(tem==string::npos)
		{
            cout << "WHOA IT IS NOT ASSIGNMENT" << "\n";
			
			//TODO Tell assembly to print values
		}
		else
		{
			//Assuming single space around '=' DONT DO THIS THIS WRONG
            cout << "IT IS ASSIGNMENT" << "\n";
			vector<string> postfix=infixToPostfix(tokenizeExpression(line.substr(tem+1)));

            cout << "GONNA PRINT VECTORS" << "\n";
            DebugOutputVector(postfix);
			stack<string> myst;
			for(int i=0;i<postfix.size();i++){
                //OPERATIONS
				if(postfix[i]=="+"){
					//BAD, MORE BAD IF TWO THINGS ARE OPERANDS
					string l=myst.top();
					myst.pop();
					string r=myst.top();
					myst.pop();
					string result = giveSpace();
					
					printMov(g,result,l);
					printAdd(g,result,r);
					myst.push(result);
					freeSpace(l);
					freeSpace(r);
					continue;
				}
				if(postfix[i]=="*"){
					//BAD, MORE BAD IF TWO THINGS ARE OPERANDS
					string l=myst.top();
					myst.pop();
					string r=myst.top();
					myst.pop();
					string result = giveSpace();
					
					printMov(g,"ax",l);
					printMov(g,result,r);
					printMul(g,result);
					printMov(g,result,"ax");
					myst.push(result);
					freeSpace(l);
					freeSpace(r);
					continue;
				}
				if(postfix[i]=="^"){
					//BAD, MORE BAD IF TWO THINGS ARE OPERANDS
					// TODO
					
					string l=myst.top();
					myst.pop();
					string r=myst.top();
					myst.pop();
					string result = giveSpace();
					
					printMov(g,"ax",l);
					printMov(g,result,r);
					printMul(g,result);
					printMov(g,result,"ax");
					myst.push(result);
					freeSpace(l);
					freeSpace(r);
					continue;
				}

				// CONSTANT NUMBER
                if(IsNumber(postfix[i]))
                {
                    myst.push(postfix[i]);
                }
                // VARIABLE
				else{
					if(varmap.find(postfix[i])==varmap.end()) // If this is the first time we counter this variable add it to map with $index form of it.
                        varmap[postfix[i]]=createNewVar();
                    myst.push(varmap[postfix[i]]);
				}


			}

		}

		/*vector<char> words;
        split1(line,words);*/
		
        // cout << line << std::endl;
    }
    cout << "AFTER A WHILE" << "\n";
}