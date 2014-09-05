/*
 *         File: Micro.l
 *         Author: Yu Liu(liu-island@purdue.edu)
 *         l file for lex to process 
 */
 
%{
#include "../src/symtable.h" 
#include "../src/ast.h"
#include "Micro.tab.h" //this file will be generated when processing
%}

%%
--.*     {} 																  /* Comments */
\"[^\"]{0,80}\"	{yylval.sval = yytext; return STRINGLITERAL; }	              /* STRINGLITERAL */
PROGRAM  { return PROGRAM; }                                                  /* Keyword */
FUNCTION { return FUNCTION; }                                                 /* Keyword */
READ     { return READ; }                                                     /* Keyword */
WRITE    { return WRITE; }                                                    /* Keyword */
IF 		 { return IF; }                                                       /* Keyword */
ELSIF    { return ELSIF; }                                                    /* Keyword */
ENDIF    { return ENDIF; }                                                    /* Keyword */
DO       { return DO; }                                                       /* Keyword */
WHILE    { return WHILE; }                                                    /* Keyword */
CONTINUE { return CONTINUE; }                                                 /* Keyword */
BREAK    { return BREAK; }                                                    /* Keyword */
RETURN   { return RETURN; }                                                   /* Keyword */
INT      { yylval.sval = yytext; return INT; }                                /* Keyword */
VOID     { yylval.sval = yytext; return VOID; }                               /* Keyword */
STRING   { yylval.sval = yytext; return STRING; }                             /* Keyword */
FLOAT    { yylval.sval = yytext; return FLOAT; }                              /* Keyword */
TRUE     { return TRUE; }                                                     /* Keyword */
FALSE    { return FALSE; }                                                    /* Keyword */
BEGIN    { return BEGINLY; }                                                  /* Keyword */
END      { return END; }                                                      /* Keyword */
[A-Za-z]([A-Za-z]|[0-9]){0,30}	{yylval.sval = yytext; return IDENTIFIER; }   /* IDENTIFIER */
-?0|[1-9][0-9]*	         { yylval.ival = atoi(yytext); return INTLITERAL; }	  /* INTLITERAL */
(-?[0-9]*)(\.[0-9]+)   { yylval.fval = atof(yytext); return FLOATLITERAL; }   /* FLOATLITERAL */
"+"|"-"|"*"|"/"|"="|"<"|">"|"("|")"|";"|","             { return *yytext; }   /* Operators */
":="     { yylval.sval = yytext; return AS; }                                 /* Operator */
"!="     { return NE; }                                                       /* Operator */
"<="     { return LE; }                                                       /* Operator */	
">="     { return GE; }                                                       /* Operator */		 
[ \t]+   {}                                                  		          /* Empty Space */  
\n       {}                                                  		          /* New Line */  
%%

int yywrap() 
{
	return 1;
}
