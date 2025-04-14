%{

#include <stdio.h>

int yylex(void);
void yyerror(const char *s);

%}

%token HELLO
%token WORLD


%union {
	int 	intValue;
	float floatValue;
	char 	*stringValue;
}


%%

program:
	HELLO WORLD { printf("HELLO WORLD\n"); } 
	;

%%

void yyerror(const char *s) {
	fprintf(stderr, "Sintax error: %s\n", s);
}

int main() {
	yyparse();
	return 0;
}
