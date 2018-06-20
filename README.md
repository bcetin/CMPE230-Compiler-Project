# CMPE 230 Compiler Project

This was an assignment given to us as a part of our CMPE 230 course in Boğaziçi University Computer Engineering Undergraduate Program on Spring 2018 term. [Specifications of the assignment is included in the repository.](/cmpe230spring2018hw1.pdf)
Our goal was to create a simple compiler for a simple programming language called COMP. Our compiler creates an A86 assembly code for the given COMP language code.

## Implementation

Implementation is explained in the code with comments in detail. In short our program follows these steps:
* Opens up the file in the path given to the program as a parameter. Also creates an .asm file with the same name which will have the output when compilation is over.
* Prints out the initial assembly code, like multiplication and power functions that will be used.
* For each line categorizes the lines as assignment operations or printing operations.
* For the assignment statements, prints out a code that will evaluate the RHS and put it into the memory location of LHS.
* For the printing operations, prints out a code that similarly evaluates the expression then calls a function that was initialized in the beginning.
* Finally it prints out a code that will allocate memory for the variables it used while evaluating the statements.

The way our program translates each line to assembly is as follows:
* It first tokenizes each line.
* Then converts this tokenized version to postfix form using a stack.
* This postfix form is ready for translation to assembly, using the specific function for each operation the postfix form is printed out as assembly code.

It can also point out the first line that it encountered a syntax error in the .asm file (if there is one).
### How to run

Our project is done in CLion using C++ language. You need cmake and make utility to run our code.
After making sure you have both installed, the following code will build our program.
```
cmake
make
```

After building our program can be run on terminal with the .co file as parameter.
```
comp input.co
```
Afterwards you can run the A86 assembly code using the compiler provided [here](http://www.eji.com/a86/). (Using DOSBox might be needed in newer systems.)

## Programs used

* [CLion](https://www.jetbrains.com/clion/)
* [A86 Assembler](http://www.eji.com/a86/)

## Authors

* **Burak Çetin**
* **Merve İlik**

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.

## Acknowledgments

* Our instructor in the course, Can Öztüran
* Our classmates
* [This Infix to Postfix tutorial.](http://csis.pace.edu/~wolf/CS122/infix-postfix.htm)