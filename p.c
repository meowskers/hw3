1 void * q3( void * y ) 2{
3 int*x=(void*)y;
4   char * s = calloc( *x + 16, sizeof( char ) );
5   sprintf( s, "ABCDEFGH" );
6   fprintf( stderr, "%s", s );
7   return NULL;
8}
9 int main()
10 {
11   int * z = calloc( 1, sizeof( int ) );
12   if (fork() == 0) *z = 10;
13 pthread_t t1;
14 pthread_create( &t1, NULL, q3, z );
15   fprintf( stderr, "ERROR" );
16 pthread_join( t1, NULL );
17 return EXIT_SUCCESS;
