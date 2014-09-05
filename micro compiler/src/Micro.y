/*
 *         File: Micro.y
 *         Author: Yu Liu(liu-island@purdue.edu)
 *         y file for yacc to process 
 */

%{
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "../src/symtable.h"
#include "../src/ast.h"
#define YYDEBUG 1

/*===============extern variables and functions supported by lex and yacc===================*/

extern FILE *yyin;
extern FILE *yyout;
extern int yylex(); 
extern void yyerror(const char*s);

/*================declarations of functions called in semantic routines======================*/

void trim(string& str); //remove the beginning and ending white spaces of a string
void process_str_decl(string s1, string s2); //deal with string declaration
void process_val_list(string s1, string s2); //deal with variable declaration list
void process_rw_stmt(list<statement*> *stmts, string s, bool isReadStmt); //deal with read/write statement
void process_rw_list(list<statement*> *f_stmts, list<statement*> *stmts); //deal with read/write statement list
void process_fun_type(string& str); //deal with function return type
Information* GetInfo(string symbol_name); //get the information instance pointer by symbol name

/*===========================gloabl variables definitions================================*/

vector<Symtable*> symtab_stack; //use vector for stack to deal with nested scopes, while at the same time support iteration
vector<Symtable*> symtab_vec; //back up vector of symbol tables, symtab_stack becomes empty after parsing the source code
unsigned int block_no = 1; //keeps the current total number of symbol tables
unsigned int block_seq = 1; //keeps the sequence of the current symbol table
string refStr = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
string whitespaces(" \t\f\v\n\r");
pgm *root = NULL; //class pgm instance pointer, see class definition in "ast.h" 
%}

/*================union contains all the possible types in semantic routines=================*/
%union {
	float fval;
	int ival;
    const char *sval;
	exp_node *expnode;
	cond_node *conode;
	exp_node_list *exps;
	list<statement*> *stmts;
	statement *st;
	func_dec *funcdec;
	list<func_dec*> *func_decs;
	assignment_stmt *as_st;
	return_stmt *re_st;
	if_stmt *if_st;
	else_stmt *else_st;
	dowhile_stmt *dw_st;
	pgm *prog;
}

/*=================legal tokens in the source code===========================*/
%token PROGRAM BEGINLY END FUNCTION READ WRITE STRING  
%token IF ELSIF ENDIF DO WHILE CONTINUE BREAK RETURN TRUE FALSE 
%token '+' '-' '*' '/' '=' '<' '>' '(' ')' ';' ','
%token NE "!="
%token GE ">=" 
%token LE "<=" 
%token AS ":="
%token <sval> IDENTIFIER
%token <sval> STRINGLITERAL
%token <sval> FLOAT
%token <sval> INT
%token <sval> VOID
%token <ival> INTLITERAL
%token <fval> FLOATLITERAL

/*========================association rules===============================*/
%nonassoc AS
%left LE GE '<' '>'
%left '+' '-'
%left '*' '/'
%right unary_minus
%left '(' ')'

/*==================all the possible types in grammar========================*/
%type <sval> id
%type <sval> str
%type <sval> var_type
%type <sval> any_type
%type <sval> id_list
%type <expnode> expr 
%type <expnode> factor 
%type <expnode> postfix_expr
%type <conode> cond 
%type <stmts> stmt_list
%type <stmts> aug_stmt_list
%type <stmts> aug
%type <funcdec> func_decl
%type <func_decs> func_declarations
%type <stmts> func_body
%type <st> stmt
%type <st> aug_stmt
%type <as_st> assign_stmt
%type <re_st> return_stmt
%type <stmts> read_stmt
%type <stmts> write_stmt
%type <exps> expr_list
%type <exps> expr_list_tail
%type <if_st> if_stmt
%type <if_st> aug_if_stmt
%type <stmts> else_part
%type <stmts> aug_else_part
%type <dw_st> do_while_stmt
%type <prog> program
%type <func_decs> pgm_body

/*==================================grammar and semantic routines begin=====================================*/
// format is "grammar productions : { semantic routines }"
%%
program : PROGRAM id BEGINLY {Symtable* s = new Symtable; s->seq = block_seq++; string str($2); std::size_t found = str.find_first_of(whitespaces);	str.erase(found); s->scope = "GLOBAL"; symtab_stack.push_back(s); symtab_vec.push_back(s);} 
		  pgm_body END {$$ = new pgm($5, symtab_stack.back()); root = $$; symtab_stack.pop_back();} ; 
id : IDENTIFIER ; 
pgm_body : decl func_declarations {$$ = $2;}
		 | decl {$$ = new list<func_dec*>();}
		 | func_declarations {$$ = $1;}
		 | {$$ = new list<func_dec*>();}
		 ;
decl : string_decl decl 
	 | string_decl 
	 | var_decl decl
	 | var_decl
	 ;
string_decl : STRING id AS str ';' { process_str_decl(string($2), string($4)); } ;
str : STRINGLITERAL ;
var_decl : var_type id_list ';' { process_val_list(string($1), string($2)); } ; 
var_type : FLOAT 
         | INT
		 ;
any_type : var_type 
		 | VOID 
		 ; 
id_list : id id_tail ;
id_tail : ',' id id_tail 
		|
		;
param_decl_list : param_decl param_decl_tail ;
param_decl : var_type id { string s1($1); string s2($2); std::size_t found = s1.find_first_of(whitespaces); s1.erase(found); trim(s1); trim(s2); Information* info = new Information(s2, s1); symtab_stack.back()->insertInfo(info, "ARG");} ;
param_decl_tail : ',' param_decl param_decl_tail 
				|
				;
func_declarations : func_decl func_declarations { $2->push_front($1); $$ = $2; }	
				  | func_decl { $$ = new list<func_dec*>(); $$->push_back($1); }	
				  ;
func_decl : FUNCTION any_type id {Symtable* s = new Symtable; s->seq = block_seq++; s->scope = $3; string ss($2); process_fun_type(ss); s->type = ss; symtab_stack.push_back(s); symtab_vec.push_back(s);} 
			func_para BEGINLY func_body END { $$ = new func_dec($7, symtab_stack.back()); symtab_stack.pop_back();} ;
func_para : '(' param_decl_list ')' | '(' ')' ;
func_body : decl stmt_list {$$ = $2;}
		  | decl {$$ = new list<statement*>();}
		  | stmt_list {$$ = $1;}
		  | {$$ = new list<statement*>();}
		  ;
stmt_list : stmt stmt_list { $2->push_front($1); $$ = $2; }	
		  | read_stmt stmt_list { process_rw_list($1, $2); $$ = $1; }	
		  | write_stmt stmt_list { process_rw_list($1, $2); $$ = $1; }	
		  | stmt { $$ = new list<statement*>(); $$->push_back($1); }	
		  | read_stmt { $$ = $1; }	
		  | write_stmt { $$ = $1; }	
		  ;
stmt : assign_stmt { $$ = $1;}	
	 | return_stmt { $$ = $1;}	
	 | if_stmt { $$ = $1;}
	 | do_while_stmt { $$ = $1;}	
	 ;
assign_stmt : id AS expr ';' {string s1($1); std::size_t found = s1.find(":="); s1.erase(found); trim(s1); $$ = new assignment_stmt(s1, $3);} ; 
read_stmt : READ '(' id_list ')' ';' { $$ = new list<statement*>(); process_rw_stmt($$, $3, true);} ; 
write_stmt : WRITE '(' id_list ')' ';' { $$ = new list<statement*>(); process_rw_stmt($$, $3, false); } ;
return_stmt : RETURN expr ';' {$$ = new return_stmt($2);} ;
expr : expr '+' factor { $$ = new operator_node($1, $3, "+"); }
	 | expr '-' factor { $$ = new operator_node($1, $3, "-"); }
	 | factor {$$ = $1;}
	 ;
factor : factor '*' postfix_expr { $$ = new operator_node($1, $3, "*"); }
	   | factor '/' postfix_expr { $$ = new operator_node($1, $3, "/"); }
	   | postfix_expr {$$ = $1;}
	   ;
postfix_expr :  '(' expr ')' {$$ = $2;}
				| id {string s1($1); std::size_t found = s1.find_first_not_of(refStr); s1.erase(found); Information* info = GetInfo(s1); if(info) $$ = new id_node(info->name, info->type); else $$ = new id_node(s1, "NULL&WRONG");}
				| INTLITERAL  {$$ = new exp_node($1, "INT"); }
				| FLOATLITERAL {$$ = new exp_node($1, "FLOAT"); }
				| id '(' expr_list ')' {string str($1); std::size_t found = str.find("("); str.erase(found); trim(str); $$ = new fun_call_node(str, $3);}
				| id '(' ')' {string str($1); std::size_t found = str.find("("); str.erase(found); trim(str); $$ = new fun_call_node(str, NULL);}				
				;
expr_list : expr expr_list_tail {$2->exp_nodes->push_front($1); $$ = $2;} ;
expr_list_tail : ',' expr expr_list_tail {$3->exp_nodes->push_front($2); $$ = $3;}
			   | {$$ = new exp_node_list(new list<exp_node*>());}
			   ;
if_stmt : IF {Symtable* s = new Symtable; s->seq = block_seq++; char buffer[10]; sprintf(buffer, "BLOCK %d", block_no++); s->scope = buffer; symtab_stack.push_back(s); symtab_vec.push_back(s);}
		  '(' cond ')' func_body else_part ENDIF {$$ = new if_stmt($4, $6, $7); symtab_stack.pop_back();} ;
else_part : ELSIF {symtab_stack.pop_back(); Symtable* s = new Symtable; s->seq = block_seq++; char buffer[10]; sprintf(buffer, "BLOCK %d", block_no++); s->scope = buffer; symtab_stack.push_back(s); symtab_vec.push_back(s);}
			'(' cond ')' func_body else_part {else_stmt *elst = new else_stmt($4, $6); $7->push_front(elst); $$ = $7;} 
		  | {$$ = new list<statement*>();}
		  ;
cond : expr '<' expr { $$ = new cond_node($1, "<", $3); }
	 | expr '>' expr { $$ = new cond_node($1, ">", $3); }
	 | expr '=' expr { $$ = new cond_node($1, "=", $3); }
	 | expr NE expr { $$ = new cond_node($1, "!=", $3); }
	 | expr LE expr { $$ = new cond_node($1, "<=", $3); }
	 | expr GE expr { $$ = new cond_node($1, ">=", $3); }
	 | TRUE {exp_node* ll = new exp_node(1, "INT"); exp_node* rr = new exp_node(1, "INT"); $$ = new cond_node(ll, "TRUE", rr); }
	 | FALSE {exp_node* ll = new exp_node(1, "INT"); exp_node* rr = new exp_node(1, "INT"); $$ = new cond_node(ll, "FALSE", rr); }  
	 ;
do_while_stmt : DO {Symtable* s = new Symtable; s->seq = block_seq++; char buffer[10]; sprintf(buffer, "BLOCK %d", block_no++); s->scope = buffer; symtab_stack.push_back(s); symtab_vec.push_back(s);} 
				aug WHILE '(' cond ')' ';' {$$ = new dowhile_stmt($6, $3); symtab_stack.pop_back();} ;
aug_stmt_list : aug_stmt_list aug_stmt { $1->push_back($2); $$ = $1; }	
			  | aug_stmt_list read_stmt { process_rw_list($1, $2); $$ = $1; }
			  | aug_stmt_list write_stmt { process_rw_list($1, $2); $$ = $1; }
			  | {$$ = new list<statement*>();}
			  ;
aug_stmt : assign_stmt { $$ = $1;}	
	     | return_stmt { $$ = $1;}	 
         | aug_if_stmt  { $$ = $1;}	
		 | do_while_stmt  { $$ = $1;}	
		 | CONTINUE ';' { $$ = new con_stmt();}	
		 | BREAK ';' { $$ = new brk_stmt();}	
		 ;
aug : decl aug_stmt_list {$$ = $2;}
	| aug_stmt_list {$$ = $1;}
	;
aug_if_stmt : IF {Symtable* s = new Symtable; s->seq = block_seq++; char buffer[10]; sprintf(buffer, "BLOCK %d", block_no++); s->scope = buffer; symtab_stack.push_back(s); symtab_vec.push_back(s);} 
			  '(' cond ')' aug aug_else_part ENDIF {$$ = new if_stmt($4, $6, $7); symtab_stack.pop_back();} ;		
aug_else_part : ELSIF {symtab_stack.pop_back(); Symtable* s = new Symtable; s->seq = block_seq++; char buffer[10]; sprintf(buffer, "BLOCK %d", block_no++); s->scope = buffer; symtab_stack.push_back(s); symtab_vec.push_back(s);} 
				'(' cond ')' aug aug_else_part {else_stmt *elst = new else_stmt($4, $6); $7->push_front(elst); $$ = $7;} 
			  | {$$ = new list<statement*>();}
			  ;
%%
/*==================================grammar and semantic routines end=======================================*/

/*==========================definitions of functions called in semantic routines begin==========================*/

void trim(string& str)
{
	string::size_type pos = str.find_last_not_of(whitespaces);
	if(pos != string::npos) 
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(whitespaces);
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
}

void process_val_list(string s1, string s2)
{
	std::size_t found = s1.find_first_of(whitespaces); 
	s1.erase(found);
	
	found = s2.rfind(";");
	s2.replace(found, 1, ",");
	
	found = s2.find(",");
	while(found != string::npos)
	{
		string str = s2.substr(0, found);
		trim(str);
		Information* info = new Information(str, s1); 
		symtab_stack.back()->insertInfo(info, "LOCAL");
		s2 = s2.substr(found+1);
		found = s2.find(",");
	}
}

void process_str_decl(string s1, string s2)
{
	std::size_t found = s1.find(":="); 
	s1.erase(found);
	trim(s1);
	found = s2.find(";"); 
	s2.erase(found);
	trim(s2);
	StringInfo* info = new StringInfo(s1, s2); 
	symtab_stack.back()->insertInfo(info, "LOCAL");
}

Information* GetInfo(string symbol_name)
{
	vector<Symtable*>::reverse_iterator iter = symtab_stack.rbegin();
	for(; iter != symtab_stack.rend(); ++iter)
	{
		Symtable* sym = *iter;
		Information* info = sym->findInfo(symbol_name);
		if(info != NULL)
			return info;
	}
	return NULL;
}

void process_rw_stmt(list<statement*> *stmts, string s, bool isReadStmt)
{
	std::size_t found = s.rfind(")");
	s.replace(found, string::npos, ",");
	
	found = s.find(",");
	while(found != string::npos)
	{
		string str = s.substr(0, found);
		trim(str);
		
		Information* info = GetInfo(str);
		if(!info)
			cout << "Wrong code & impossible at this stage!" << endl;
		else
		{
			if(isReadStmt)
				stmts->push_back(new read_stmt(info->name, info->type));
			else	
				stmts->push_back(new write_stmt(info->name, info->type));
		}
		s = s.substr(found+1);
		found = s.find(",");
	}
}

void process_rw_list(list<statement*> *f_stmts, list<statement*> *stmts)
{
	list<statement*>::iterator iter = stmts->begin(); 
	list<statement*>::iterator iter_end = stmts->end(); 
	for(; iter != iter_end; ++iter) 
		f_stmts->push_back(*iter);
}

void process_fun_type(string& str)
{
	std::size_t found = str.find_first_of(whitespaces);
	if(found != string::npos)	
		str.erase(found);
}
/*==========================definitions of functions called in semantic routines end==========================*/

void yyerror(const char*s) 
{
	cout << "Grammar error in source code! Aborting..." << endl;
	exit(0);
}

int main(int argc, char** argv)
{	
	if(argc > 1)
    {   
		yyin = fopen(argv[1], "r");
		if(!yyin) 
		{
			cout << "Error occurred when reading source code!" << endl;
			exit(0);
		}
	}
    else
        yyin = stdin;
	yyparse();
	
	vector<Symtable*>::iterator iter = symtab_vec.begin();
	for(; iter != symtab_vec.end(); ++iter)
		(*iter)->genIRName();

	cout << ";IR code" << endl;
	root->genIR();
	root->printIR();
	
	cout << ";tiny code" << endl;
	root->printTiny();
	cout << "end" << endl;

	vector<Symtable*>::iterator it = symtab_vec.begin();
	for(; it != symtab_vec.end(); ++it)
		delete *it;
	
	delete root;
	return 0;
}
