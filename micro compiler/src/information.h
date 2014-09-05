/*
 *         File: information.h
 *         Author: Yu Liu(liu-island@purdue.edu)
 */
#ifndef INFORMATION_H
#define INFORMATION_H

#include <string>

using namespace std;

/*============================================================================*/
// Information class is used to store the information of symbols in symbol tables
// base class is for INT or FLOAT type
class Information 
{
public:
	string name;
	string type;
	string IRname; //symbol name in the IR(intermediate representation) code
	Information(string nameIn, string typeIn): name(nameIn), type(typeIn) {}
};

class StringInfo: public Information //derived class for STRING type
{
public:
	string strValue;
	StringInfo(string nameIn, string valueIn): \
		Information(nameIn, "STRING"), strValue(valueIn) {}
};
#endif
