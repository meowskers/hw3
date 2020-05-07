
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h>
#include <string.h>

int main() {
char * z = calloc( 32, sizeof( char ) ); int pid = fork();
if (pid==0)
{
char *x = malloc(32 * sizeof(*x)); } else {
waitpid(pid, NULL, 0);
int **x = malloc(32 * sizeof(*x)); free(z);
}
return EXIT_SUCCESS; }