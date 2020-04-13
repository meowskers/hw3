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
pthread_t main_id;
int sequence[8][2]= {{-2,-1},{-1,-2},{1,-2},{2,-1},{2,1},{1,2},{-1,2},{-2,1}};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct thread_params{
    int c;
    int r; 
    int move; 
    char ** board;
};
void free_board(char ** board){
    for(int i=0; i< m; i++){
        free(board[i]);
    }
    free(board);
}
void free_t_board(char ** board){
    for(int i = 0; i < n; i++){
        free(board[i]);
    }
    free(board);
}
char ** transpose(char ** board){
    char ** new_board = (char **)calloc(m*n, sizeof(char *));
    for(int i = 0; i <  n; i++){
        new_board[i] = (char *)calloc(m,sizeof(char));
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            new_board[i][j] = board[j][i];
        }
    }
    return new_board;
}
void add_dead_end(char ** board){
    pthread_mutex_lock(&lock);
    if(dead_end_pos==dead_end_size){
        dead_end_size = dead_end_size*2;
        dead_end_boards = (char ***)realloc(dead_end_boards, sizeof(char **)*dead_end_size);
    }
    dead_end_boards[dead_end_pos] = board;
    dead_end_pos = dead_end_pos + 1;
    pthread_mutex_unlock(&lock);
}
void print_board(char ** board){
    for(int i=0; i < n; i++){
        if(i == 0){
            printf("THREAD %ld: > ",(long) pthread_self());
        }else{
            printf("THREAD %ld:   ",(long) pthread_self());
        }
        for(int j=0; j < m; j++){
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
void* sonnys_place(void * input){ 
    struct thread_params *in_args = input;
    int c = in_args->c;
    int r = in_args->r;
    int move = in_args->move;
    char ** board = in_args->board;
    
    //printf("THREAD %ld: Entering sonny's place, move %d\n",(long)pthread_self(),move);
    //print_board(board);
    int total_moves = 0;
    // HAVE TO BE CAREFUL TO FREE VALID_MOVES
    int * valid_moves = find_moves(c,r,board);
    for(int i =0; i < 8; i++){
        if(*(valid_moves+i)!=-1){
            total_moves++;
        }
    }
    //printf("%ld*****%d*******\n",(long)pthread_self(),in_args->move);

    if(total_moves == 0){
        //printf("*****%d*******\n",in_args->move);

        //printf("NO MOVES\n");
        if(complete_board(board)){
            max_squares = m*n;
            printf("THREAD %ld: Sonny found a full knight's tour!\n",(long)pthread_self());
            free_board(board);
			//pthread_exit(NULL);
			//free(valid_moves);
			return NULL;
        }else{
            printf("THREAD %ld: Dead end after move #%d\n",(long)pthread_self(),move);
            if(max_squares<move){
                max_squares=move;
            }
            if(x==-1){
                add_dead_end(board);
            } else if(move>=x) {
                add_dead_end(board);
            } else{
                free_board(board);
            }
            //free(valid_moves);
            //pthread_exit(NULL);
            return NULL;
        }
    }else if(total_moves == 1){
        //printf("ONE MOVE\n");
        for(int i = 0; i < 8; i++){
            if(*(valid_moves+i)!=-1){
                board[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                struct thread_params *argss;
                argss = (struct thread_params*)calloc(1,sizeof(struct thread_params*));
                argss->c = c+sequence[i][0];
                argss->r = r+sequence[i][1];
                argss->move = move+1;
                argss->board = board;
                sonnys_place(argss);
                in_args->move = argss->move;
                free(argss);
            }
        }
        //free(valid_moves);
        return NULL;
        //pthread_exit(NULL);
    }else{
        //printf("MORE THEN ONE MOVE\n");
        pthread_t *tid = (pthread_t*)calloc(8,sizeof(pthread_t));
        int placeholder = total_moves;
        struct thread_params * argss = (struct thread_params * )calloc(8,sizeof(struct thread_params));
        
        //struct thread_params argss[8];
        //(tid+1) = 2;
        printf("THREAD %ld: %d moves possible after move #%d; creating threads...\n",(long)pthread_self(),total_moves,move);
        int max_move = 0;
        for(int i = 0; i < 8; i++){
            if(*(valid_moves+i)!=-1){
                //struct thread_params argss;
                //argss = (struct thread_params)calloc(1,sizeof(struct thread_params));
                argss[i].c = c+sequence[i][0];
                argss[i].r = r+sequence[i][1];
                argss[i].move = move+1;
                if(placeholder ==1){
                    board[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                    argss[i].board = board;
                }else{
                    char ** board_copy = copy_board(board);
                    board_copy[c+sequence[i][0]][r+sequence[i][1]] = 'S';
                    argss[i].board=board_copy;
                }
                placeholder = placeholder -1;
                pthread_create(tid+i,NULL,sonnys_place,&argss[i]);
                pthread_join(*(tid+i), NULL);
                in_args->move = argss[i].move;
                if(max_move<argss[i].move){
                    max_move = argss[i].move;
                }
             
                printf("THREAD %ld: Thread [%ld] joined (returned %d)\n",(long)pthread_self(), (long)*(tid+i),argss[i].move);
                //printf("$$%d$$%d\n",i,max_move);
            }
        }
        
        in_args->move = max_move;
        free(tid);
        /*
        for(int i = 0; i < 8; i++){
            if((*valid_moves+i)!=-1){
                printf("THREAD %ld: Thread [%ld] joined (returned %d)\n",(long)pthread_self(), (long)*(tid+i),argss[i].move);
            }
        }
        */
        /*
        for(int i = 0; i < 8; i++){
            if(*(valid_moves+i)!=-1){
                pthread_join(*(tid+i),NULL);
                printf("THREAD %ld: Thread [%ld] joined (returned %d)\n",(long)pthread_self(), (long)*(tid+i),argss[i].move);
            }
        
        }
        */    
        }
    //free(valid_moves);
    return NULL;
} 
int main(int argc, char** argv){
    setvbuf(stdout,NULL,_IONBF,0);
    main_id = pthread_self();
    #ifdef NO_PARALLEL
    #endif
    
    if((argc != 3) && (argc != 4)){
        fprintf(stderr,"ERROR: Invalid argument(s)\n");
        fprintf(stderr,"USAGE: a.out <m> <n> [<x>]\n");
        return EXIT_FAILURE;
    }
    char *mt = *(argv+1);
    char *nt = *(argv+2);
    char *xt;
    m = atoi(nt);
    n = atoi(mt);
    x = -1;
    // TAKING IN INPUTS
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if((m==0) || (n==0) || (m<3) || (n<3)){
    // Checking if m or n is invalid
        fprintf(stderr,"ERROR: Invalid argument(s)\n");
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
            if((x<=0)||(x>m*n)){
                fprintf(stderr,"ERROR: Invalid argument(s)\n");
                fprintf(stderr,"USAGE: a.out <m> <n> [<x>]\n");
                return EXIT_FAILURE;
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    printf("THREAD %ld: Solving Sonny's knight's tour problem for a %dx%d board\n",(long) pthread_self(),n,m);
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
    struct thread_params *argss;
    argss = (struct thread_params*)calloc(1,sizeof(struct thread_params*));
    argss->c = 0;
    argss->r = 0;
    argss->move = 1;
    argss->board = blank_board;
    sonnys_place(argss);
    free(argss);
    printf("THREAD %ld: Best solution(s) found visit %d squares (out of %d)\n",(long)pthread_self(),max_squares,m*n);
    printf("THREAD %ld: Dead end boards:\n",(long)pthread_self());
    for(int i = 0; i < dead_end_pos; i++){
        char ** temp = transpose(dead_end_boards[i]);
        print_board(temp);
        free_t_board(temp);
        free_board(dead_end_boards[i]);
    }
    free(dead_end_boards);
    pthread_exit(NULL);
    return 0; 
    
}
