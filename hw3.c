#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int m;
int n;
int x;
int max_squares = 0;
char *** dead_ends;

void* sonnys_place(int c, int r, int move, char ** board) 
{ 
    c++;
    // using pthread_self() get current thread id 
    //printf("In function \nthread id = %d\n", pthread_self()); 
    //pthread_exit(NULL); 
    return NULL; 
} 
int main(int argc, char** argv){
    setvbuf(stdout,NULL,_IONBF,0);
    if((argc != 3) && (argc != 4)){
        fprintf(stderr,"ERROR: Invalid arguments(s)\n");
        fprintf(stderr,"USAGE: a.out <m> <n> [<x>]\n");
        return EXIT_FAILURE;
    }
    char *mt = *(argv+1);
    char *nt = *(argv+2);
    char *xt;
    m = atoi(mt);
    n = atoi(nt);
    x = -1;
    // TAKING IN INPUTS
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if((m==0) || (n==0) || (m<3) || (n<3) ){
    // Checking if m or n is invalid
        fprintf(stderr,"ERROR: Invalid arguments(s)\n");
        fprintf(stderr,"USAGE: a.out <m> <n> [<x>]\n");
        return EXIT_FAILURE;
    }else if(argc == 4){
        // Checking if optional argument is valid
        xt = *(argv+3);
        if( strcmp(xt,"0")==0){
            // Special case where x is 0 - doesnt work work with atoi 
            x = 0;
        }else{
            x = atoi(xt);
            if((x<=0)){
                fprintf(stderr,"ERROR: Invalid arguemtns(s)\n");
                fprintf(stderr,"USAGE: a.out <m> <n> [<x>]\n");
                return EXIT_FAILURE;
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    printf("THREAD %ld: Solving Sonny's knight's tour problem for a %dx%d board\n",(long) pthread_self(),m,n);
    printf("arg1: %d, arg2: %d, arg3: %d\n",m,n,x); 
    // MAKING A BLANK BOARD
    char ** blank_board = (char **)calloc(m*n,sizeof(char *)); 
    for(int i=0; i< m; i++){
        blank_board[i] = (char *)malloc(n * sizeof(char));
    }
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            blank_board[i][j] = '.';
        }
    }
    sonnys_place(0, 0, 1, blank_board);
    free(blank_board);
    
    
    pthread_t thread; // declare thread 
    //pthread_create(&thread, NULL, calls, NULL); 
    //printf("In main \nthread id = %d\n", pthread_self());  
    //pthread_join(thread, NULL);  
    return 0; 
    
}
