/*
 *         File: symtable.h
 *         Author: Yu Liu(liu-island@purdue.edu)
 *         symbol table 
 */
#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "information.h"

/*============================================================================*/
// Symtable class is used to store and output the information of symbol tables
// (including nested symbol tables) for each scope in the source code
class Symtable
{
public:
	Information* findInfo(string name); //get symbol Information by its name
	string getIRname(string name); //get IR code symbol name by its name
	string IRtoTiny(string name); //convert IR code symbol name to Tiny code symbol name
	void insertInfo(Information* Info, string infoType); //insert symbol Information into hashTable
	void printSymbolTiny(); //print global symbol declarations in tiny code
	void genIRName(); //generate the IRnames for all the function arguments and local variables
	~Symtable(); //hashTable clean up
	
	int seq; //indicate the sequence of current symbol table
	string scope; //store the name of symbol table, in the form of "BLOCK #" or function name
	string type; //store the type of the return value if the symbol table is of function type
	unordered_map<string, Information*> hashTable; //map variable name to variable Information
	vector<string> syms_arg; //vector of function arguments if the symbol table is of function type
	vector<string> syms_local; //vector of local variables in the symbol table
};
#endif
