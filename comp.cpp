#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstdlib>
using namespace std;
ofstream g;
ifstream f;
int lineCount=0;
void syntaxError()
{
    cout << "ERROR" << endl;
    g << "ERROR" << endl;
    exit(0);
}
bool isNumber(string st)
{
    return '0' <= st[0] && '9' >= st[0];
}
// WELCOME TO MY COFFEE INDUCED ASSEMBLY EXTRAVAGANZA MY FRIEND!!!
// TOGETHER WE WILL DISCOVER THE SECRETS OF PARSING AND HEXADECIMAL NUMBERS!!
// BUCKLE UP!!!!!!

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
map<string,string> varmap; // WE REPLACE VARS WITH $1,$2....
stack<string> freeSpaces;
void BAD()
{
	
}
void DebugOutputVector(vector<string> v)
{
    cout << "Size: " << (int)v.size() << " Vector: ";
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
        DebugOutputVector(postfix);
		if(infix[i] == ",")
		{
            cout << "COMMA" << endl;
			while(st.top()!="pow(")
			{
				postfix.push_back(st.top());
				st.pop();
				if(st.empty()) BAD();
			}
		}
		else if(infix[i] == "*")
		{
            cout << "STAR" << endl;
			if(st.empty() || st.top()=="+" || st.top()=="*" || st.top()=="(" || st.top()=="pow(")
				st.push("*");
		}
		else if(infix[i] == "+")
		{
			// empty = BAD
            cout << "PLUS" << endl;
			while(!st.empty() && st.top()=="*")
			{
				postfix.push_back("*");
				st.pop();
			}
			if(st.empty() || st.top()=="+" || st.top()=="(" || st.top()=="pow(")
				st.push("+");
		}
		else if(infix[i] == "(" || infix[i] == "pow(")
		{
            cout << "OPEN Paranthesis or pow" << endl;
			st.push(infix[i]);
		}
		else if(infix[i] == ")")
		{
            cout << "CLOSE PARANTHESIS" << endl;
			//BAD
			while(st.top()!="(" && st.top()!="pow(")
			{
				postfix.push_back(st.top());
				st.pop();
			}
			if(st.top()=="pow("){
				st.push("^");
			}
		}
        else
        {
            cout << "ALPHANUMERIC" << endl;
            postfix.push_back(infix[i]);
        }
	}
    //Clear operator stack
    while(!st.empty())
    {
        postfix.push_back(st.top());
        st.pop();
    }
    cout << "INFIX POSTFIX OVER, LAST RESULT:" << endl;
    DebugOutputVector(postfix);
	return postfix;
}

vector<string> tokenizeExpression(string expr) // Tokenize the expression for parsing.
{
    cout << "TOKENOZE THIS:" << expr << endl;
    expr+=' '; // Add a space to end so process can push out the last token.
	vector <string> tokens; // Generated tokens go here
	string temp = ""; // Current token that's being produced.
	for(int i=0;i<(int)expr.length();i++)
	{
        DebugOutputVector(tokens);
        cout << "Temp: " << temp << endl;
        if(expr[i] == ' ') // If there is a space, skip it and finish the current token. BUT if it is pow maybe there will be a ( so keep it there buddy.
        {
            cout << "SPACE" << endl;
            if(temp.length() > 0 && temp!="pow") {
                tokens.push_back(temp);
                temp = "";
            }
        }
		if(expr[i] == '+' || expr[i] == '*' || expr[i] == '(' || expr[i] == ')' || expr[i] == ',') // Symbols, if something in temp finish that token.
		{
            cout << "OPERATOR: " <<  expr[i] <<endl;
			if(temp == "pow" && expr[i] == '(')
			{
				tokens.push_back(temp+"(");
				temp = "";
			}
			else
            {
                if(temp.length() > 0)
                {
                    tokens.push_back(temp);
                    temp = "";
                }
                tokens.push_back(string(1, expr[i]));
            }
        }
		if(('a' <= expr[i] && 'z' >= expr[i]) || ('A' <= expr[i] && 'Z' >= expr[i]) || ('0' <= expr[i] && '9' >= expr[i])) // Alphanumeric, add to temp.
		{
            cout << "ALPHANUMERIC: " <<  expr[i] << endl;
			temp+=expr[i];
		}
	}

    cout << "TOKENOZE OVER LAST RESULT:" << "\n";
    DebugOutputVector(tokens);
    return tokens;
}
void printMov(string dest,string source){
    if(isNumber(source)) //IS SOURCE NUMBER?
    {
        g<<"mov w "+ dest+"l,"+ source.substr(4,4) << endl; //Move source to dest
        g<<"mov w "+ dest+"u,"+ source.substr(0,4) << endl;
    } else
    {
        g<<"mov w "+ dest+"l,w "+source+"l" << endl; //Move source to dest
        g<<"mov w "+ dest+"u,w "+source+"u" << endl;
    }

}
void printAdd(string dest,string source){
	printMov("ad1",dest);//Move dest to adder
    printMov("ad2",source); //Move source to adder
    g<<"call comp_add" << endl; //Call adder
    printMov(dest,"ad1");//Move result to dest
}
void printMul(string source){
	g<<"mul "+source << endl;
}
void printPow(string dest,string source){
	// TODO
}
void printOut(string output){ // TODO IF ONLY NUMBER?
    g<<"mov w[outhex],"+output+"l\nmov w[outhex+2],"+output+"u\ncall print_hex\n";
}
string createNewSpace() // Underscore for stack spaces.
{
		string newstr = "_" + to_string(++createdSpaceCount);
		return newstr;
}
string createNewVar() // Dollar for actual variables from comp code.
{
    string newstr = "$" + to_string(++createdVariableCount);
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

// TODO ADD LINE COUNT FOR COMPILER ERROR, ADD ERRORS FOR EXTRA = charss
string executePostfix(ofstream &g,vector <string> &postfix)
{
    cout << "GONNA PRINT VECTORS" << "\n";
    DebugOutputVector(postfix);
    stack<string> myst;
    for(int i=0;i<postfix.size();i++) {
        //OPERATIONS
        if (postfix[i] == "+") {
            cout << "Plus" << endl;
            //BAD, MORE BAD IF TWO THINGS ARE OPERANDS
            string l = myst.top();
            myst.pop();
            string r = myst.top();
            myst.pop();
            string result = giveSpace();

            printMov(result, l);
            printAdd(result, r);
            myst.push(result);
            freeSpace(l);
            freeSpace(r);
            continue;
        }
        if (postfix[i] == "*") {
            //BAD, MORE BAD IF TWO THINGS ARE OPERANDS
            string l = myst.top();
            myst.pop();
            string r = myst.top();
            myst.pop();
            string result = giveSpace();

            printMov("ax", l);
            printMov(result, r);
            printMul(result);
            printMov(result, "ax");
            myst.push(result);
            freeSpace(l);
            freeSpace(r);
            continue;
        }
        if (postfix[i] == "^") {
            //BAD, MORE BAD IF TWO THINGS ARE OPERANDS
            // TODO
/*
            string l = myst.top();
            myst.pop();
            string r = myst.top();
            myst.pop();
            string result = giveSpace();

            printMov(g, "ax", l);
            printMov(g, result, r);
            printMul(g, result);
            printMov(g, result, "ax");
            myst.push(result);
            freeSpace(l);
            freeSpace(r);
            continue;*/
        }

        // CONSTANT NUMBER
        if (isNumber(postfix[i])) {
            cout << "NIMBER" << endl;
            if(postfix[i].length()>8) // Error if bigger than 32 bit. 0AAAAAAAA is wrong since bigger than 32 bit. IF THIS IS A PROBLEM I WILL OBJECT SINCE THIS WASNT CLARIFIED. - BURAK CETIN
                syntaxError();
            cout << postfix[i] << endl;
            string num8 = "";
            cout << num8 << endl;
            for(int j=0;j<8-postfix[i].length();j++)
                num8.push_back('0');
            cout << num8 << endl;
            num8=num8 + postfix[i];
            cout << num8 << endl;
            cout << "NIMBER" << endl;
            cout << "Constant Number: " << num8 << endl;
            myst.push(num8);
        }
            // VARIABLE
        else {
            if (varmap.find(postfix[i]) == varmap.end()) // If this is the first time we counter this variable add it to map with $index form of it.
                varmap[postfix[i]] = createNewVar();
            myst.push(varmap[postfix[i]]);
        }
    }
    cout << "HEYO: " << myst.top() <<endl;
    if(myst.size()!=1)
        syntaxError();
    if(!(myst.top()[0]=='_' || myst.top()[0]=='$' || isNumber(myst.top())))
        syntaxError();
    return myst.top();
}
void initAssembly()
{
    // Hexadecimal print function
    g << "print_hex:\npusha\nmov bx, offset outhex\nmov dx, w[bx]\ncall print_half\nmov bx, offset outhex\nmov dx, w[bx+2]\ncall print_half\nMOV AH,02\nMOV DL,10\nINT 21h\npopa\nret\nprint_half:\npusha\nmov cx,4\nchar_loop:\ndec cx\nmov ax,dx\nshr dx,4\nand ax,0fh\nmov bx, offset string_output\nadd bx, cx\nmov b[bx],al\ncmp ax,10\njl is_num\nadd b[bx],7\nis_num:\nadd b[bx],30h\ncmp cx,0\nje print_half_done\njmp char_loop\nprint_half_done:\nmov bx,offset string_output+4\nmov b[bx],'$'\nmov dx, offset string_output\nmov ah,09\nint 21h\npopa\nret\nstring_output db 5 DUP 0\nouthex dw 2 DUP 0\n";
    // 32 bit adder function
    g << "comp_add:\npusha\nmov bx,w ad1l\nadd bx,w ad2l\njnc add_no_carry\ninc w ad1u\nadd_no_carry:\nmov w ad1l,bx\nmov bx,w ad1u\nadd bx,w ad2u\nmov w ad1u,bx\npopa\nret\nad1u dw 0\nad1l dw 0\nad2u dw 0\nad2l dw 0\n";
}
void finalizeAssembly()
{
    g << "mov ah,4ch\nmov al,00\nint 21h\n"; //Close the program
    for(int i=1;i<=createdSpaceCount;i++)
    {
        g << '_' << i << "u dw 0\n_" << i << "l dw 0\n";
    }
    for(int i=1;i<=createdVariableCount;i++)
    {
        g << '$' << i << "u dw 0\n$" << i << "l dw 0\n";
    }
}
int main(int argc,char* argv[]){
 	string file=(string(argv[1]));
	string newfilename=(file.substr(0,file.find_last_of('.'))+".asm");
	g.open(newfilename);
    f.open(argv[1]);
    initAssembly();
    string line;
    cout << "BEFORE WHILE" << "\n";
    while (getline(f, line)){
        cout << "Line i got: "<< line << "\n";
        ++lineCount;
        string found;
		unsigned long tem=line.find("=");
		if(tem==string::npos)
		{
            cout << "I DONT KNOW HOW TO PRINT" << "\n";
            vector<string> postfix=infixToPostfix(tokenizeExpression(line));
            string tempSpace=giveSpace();
            printMov(tempSpace,executePostfix(g,postfix));
            printOut(tempSpace);
            freeSpace(tempSpace);
		}
		else
		{
			//Assuming single space around '=' DONT DO THIS THIS WRONG
            cout << "IT IS ASSIGNMENT" << endl;
            vector<string> postfix=infixToPostfix(tokenizeExpression(line.substr(tem+1)));
            vector<string> tokenizedLeft=tokenizeExpression(line.substr(0,tem));
            if(tokenizedLeft.size()!=1)
                syntaxError();
            if (varmap.find(tokenizedLeft[0]) == varmap.end()) // If this is the first time we counter this variable add it to map with $index form of it.
                varmap[tokenizedLeft[0]] = createNewVar();
            printMov(varmap[tokenizedLeft[0]],executePostfix(g,postfix));
		}
		
        // cout << line << std::endl;
    }
    finalizeAssembly();
    cout << "AFTER A WHILE YOU DID IT" << "\n";
}