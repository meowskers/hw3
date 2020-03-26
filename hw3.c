#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int m;
int n;
int x;
int max_squares = 0;
char *** dead_end_boards;
int dead_end_size = 10;
int dead_end_pos = 0;
int sequence[8][2]= {-2,-1,-1,-2,1,-2,2,-1,2,1,1,2,-1,2,-2,1};
void free_board(char ** board){
    for(int i=0; i< m; i++){
        free(board[i]);
    }
    free(board);
}
void add_dead_end(char ** board){
    if(dead_end_pos==dead_end_size){
        dead_end_size = dead_end_size*2;
        dead_end_boards = (char ***)realloc(dead_end_boards, sizeof(char **)*dead_end_size);
    }
    dead_end_boards[dead_end_pos] = board;
    dead_end_pos = dead_end_pos + 1;
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
char ** copy_board(char ** board){
    char ** board_copy = (char **)calloc(m*n, sizeof(char *)); 
    for(int i=0; i< m; i++){
        board_copy[i] = (char *)calloc(n, sizeof(char));
    }
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            board_copy[i][j] = board[i][j];
        }
    }
    return board_copy;
}
int complete_board(char ** board){
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            if(board[i][j]=='.'){
                return 0;
            }
        }
    }
    return 1;
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
        if((nc>m-1)||(nc<0)||(nr>n-1)||(nr<0)){
            *(vm + i) = -1;
        }else{
            if(board[nc][nr]=='.'){
                //printf("%d,%d made it through\n",nc,nr);
            }else{
                *(vm + i) = -1;
            }
        }
    }
    return vm;
}
void* sonnys_place(int c, int r, int move, char ** board){ 
    int total_moves = 0;
    int * valid_moves = find_moves(c,r,board);
    for(int i =0; i < 8; i++){
        if(*(valid_moves+i)!=-1){
            total_moves++;
        }
    }
    if(total_moves == 0){
        if(complete_board(board)){
            max_squares = m*n;
            printf("THREAD %ld: Sonny found a full knight's tour!\n",(long)pthread_self());
            free_board(board);
        }else{
            printf("THREAD %ld: Dead end after move #%d\n",(long)pthread_self(),move);
            if(max_squares<move){
                max_squares=move;
            }
            
            if(x==-1){
                add_dead_end(board);
            }else if(move>=x){
                printf("ADDING!!\n");
                add_dead_end(board);
            }
            
            //add_dead_end(board);
        }
    }else if(total_moves == 1){
        for(int i = 0; i < 8; i++){
            if(*(valid_moves+i)!=-1){
                board[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                sonnys_place(c+sequence[i][0], r+sequence[i][1], move+1, board);
            }
        }
        
    }else{
        int place_holder = total_moves;
        for(int i = 0; i < 8; i++){
            if(*(valid_moves+i)!=-1){
                if(place_holder==1){
                    board[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                    sonnys_place(c+sequence[i][0], r+sequence[i][1], move+1, board);
                }else{
                    printf("THREAD %ld: %d moves possible after move #%d; creating threads...\n",(long)pthread_self(),total_moves,move);
                    char ** board_copy = copy_board(board);
                    board_copy[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                    sonnys_place(c+sequence[i][0], r+sequence[i][1], move+1, board_copy);
                }
                place_holder--;
            }
        }
        
        
    }
    
    free(valid_moves);
    return NULL; 
} 
int main(int argc, char** argv){
    setvbuf(stdout,NULL,_IONBF,0);
    #ifdef NO_PARALLEL
    printf("YOOOOOOO\n");
    #endif
    
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
    //printf("arg1: %d, arg2: %d, arg3: %d\n",m,n,x); 
    // MAKING A BLANK BOARD
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    char ** blank_board = (char **)calloc(m*n, sizeof(char *)); 
    for(int i=0; i< m; i++){
        blank_board[i] = (char *)calloc(n, sizeof(char));
    }
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            blank_board[i][j] = '.';
        }
    }
    blank_board[0][0] = 'S';
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    dead_end_boards = (char ***)calloc(m*n*dead_end_size,sizeof(char **));

    sonnys_place(0, 0, 1, blank_board);
    printf("THREAD %ld: Best solution(s) found visit %d squares (out of %d)\n",(long)pthread_self(),max_squares,m*n);
    
    pthread_t thread; // declare thread 
    //pthread_create(&thread, NULL, calls, NULL); 
    //printf("In main \nthread id = %d\n", pthread_self());  
    //pthread_join(thread, NULL);  
    printf("****** %d ******\n",dead_end_pos);
    print_board(dead_end_boards[dead_end_pos-1]);
    printf("**********\n");
    for(int i = dead_end_pos-1; i > -1; i--){
        print_board(dead_end_boards[i]);
        free_board(dead_end_boards[i]);
    }
    free(dead_end_boards);
    return 0; 
    
}
