#include <stdio.h>
#include <stdbool.h>

int fatorial(int n) {
int resultado = 1;
while ((n > 1)) {
resultado = (resultado * n);
n = (n - 1);
}
return resultado;
}

