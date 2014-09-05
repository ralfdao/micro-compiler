/*
 *         File: symtable.cpp
 *         Author: Yu Liu(liu-island@purdue.edu)
 */
#include "symtable.h"

Symtable::~Symtable()
{
	unordered_map<string, Information*>::iterator it = hashTable.begin();
	for(; it != hashTable.end(); ++it)
		delete it->second;
}

void Symtable::insertInfo(Information* Info, string infoType)
{
	if(hashTable.find(Info->name) == hashTable.end())
		hashTable[Info->name] = Info;
	else
	{
		printf("%s redefinition!\n", Info->name.c_str());
		exit(0);
	}
	if(infoType == "ARG")
		syms_arg.push_back(Info->name);
	else if(infoType == "LOCAL")
		syms_local.push_back(Info->name);
}

Information* Symtable::findInfo(string name)
{
	if(hashTable.find(name) != hashTable.end())
		return hashTable[name];	
	return NULL;
}

string Symtable::getIRname(string name)
{
	Information *info = findInfo(name);
	if(info != NULL)
	{
		if(info->type != "STRING")
			return info->IRname;
	}
	return name;
}

string Symtable::IRtoTiny(string name)
{
	//rename IR local variable in tiny code according to the function stack
	//$Lm (Local Var) $-m 
	//...
	//$L1 (Local Var) $-1 
	std::size_t found = name.find("$L"); 
	if(found != string::npos)	
		return name.replace(0, 2, "$-");
	
	//rename IR function argument in tiny code according to the function stack
	//$Pn (Parameter) $(6+n-n) 
	//...
	//$P1 (Parameter) $(6+n-1) 
	//$R (Return Value) $(6+n)
	found = name.find("$P"); 
	if(found != string::npos)
	{
		name.replace(0, 2, "");
		int c = atoi(name.c_str());
		c = 6 + syms_arg.size() - c;
		
		char buf[10];
		sprintf(buf, "$%d", c);
		return string(buf);	
	}
	
	//rename IR local temp variable in tiny code according to the function stack
	//$Tk (Local Temp) $-(m+k) 
	//...
	//$T1 (Local Temp) $-(m+1) 
	found = name.find("$T"); 
	if(found != string::npos)
	{
		name.replace(0, 2, "");
		int c = atoi(name.c_str());
		c = syms_local.size() + c;
		
		char buf[10];
		sprintf(buf, "$-%d", c);
		return string(buf);	
	}
	
	return name;
}

void Symtable::genIRName()
{
	//for local variables in GLOBAL scope, "IRname" is the same as "name"
	if(scope == "GLOBAL") 
	{
		for(int i = 0; i < syms_local.size(); ++i)  
		{	
			Information* info = findInfo(syms_local[i]);
			if(info)
				info->IRname = info->name;
		}
	}
	else //in other scopes, function arguments' IRname is of "$P#" form, local variables "$L#"
	{
		for(int i = 0; i < syms_arg.size(); ++i) 
		{	
			Information* info = findInfo(syms_arg[i]);
			if(info)
			{
				char buf[10];
				sprintf(buf, "$P%d", i+1);
				info->IRname = string(buf);
			}
		}
		
		for(int i = 0; i < syms_local.size(); ++i) 
		{	
			Information* info = findInfo(syms_local[i]);
			if(info)
			{
				char buf[10];
				sprintf(buf, "$L%d", i+1);
				info->IRname = string(buf);
			}
		}	
	}
}

void Symtable::printSymbolTiny()
{
	for(int i = 0; i < syms_arg.size(); ++i)
	{	
		Information* info = findInfo(syms_arg[i]);
		if(info != NULL)
		{
			if(info->type == "STRING") //if the type is STRING, also output its content
			{
				StringInfo* sp = static_cast<StringInfo*>(info);
				cout << "str " << sp->name << " " << sp->strValue << endl;
			}
			else
				cout << "var " << info->name << endl;
		}
	}
	
	for(int i = 0; i < syms_local.size(); ++i)  
	{	
		Information* info = findInfo(syms_local[i]);
		if(info)
		{
			if(info->type == "STRING") //if the type is "STRING", also output its content
			{
				StringInfo* sp = static_cast<StringInfo*>(info);
				cout << "str " << sp->name << " " << sp->strValue << endl;
			}
			else
				cout << "var " << info->name << endl;
		}
	}
}
