/*
 *         File: ast.h
 *         Author: Yu Liu(liu-island@purdue.edu)
 *         Abstract syntax tree
 */
#ifndef AST_H
#define AST_H

#include <list>
#include <sstream>
#include "symtable.h"

extern vector<Symtable*> symtab_vec;  //defined in "Micro.y"

class IR_node
{
public:
	void output();
	string opcode, op1, op2, op3;	
	vector<string> gen_vec; //data flow analysis generation set
	vector<string> kill_vec; //data flow analysis kill set	
	vector<string> live_in_vec; //alive symbols before current IR node
	vector<string> live_out_vec; //alive symbols after current IR node	
};

class IRnodeInList
{
public:
	bool isleader(); //decide if this IR_node is the leading one in current code block
	IR_node node;
	vector<IRnodeInList*> preVec; //the preceding IR_nodes
	vector<IRnodeInList*> sucVec; //the successive IR_nodes   
};

class registers //register allocation management, 4 registers in total
{
public:
	registers(): r0(""), r1(""), r2(""), r3(""), \
		isdirty0(false), isdirty1(false), isdirty2(false), isdirty3(false) 
	{}
	
	void markDirty(string rg);
	void reset(IRnodeInList* p, Symtable* pfunc, vector<IRnodeInList*> *pp);
	void free(string rg/*"r0"*/, IRnodeInList* p, Symtable* pfunc);
	string allocate(string opr, IRnodeInList* p, Symtable* pfunc);
	string ensure(string opr, IRnodeInList* p, Symtable* pfunc);
	
	string r0, r1, r2, r3;
	bool isdirty0, isdirty1, isdirty2, isdirty3;
};

class exp_node //expression node
{
public:
	exp_node() {}
	exp_node(exp_node *n);
	exp_node(float value, string t);
	
	virtual string genIR();
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~exp_node();
	
	float num;
	string type;
	IR_node ir;
	vector<IRnodeInList*> *IRnodeList;
	IRnodeInList* pbase;
	Symtable* pglobal;
	Symtable* pfunc;
};

class id_node: public exp_node //identifier
{
public:	
	id_node(string id, string t);
	virtual string genIR();
	virtual void printIR();
	virtual void printTiny();
	
	string name;
};

class exp_node_list //for comma expression
{
public:
	list<exp_node*> *exp_nodes;
	exp_node_list(list<exp_node*> *exp_nodeList): exp_nodes(exp_nodeList) {}
	~exp_node_list();
};

class fun_call_node: public exp_node //function call expression
{
public:
	fun_call_node(string id, exp_node_list *L): name(id), explist(L) { getSymtable(); }
	void getSymtable();
	virtual string genIR(); 
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~fun_call_node();
	
	exp_node_list *explist;
	string name;
	Symtable* funsym;
	list<string> tmplist;
	IRnodeInList *p0, *p1, *finalpop; //p0 -> first "PUSH", p1 -> "JSR"
	list<IRnodeInList*> pushList;
	list<IRnodeInList*> popList;	
};

class cond_node: public exp_node //condition expression, used in if-else/while statement
{
public:
	cond_node(exp_node *L, string op, exp_node *R);
	virtual string genIR();
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~cond_node();
		
    exp_node *left, *right;
	IRnodeInList* pcond;
	string opcode, tmp; // tmp used only when comparing two identifiers 
};

class operator_node: public exp_node //arithmetic expression
{
public:
	operator_node(exp_node *L, exp_node *R, string op): left(L), right(R), opCode(op) 
	{ type = L->type; } //suppose left expression and right expression have the same type 
	virtual string genIR();
	virtual void printIR(); 
	virtual void printTiny(); 
    virtual ~operator_node();
	
	string opCode;
	exp_node *left, *right;
	IRnodeInList *pOperator;
};

class statement //general statement class, serving as the base class of other statement class
{
public:
	virtual void genIR() {}
	virtual void printIR() {}
	virtual void printTiny() {}
	virtual ~statement();
	
	string loopoutlabel; //if this statement is in a loop, loopoutlabel tells where to jump if it needs to get out.
	string label_before_cond; //label for continue to jump to
	Symtable* pglobal; //global symbol table
	Symtable* pfunc; //symbol table of function containing the current statement
	vector<IRnodeInList*> *IRnodeList;
};

class return_stmt: public statement 
{
public:
	return_stmt(exp_node *expression): ret_exp(expression) {}
	virtual void genIR();
	virtual void printIR(); 
	virtual void printTiny(); 
	virtual ~return_stmt();
	
	exp_node *ret_exp;
	IRnodeInList *pStore, *pRet;
	string tmp;
	IR_node ir;
};

class assignment_stmt: public statement 
{
public:
	assignment_stmt(string name, exp_node *expression): id(name), exp(expression) {}
	virtual void genIR();
	virtual void printIR(); 
	virtual void printTiny(); 
	virtual ~assignment_stmt();
	
	string id, tmp; //tmp used only when storing var to var
	exp_node *exp;
	IRnodeInList* pAssign;
	IR_node ir;
};

class read_stmt: public statement 
{
public:	
	read_stmt(string name, string tp): id(name), type(tp) {}
	virtual void genIR();	
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~read_stmt();
	
	string id, type;
	IR_node ir;
	IRnodeInList* pRead;
};

class write_stmt: public statement 
{
public:
	write_stmt(string name, string tp): id(name), type(tp) {}
	virtual void genIR();	
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~write_stmt();
	
	string id, type;
	IR_node ir;
	IRnodeInList* pWrite;
};

class brk_stmt: public statement //"BREAK"
{
public:
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~brk_stmt();
	IRnodeInList* p;
};

class con_stmt: public statement //"CONTINUE"
{
public:
	virtual void printIR(); 
	virtual void printTiny();
    virtual ~con_stmt();
	IRnodeInList* p;
};

class else_stmt: public statement 
{
public:
	else_stmt(cond_node *cond_exp, list<statement*> *else_stmts): cond(cond_exp), stmts(else_stmts) {}	
	virtual void genIR(); 
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~else_stmt();
		
	cond_node *cond;
	list<statement*> *stmts;
	IRnodeInList *p, *p1; //p -> "JUMP", p1 -> "LABEL"
	string outlabel, nextlabel;
};

class if_stmt: public statement 
{
public:
	if_stmt(cond_node *cond_exp, list<statement*> *ifstmts, list<statement*> *elsestmts): 
		cond(cond_exp), stmts(ifstmts), else_stmts(elsestmts) {}	
	virtual void genIR();
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~if_stmt();	
	
	cond_node *cond;
	IRnodeInList *p, *p1, *p2; // p -> "JUMP", p1/p2 -> "LABEL"
	string outlabel, nextlabel;
	list<statement*> *stmts, *else_stmts; 
};

class dowhile_stmt: public statement
{
public:	
	dowhile_stmt(cond_node *cond_exp, list<statement*> *dowhile_stmts): cond(cond_exp), stmts(dowhile_stmts) {}	
	virtual void genIR(); 
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~dowhile_stmt();
	
	cond_node *cond;
	list<statement*> *stmts;
	IRnodeInList *p, *p1, *p2, *p3; // p/p1/p3 -> "LABEL", p2 -> "JUMP"
	string looplabel, outlabel;
};

class func_dec // function class
{
public:
	func_dec(list<statement*> *stmtList, Symtable* p): stmts(stmtList), psym(p) 
	{
		IRnodeList = new vector<IRnodeInList*>();
	}
	virtual void genIR();	
	virtual void printIR(); 
	virtual void printTiny();
	virtual ~func_dec();

	int tmpCount;	
	vector<IRnodeInList*> *IRnodeList;
	Symtable *pglobal, *psym;
	list<statement*> *stmts;
	IRnodeInList *p1, *p2, *p3; // p1 -> "LABEL", p2 -> "LINK", p3 -> "RET"
};

class pgm // main program class
{
public:
	pgm(list<func_dec*> *func_decList, Symtable* p): func_decs(func_decList), psym(p) {}	
	virtual void genIR();
	virtual void printIR(); 
	virtual void printTiny(); 
	virtual ~pgm();
	
	Symtable* psym;
	list<func_dec*> *func_decs;
};
#endif
