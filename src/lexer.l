%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // Incluindo os tokens gerados pelo Bison

int indent_level = 0;
int current_line_indent = 0;

%}

%x STARTLINE

%%


\n 										{ current_line_indent = 0; BEGIN(STARTLINE);  }
<STARTLINE>[ \t]+ 		{  current_line_indent++; }
<STARTLINE>\n 				{  current_line_indent = 0; }
<STARTLINE>. {
	 unput(*yytext);
	 if (current_line_indent > indent_level) {
		 indent_level++;
		 return INDENT;
	 } else if (current_line_indent < indent_level) {
		 indent_level--;
		 return DEDENT;
	 } else {
		 BEGIN (INITIAL);
	 }
}
<<EOF>> {
	if (indent_level > 0) {
		indent_level--;
		return DEDENT;
	}
	return 0;
}

"def"                { return DEF; 		}
"return"             { return RETURN; }
"if"                 { return IF; 		}
"elif"               { return ELIF; 	}
"else"               { return ELSE; 	}
"while"              { return WHILE; 	}
"for"                { return FOR; 		}
"in"                 { return IN; 		}
"range"              { return RANGE; 	}
"and"                { return AND; }
"or"                 { return OR; }
"not"                { return NOT; }
"True"               { yylval.string = strdup(yytext); return TRUE; }
"False"              { yylval.string = strdup(yytext); return FALSE; }

"int" 							 { yylval.string = strdup("int"); return TYPE_INT; 	}
"float" 						 { yylval.string = strdup("float"); return TYPE_FLOAT; }
"bool" 							 { yylval.string = strdup("bool"); return TYPE_BOOL; 	}

[0-9]+               { yylval.intValue = atoi(yytext); return NUM; }
[0-9]+\.[0-9]+       { yylval.string = strdup(yytext); return FLOAT_NUM; }

[A-Za-z_][A-Za-z0-9_]* {
    yylval.string = strdup(yytext);
    return ID;
}

"=="                  { return EQ; }
"!="                  { return NE; }
"<"                   { return LT; }
">"                   { return GT; }
"<="                  { return LE; }
">="                  { return GE; }
"="                   { return ASSIGN; }

"->"									{ return ARROW; }

"+"                   { return PLUS; }
"-"                   { return MINUS; }
"*"                   { return TIMES; }
"/"                   { return DIVIDE; }
"%"                   { return MOD; }

"("                   { return LPAREN; }
")"                   { return RPAREN; }
"{"                   { return LBRACE; }
"}"                   { return RBRACE; }
","                   { return COMMA; }
":"                   { return COLON; }
";"                   { return SEMICOLON; }

[ ]+            { /* Ignorar espaços em branco, tabs e quebras de linha */ }

%%
