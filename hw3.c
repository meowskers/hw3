#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int m;
int n;
int x;
int max_squares = 0;
char *** dead_ends;
int sequence[8][2]= {-2,-1,-1,-2,1,-2,2,-1,2,1,1,2,-1,2,-2,1};
void free_board(char ** board){
    for(int i=0; i< m; i++){
        free(board[i]);
    }
    free(board);
}
void print_board(char ** board){
    for(int i=0; i < m; i++){
        if(i == 0){
            printf("THREAD %ld: >  ",(long) pthread_self());
        }else{
            printf("THREAD %ld:    ",(long) pthread_self());
        }
        for(int j=0; j < n; j++){
            printf("%c",board[i][j]);
        }
        printf("\n");
    }
}
int *find_moves(int c, int r, char ** board){
    int valid_moves[8] = {0,1,2,3,4,5,6,7};
    int * vm = (int *)calloc(8,sizeof(int));
    for(int i = 0; i < 8; i++){
        *(vm +i) = i;
    }
    for(int i = 0; i < 8; i++){
        int nc = c+sequence[i][0];
        int nr = r+sequence[i][1];
        //printf("CHECKING %d,%d\n",nc,nr);
        if((nc>m-1)||(nc<0)||(nr>n-1)||(nr<0)){
            *(vm + i) = -1;
        }else{
            if(board[nc][nr]=='.'){
                //*(vm+i) = i;
                printf("%d,%d made it through\n",nc,nr);
            }else{
                *(vm + i) = -1;
            }
        }
    }
    return vm;
}
void* sonnys_place(int c, int r, int move, char ** board){ 
    
    int * valid_moves = find_moves(c,r,board);
    for(int i =0; i < 8; i++){
        printf("%d ",*(valid_moves+i));
    }
    free(valid_moves);
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
    blank_board[0][0] = 'S';
    print_board(blank_board);
    sonnys_place(0, 0, 1, blank_board);
    free_board(blank_board);
    
    
    pthread_t thread; // declare thread 
    //pthread_create(&thread, NULL, calls, NULL); 
    //printf("In main \nthread id = %d\n", pthread_self());  
    //pthread_join(thread, NULL);  
    return 0; 
    
}
