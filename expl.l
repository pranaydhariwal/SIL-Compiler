%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "ast.h"
	#include "typeTable.h"

	#define YYSTYPE tnode*
	#include "y.tab.h"

	int number;
	int lineNo = 1;
	int yyerror();
%}

letter [a-zA-Z]
digit [0-9]+
openComment "/*"
closeComment "*/"

%%

"+" 		{return PLUS;}
"-" 		{return SUB;}
"*" 		{return MUL;}
"/" 		{return DIV;}
[%]			{return MOD;}
"=" 		{return ASSG;}
"<"			{return LT;}
">"			{return GT;}
"<="		{return LE;}
">="		{return GE;}
"=="		{return EQ;}
"!="		{return NE;}
"type"		{return TYPE;}
"endtype"	{return ENDTYPE;}
"read"  	{return READ;}
"write" 	{return WRITE;}
"begin" 	{return BEGIN1;}
"end" 		{return END;}
"decl"		{return DECL;}
"enddecl" 	{return ENDDECL;}
"integer"	{return integer;}
"boolean"	{return boolean;}
"if"		{return IF;}
"then" 		{return THEN;}
"else" 		{return ELSE;}
"endif"		{return ENDIF;}
"while"		{return WHILE;}
"do"		{return DO;}
"endwhile"	{return ENDWHILE;}
"AND"		{return AND;}
"OR"		{return OR;}
"NOT"		{return NOT;}
"TRUE"		{yylval = makeLeaf(1, tSearch("boolean")->index);return NUM;}
"FALSE"		{yylval = makeLeaf(0, tSearch("boolean")->index);return NUM;}
"return"	{return RETURN;}
"alloc()"	{return ALLOC;}
"free"		{return FREEM;}
"NULL"		{return nullPointer;}
[ \t] 		{}
[;]   		{return *yytext;}
[,]			{return *yytext;}
[.]			{return *yytext;}
[()] 		{return *yytext;}
[{}]		{return *yytext;}
"["			{return *yytext;}
"]"			{return *yytext;}
"&"			{return *yytext;}
{letter}({letter}|{digit})*   {yylval = makeID(yytext);return ID;}
{digit}  	{number = atoi(yytext); printf("Number: %d\n", number);yylval = makeLeaf(number, tSearch("integer")->index); return NUM;}

[\n] 		{lineNo++;}
["//"][^"\n"]*	{lineNo++;}
. 			{yyerror("Unknown character.\n");}

%%

int yywrap(){
	return 1;
}
