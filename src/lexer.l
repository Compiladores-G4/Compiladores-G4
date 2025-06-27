%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // Incluindo os tokens gerados pelo Bison

#define MAX_INDENT_STACK 50

int indent_stack[MAX_INDENT_STACK];
int indent_stack_ptr = 0;
int current_line_indent = 0;
int pending_dedents = 0;

%}

%x STARTLINE

%%

\n {
    current_line_indent = 0;
    BEGIN(STARTLINE);
}

<STARTLINE>[ ] {
    current_line_indent++;
}

<STARTLINE>\t {
    current_line_indent += 4; // Tab vale 4 espaços
}

<STARTLINE>\n {
    current_line_indent = 0;
}

<STARTLINE>. {
    unput(*yytext);
    
    // Se há DEDENTs pendentes, retorne um por vez
    if (pending_dedents > 0) {
        pending_dedents--;
        return DEDENT;
    }
    
    // Se indentação aumentou
    if (current_line_indent > indent_stack[indent_stack_ptr]) {
        indent_stack[++indent_stack_ptr] = current_line_indent;
        BEGIN(INITIAL);
        return INDENT;
    }
    // Se indentação diminuiu
    else if (current_line_indent < indent_stack[indent_stack_ptr]) {
        // Encontrar o nível correspondente na pilha
        while (indent_stack_ptr > 0 && indent_stack[indent_stack_ptr] > current_line_indent) {
            indent_stack_ptr--;
            pending_dedents++;
        }
        
        // Verificar se a indentação corresponde a um nível válido
        if (indent_stack[indent_stack_ptr] != current_line_indent) {
            printf("Erro: indentação inconsistente\n");
            return -1;
        }
        
        if (pending_dedents > 0) {
            pending_dedents--;
            BEGIN(INITIAL);
            return DEDENT;
        }
    }
    
    BEGIN(INITIAL);
}

<<EOF>> {
    if (pending_dedents > 0) {
        pending_dedents--;
        return DEDENT;
    }
    if (indent_stack_ptr > 0) {
        indent_stack_ptr--;
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
"["                   { return LBRACKET; }
"]"                   { return RBRACKET; }
","                   { return COMMA; }
":"                   { return COLON; }
";"                   { return SEMICOLON; }

[ ]+            { /* Ignorar espaços em branco, tabs e quebras de linha */ }

%%

int yywrap() {
    return 1;
}
