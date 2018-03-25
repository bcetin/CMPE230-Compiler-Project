#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <map>
using namespace std;

ofstream g; // Output .asm file
ifstream f; // Input .co file

int lineCount=0; // Read line count for syntax errors
void syntaxError() //TODO Syntax error
{
    cout << "ERROR" << endl;
    g << "ERROR" << endl;
    exit(0); // Terminate the program
}

bool isNumber(string st) //Checks if a string is a constant number or not. Ex. 005A8 is a number but A842 is a variable by the language definition.
{
    return '0' <= st[0] && '9' >= st[0];
}

int createdVariableCount=0,createdSpaceCount=0; // Variable = Original variables from Comp source code. Space = Temporary variables to be kept in stack for our calculations.
map<string,string> varmap; // We replace the variables in the Comp source code with $1l:$1u,$2l:$2u,$2l:$2u.... in our assembly version.
stack<string> freeSpaces; // If a temporary variable is pulled out of stack we put it back in here to prevent unnecessary memory allocations.

void initAssembly()
{
    // A hexadecimal print function for a86 assembly
    g << "jmp main\nprint_hex:\npusha\nmov bx, offset outhex\nmov dx, w[bx]\ncall print_half\nmov bx, offset outhex\nmov dx, w[bx+2]\ncall print_half\nMOV AH,02\nMOV DL,10\nINT 21h\npopa\nret\nprint_half:\npusha\nmov cx,4\nchar_loop:\ndec cx\nmov ax,dx\nshr dx,4\nand ax,0fh\nmov bx, offset string_output\nadd bx, cx\nmov b[bx],al\ncmp ax,10\njl is_num\nadd b[bx],7\nis_num:\nadd b[bx],30h\ncmp cx,0\nje print_half_done\njmp char_loop\nprint_half_done:\nmov bx,offset string_output+4\nmov b[bx],'$'\nmov dx, offset string_output\nmov ah,09\nint 21h\npopa\nret\nstring_output db 5 DUP 0\nouthex dw 2 DUP 0\n";
    // 32 bit adder function for a86 assembly
    g << "comp_add:\npusha\nmov bx,w ad1l\nadd bx,w ad2l\njnc add_no_carry\ninc w ad1u\nadd_no_carry:\nmov w ad1l,bx\nmov bx,w ad1u\nadd bx,w ad2u\nmov w ad1u,bx\npopa\nret\nad1u dw 0\nad1l dw 0\nad2u dw 0\nad2l dw 0\n";
    // 32 bit multiplication function for a86 assembly
    g << "comp_mul:\npusha\nmov ax,w mu1l\nmul w mu2l\nmov w varu,dx\nmov w varl,ax\nmov ax,w mu1l\nmul w mu2u\nadd ax,w varu\nmov w varu,ax\nmov ax,w mu1u\nmul w mu2l\nadd ax, w varu\nmov w varu,ax\nmov w mu1u,w varu\nmov w mu1l,w varl\npopa\nret\nvaru dw 0\nvarl dw 0\nmu1u dw 0\nmu1l dw 0\nmu2u dw 0\nmu2l dw 0\n";
    // 32 bit power function for a86 assembly
    g << "comp_pow:\npusha\ncmp w po2l,1h\njne pow_skip\npopa\nret\npow_skip:\nmov bx,w po2l\nmov w paddu,w po1u\nmov w paddl,w po1l\nmov w po1l,1h\nmov w po1u,0h\npow_loop:\nmov ax,bx\nand ax,1h\njz pow_not_one\nmov w mu1l, w po1l\nmov w mu1u, w po1u\nmov w mu2l, w paddl\nmov w mu2u, w paddu\ncall comp_mul\nmov  w po1l,w mu1l\nmov  w po1u,w mu1u\npow_not_one:\nmov w mu1l, w paddl\nmov w mu1u, w paddu\nmov w mu2l, w paddl\nmov w mu2u, w paddu\ncall comp_mul\nmov  w paddl,w mu1l\nmov  w paddu,w mu1u\nshr bx,1\njnz pow_loop\npopa\nret\npo1u dw 0\npo1l dw 0\npo2u dw 0\npo2l dw 0\npaddu dw 0\npaddl dw 0\n";
    // Main location
    g << "main:\n";
}
void finalizeAssembly()
{
    g << "mov ah,4ch\nmov al,00\nint 21h\n"; //Close the program
    //Allocate space for the variables used in both our stack and in source code. Each variable has a 16 bit lower and a 16 bit upper part. Example: _5u:_5l
    for(int i=1;i<=createdSpaceCount;i++) // Our stack variables.
    {
        g << '_' << i << "u dw 0\n_" << i << "l dw 0\n";
    }
    for(int i=1;i<=createdVariableCount;i++) // Variables from .co source code.
    {
        g << '$' << i << "u dw 0\n$" << i << "l dw 0\n";
    }
}

//A function used for debugging.
/*void DebugOutputVector(vector<string> v)
{
    cout << "Size: " << (int)v.size() << " Vector: ";
    for(int i=0;i<v.size();i++)
        cout<< "\""+ v[i] +"\" ";
    cout << endl;
}*/

vector<string> tokenizeExpression(string expr) // Tokenize an infix expression for parsing. Tokens are stored as strings in a std::vector.
{
    expr+=' '; // Add a space to end so process can push out the last token.
    vector <string> tokens; // Generated tokens go here
    string temp = ""; // Current token that's being generated.
    for(int i=0;i<(int)expr.length();i++)
    {
        if(expr[i] == ' ') // Case for a space, skip it and finish the current token. But if it is "pow" probably there will be a "(" so keep it there.
        {
            if(temp.length() > 0 && temp!="pow") {
                tokens.push_back(temp);
                temp = "";
            }
        }
        else if(expr[i] == '+' || expr[i] == '*' || expr[i] == '(' || expr[i] == ')' || expr[i] == ',') // Case for operator symbols, if something in temp finish generating that token as well.
        {
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
        else if(('a' <= expr[i] && 'z' >= expr[i]) || ('A' <= expr[i] && 'Z' >= expr[i]) || ('0' <= expr[i] && '9' >= expr[i])) // Case for alphanumeric characters, add to temp for generating a token.
        {
            temp+=expr[i];
        }
        else // Unknown character in source code. Syntax Error.
            syntaxError();
    }
    return tokens;
}
vector<string> infixToPostfix(vector<string> infix) // Converts a tokenized-infix expression to a postfix expression.
{
    vector<string> postfix; // The postfix expression we are producing.
	stack<string> st; // Operator stack. Follows the logic presented here. http://condor.depaul.edu/ichu/csc415/notes/notes9/Infix.htm
	for(int i=0;i<(int)infix.size();i++)
	{
		if(infix[i] == ",") // When a comma appears pop everything in stack until you find a pow( token.
		{
			while(st.top()!="pow(")
			{
				postfix.push_back(st.top());
				st.pop();
				if(st.empty()) syntaxError(); // If no corresponding pow( appears there is a syntax error.
			}
		}
		else if(infix[i] == "*") // When a comma appears just push it into the stack since it has a higher precedence than +.
		{
            st.push("*");
		}
		else if(infix[i] == "+") // When a plus appears pop out all *'s from the stack since + has a lower precedence.
		{
			while(!st.empty() && st.top()=="*")
			{
				postfix.push_back("*");
				st.pop();
			}
            st.push("+");
		}
		else if(infix[i] == "(" || infix[i] == "pow(") // When a ( or pow( appears just put it into the stack.
		{
			st.push(infix[i]);
		}
		else if(infix[i] == ")") // When a ) appears pop the stack until you get a ( or pow(.
		{
            if(st.empty())
                syntaxError();
			while(st.top()!="(" && st.top()!="pow(")
			{
				postfix.push_back(st.top());
				st.pop();
                if(st.empty())
                    syntaxError();
			}
			if(st.top()=="pow("){
				postfix.push_back("^");
			}
            st.pop();
		}
        else // If the token is not any of our special characters, it is a variable or a constant. Pass it to the postfix expression.
        {
            postfix.push_back(infix[i]);
        }
	}
    //Move the remaining operators stack to the postfix expression.
    while(!st.empty())
    {
        postfix.push_back(st.top());
        st.pop();
    }
	return postfix;
}
void printMov(string dest,string source){ //Print a command that moves the value in source to dest to our .asm output file.
    if(isNumber(source)) //If source is a constant number split it accordingly.
    {
        g<<"mov w "+ dest+"l,0"+ source.substr(4,4)+"h" << endl;
        g<<"mov w "+ dest+"u,0"+ source.substr(0,4)+"h" << endl;
    } else
    {
        g<<"mov w "+ dest+"l,w "+source+"l" << endl;
        g<<"mov w "+ dest+"u,w "+source+"u" << endl;
    }
}
void printAdd(string dest,string source){ //Print a command that adds the value in source to dest to our .asm output file.
	printMov("ad1",dest); //Move dest to adder
    printMov("ad2",source); //Move source to adder
    g<<"call comp_add" << endl; //Call adder
    printMov(dest,"ad1");//Move result to dest
}
void printMul(string dest,string source){ //Print a command that multiplies the value in source with dest to our .asm output file.
	printMov("mu1",dest); //Move dest to multiplier
    printMov("mu2",source); //Move source to multiplier
    g << "call comp_mul\n"; //Call multiplier
    printMov(dest,"mu1"); //Move result to dest
}
void printPow(string dest,string source){ //Print a command that calculates source^dest and places it in dest to our .asm output file.
    printMov("po1",dest); //Move dest to power function
    printMov("po2",source); //Move source to power function
    g << "call comp_pow\n"; //Call power function
    printMov(dest,"po1"); //Move result to dest
}
void printOut(string output){ // Print a command that outputs the value of given variable in hexadecimal to our .asm output.
    g<<"mov w[outhex+2],"+output+"l\nmov w[outhex],"+output+"u\ncall print_hex\n";
}
string createNewSpace() //Increase space count by 1 and return the name of that space.
{
    string newstr = "_" + to_string(++createdSpaceCount); // Underscore for stack spaces.
    return newstr;
}
string createNewVar() //Increase variable count by 1 and return the name of that variable.
{
    string newstr = "$" + to_string(++createdVariableCount); // Dollar for variables.
    return newstr;
}
string giveSpace() //Return the name of an unused space and remove it from freeSpaces. If there is none generate one and return it instead.
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
void freeSpace(string name) //Free a space that won't be used anymore by adding it to freeSpaces.
{
	if(name[0]!='_') return;
	freeSpaces.push(name);
}

string executePostfix(ofstream &g,vector <string> &postfix) //Execute a tokenized postfix expression by printing instructions to .asm output.
{
    stack<string> myst;
    for(int i=0;i<postfix.size();i++) {
        if (postfix[i] == "+") {
        cout << "Plus" << endl;
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

            printMov(result,l);
            printMul(result,r);
            myst.push(result);
            freeSpace(l);
            freeSpace(r);
            continue;
        }
        if (postfix[i] == "^") {
            string l = myst.top();
            myst.pop();
            string r = myst.top();
            myst.pop();
            string result = giveSpace();

            printMov(result,r);
            printPow(result,l);
            myst.push(result);
            freeSpace(l);
            freeSpace(r);
            continue;
        }

        // CONSTANT NUMBER
        if (isNumber(postfix[i])) {
            cout << "NIMBER" << endl;
            if(postfix[i].length()>8) // Error if bigger than 32 bit. 0AAAAAAAA is wrong since bigger than 32 bit. IF THIS IS A PROBLEM I WILL OBJECT SINCE THIS WASNT CLARIFIED. - BURAK CETIN
                syntaxError();
            cout << postfix[i] << endl;
            string num8 = "";
            //cout << num8 << endl;
            for(int j=0;j<8-postfix[i].length();j++)
                num8.push_back('0');
            //cout << num8 << endl;
            num8=num8 + postfix[i];
            //cout << num8 << endl;
           // cout << "NIMBER" << endl;
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
    cout << "HEYO: " << myst.size() << " TOP: "<< myst.top() <<endl;
    if(myst.size()!=1)
        syntaxError();
    if(!(myst.top()[0]=='_' || myst.top()[0]=='$' || isNumber(myst.top())))
        syntaxError();
    return myst.top();
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
            cout << "TIME TO PRINT" << endl;
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