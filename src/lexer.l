%{

#include <stdio.h>
#include "parser.h"

%}

%%

"hello"		{ return HELLO; }
"world"		{ return WORLD; }

[ \t\n] { }
. 			{ }

%%
