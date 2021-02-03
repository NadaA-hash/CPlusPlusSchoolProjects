/******************************************************************************
 CS288 HW7 three search strategies: depth, breadth, and intelligent
 command example: command 16 numbers and search strategy

 fifteen 1 2 3 4 5 6 7 8 9 10 11 12 13 14 0 15 {dfs|bfs|astar}
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BF 4			/* Branching factor of the search tree */
#define N 4
#define NxN 16
#define DFS 1                   /* depth first search */
#define BFS 2                   /* breadth first search */
#define BEST 3                  /* best first search */
#define BB 4                    /* branch and bound */
#define ASTAR 5                 /* A* search */
#define UNKNOWN 9		/* unknown search strategy */

#define MAX_PATH 1000

#define DN 0			/* for finding path */
#define RT 1
#define UP 2
#define LT 3
#define UK 9

#define FVAL 0			/* f=g+h, keep it simple for now */
#define GVAL 1
#define HVAL 2
#define PATH 3			/* array index, how it got to this state */

#define TRUE 1
#define FALSE 0

int level,strategy;

int manhattan_distance();//added function for

int nodes_same(),str_compare(),count(),find_h();
void swap(),exit_proc(),print_a_node(),print_nodes();
int toggle_dir(), solvable();
void find_path(),print_path(),find_parent();//find_parent function? huh? it should be struct node *find_parent();
int path[MAX_PATH],path_buf[MAX_PATH];
void prep_dir_to_str(),dir_to_str();
char *dir_to_sdir[4],strategy_s[10]; /* four directions */

struct node {
  int board[N+1][N];
  struct node *next;
};

struct node *start,*goal;
struct node *initialize(),*expand(),*merge(),*filter(),*move(),*prepend(),*append();
struct node *insert_node(),*check_list(),*goal_found();

int n=0;
int main(int argc,char **argv) {
  int iter,cnt=0,total=1,ocnt=0,ccnt=0;
  int perm;		/* perm=odd=no solution, perm=even=solvable */
  struct node *cp,*open,*closed,*succ,*tp;//cp is curr

  struct node *path = malloc(sizeof(struct node));

  open=closed=succ=NULL;
  start=initialize(argc,argv);	/* init initial and goal states */
  //print_a_node(start);
  perm=solvable(start);		/* check if solvable permutation */
  //printf("####### permutation %l\n",perm);
  if (perm & 1) return 1;//if odd -- stop program

  open=start;
  iter=0;
  ocnt=count(start);
  struct node *test = malloc(sizeof(struct node)); //-----test
  while (open) {
    printf("%d: open=%d + clsd=%d = total=%d\n",iter,ocnt,ccnt,ocnt+ccnt);
    ocnt=count(open);
    ccnt=count(closed);

    cp=open; open=open->next; cp->next=NULL; // get the first node from open

    succ = expand(cp);			     /* Find new successors *///either get 4 nodes back or 3-- a pointer to the sturct node
    printf("succ: %d\n",count(succ));
    succ = filter(succ,open);		     /* New succ list */
    succ = filter(succ,closed);		     /* New succ list */

    cnt=count(succ);
    total=total+cnt;

    if (succ) open=merge(succ,open,strategy); /* New open list */
    closed=append(cp,closed);		      /* New closed */


    if (succ && !goal_found(succ, goal)) {
    	for (int i = 0; i<N+1; i++)
		for (int j=0; j<N; j++)
			test->board[i][j] = succ->board[i][j];
		test->next = NULL;
	path = prepend(test, path);
    }

    if ((cp=goal_found(succ,goal))) {
	    printf("goal found after %d iters\n", iter);
	    break;
    }

    iter++;

  }
  printf("%s strategy: %d iterations %d nodes\n",strategy_s,iter+1,total);
  find_path(cp,open,closed);

  return 0;
} /* end of main */

int toggle_dir(int dir){
  int opp_dir;
  // return opp direction
  if (dir == DN) opp_dir = UP;
  else if (dir == UP) opp_dir = DN;
  else if (dir == LT) opp_dir = RT;
  else if (dir == RT) opp_dir = LT;
  return opp_dir;
}

void print_path(int n,int *path){
  int i,p;
  //  for (i=0;i<n;i++) path[i] = path_buf[path_cnt-i-1];
  //  for (i=0;i<path_cnt;i++) printf("%d ",path[i]); printf("\n");
  //  printf("entering print_path: n=%d\n",n);

  ////  for (i=n-1;i>=0;i--) printf("%d ",*(path+i)); printf("\n");
  for (i=n-1;i>=0;i--) {
    p = *(path+i);
    if (i>0) printf("%s -> ",dir_to_sdir[p]);
    else printf("%s",dir_to_sdir[p]);
  }
  printf("\n");
  //  printf("exiting print_path\n");
}

//char **dir_to_sdir={"DN","RT","UP","LT"}; //[4];
void prep_dir_to_str(){
  //...
}

void find_path(struct node *cp,struct node *opnp,struct node *cldp){
  int i,j,dir,opp_dir;
  char *sdir,*opp_sdir;
  int path_cnt=0,p;
  struct node *tp;
  //start from the very last node that matches the goal state because
  //that has the direction info
  //toggle the direction, get the parent
  //look for that parent in closed
  //repeat until you find the initial state
}

void find_parent(struct node *cp,int prev_dir){
  int i,j,k,cnt,row=0,col=j;
}

// Expand: generate successors of the current node
struct node *expand(struct node *cp) {
  int i,j,k,cnt,row=0,col=j;
  struct node *succ,*tp;
  succ=NULL;

  //check where 0 is. find indices i,j
  //printf("okso%d\n",n);//n should be 4
  for (int i = 0; i < N; i++)
  {
	  for (int j = 0; j < N; j++)
		  if (cp->board[i][j] == 0) break;

	  if (j<N) break;
  }

  /* DOWN */
  if (i+1 < N) {
  	tp = move(cp, i, j, i+1, j, DN);
	succ = append(succ, tp);
  }
  /* RIGHT */
  if (j+1 < N){
  	tp = move(cp, i, j, i, j+1, RT);
	succ = append(succ, tp);
  }
  /* UP */
  if (i-1 >= 0){
	  tp = move(cp, i, j, i-1, j, j, UP);
	  succ = append(succ, tp);
  }
  /* LEFT */
  if (j-1 >= 0){
	  tp = move(cp, i, j, i, j-1, LT);
          succ = append(succ, tp);
  }
  return succ;
}

/* attach in the beginning */
struct node *prepend(struct node *tp,struct node *sp) {
  //.....
  return append(sp, tp);
  //return sp;
}

/* attach at the end */
struct node *append(struct node *tp,struct node *sp) {
  struct node *cp;
  //....
  //printf("hi\n");
  cp = tp;
  if (cp == NULL)
	  return sp;
    while(cp){
        cp=cp->next;//yp->next = new
    }
    if (cp == NULL)
	    return sp;
    cp->next=sp;
    print_a_node(sp);
    return tp;
  //return sp;
}

void swap(struct node *cp,int i,int j,int k,int l){
  int tmp;
  //.....
  tmp = i;
  i = j;
  j = tmp;
  cp->board[i][j];
}

struct node *move(struct node *cp,int a,int b,int x,int y,int dir) {
  //struct node *newp;
  int i,j,k,l,tmp;
  // malloc
  struct node *newp = malloc(sizeof(struct node));

  // copy from cp
  for (i = 0; i<N; i++){
  	for (j = 0; j<N; j++){
		newp->board[i][j]=cp->board[i][j];
	}
  }

  // swap two vals
  swap(newp, i, j, k, l);
  // row, col
  // row+1, col

  // compute f,g,h
  newp->board[N][1];//computed g
  //board[N][0,1,2];

  // insert the direction that resulted in this node, used for printing path
  newp->board[N][3] = dir;

  return newp;
}

struct node *goal_found(struct node *cp,struct node *gp){
  int flg=FALSE;
  // check if succ list has goal
  while (cp){
  	if (nodes_same(cp, goal)){
		flg=TRUE;
		break;
	}
	cp = cp->next;
  }
  // if found, return that for finding path else return NULL
  return cp;
}

int count(struct node *cp) {
  int cnt=0;
  //return the number of nodes on the list
  //struct node *head = cp;
  while (cp != NULL){
  	cnt++;
	cp = cp->next;
  }
  return cnt;
}

struct node *merge(struct node *succ,struct node *open,int flg) {
  struct node *csucc,*copen;

  if (flg==DFS) {	/* attach in the front: succ -> ... -> open */
    open = prepend(succ, open);
  }else if (flg==BFS) {	  /* attach at the end: open -> ... -> succ */
    open = append(open, succ);
    append(csucc, copen);
  }else if (flg==BEST) {	/* Best first: sort on h value */
    //...
  }else{			/* A* search: sort on f=g+h value */
    //increasing f value
	  csucc=succ;
        while(csucc->next){
            open = insert_node(succ,open);
            csucc=csucc->next;
        }
  }
  return open;
}


/* insert succ into open in ascending order of x value, where x is an array
   index: 0=f,1=g,h=2 of board[N][x]
 */
//sorting for astar
struct node *insert_node(struct node *succ,struct node *open,int x) {
   int cnt;
   struct node *copen,*topen;
   //...
   copen = succ;
   topen = open;
   //initialize open if empty
   if (open==NULL){
        open = succ;
        return open;
   }
    struct node *temp;

    while (topen->next) {
        if (copen->board[N][0] > topen->board[N][0]) {
            topen = topen->next;
        }
        else{
            break;
        }
    }

    //Insert succ into open
    temp = topen;
    topen=open;
    while (topen->next){
        if (nodes_same(topen->next,temp)){
            topen->next=copen;
            copen->next=temp;
            break;
            return open;
        }
        else {
            topen = topen->next;
        }
   }
   return open;
}

int find_h(int current[N+1][N],int goalp[N+1][N]) {
  int h=0,i,j,k,l,done;
  // ...
  return h;
}

/* a=b=x[N][N] */
int nodes_same(struct node *xp,struct node *yp) {
  int i,j,flg=FALSE;
  //...
  for (i=0; i<n; i++){
  	for (j=0;j<n;j++){
		if (xp->board[i][j] != yp->board[i][j])
			return flg;
	}
  }
  flg=TRUE;
  return flg;
}

/******************************************************************************
  Check succ against open and closed. Remove those succ nodes that are in open or closed.
******************************************************************************/
struct node *filter(struct node *succ,struct node *hp){
   struct node *lsp,*rsp;	/* lsp=left succ p, rsp=right succ p */
   struct node *tp;		/* temp ptr */

   lsp=rsp=succ;
   tp=hp;//cp stands for closed pointer
   //print_nodes(succ, "TEST");
   while (rsp){
   	tp = hp;
	while (tp && !(nodes_same(tp, rsp))){
		tp=tp->next;
	}
	//either cp is null, or cp & rp match
	if (tp == NULL){
		lsp=rsp;
		rsp=rsp->next;
	}
	else{//if cp is not null and they are the same
		if(lsp==rsp){//they are the same
			succ = lsp = rsp = rsp->next;
		}
		else{//if they are not the same
			lsp->next=rsp->next;
			rsp=rsp->next;
		}
	}
   }
   return succ;
}

void print_nodes(struct node *cp,char name[20]) {
  int i;
  printf("%s:\n",name);
  while (cp) { print_a_node(cp); cp=cp->next; }
}

void print_a_node(struct node *np) {
  int i,j;
  for (i=0;i<N+1;i++) {
    for (j=0;j<N;j++) printf("%2d ",np->board[i][j]);
    printf("\n");
  }
  printf("\n");
}

//cnt=odd -> no solution, cnt=even=solvable
int solvable(struct node *cp) {
  int i,j,k=0,lst[NxN],cnt=0,total=0;
  //flatten the board in to flat lst and work with that
  for (i=0; i < N; i++){
  	for (j=0; j < N; j++){
		lst[i * N + j]=cp->board[i][j];
	}
  }
  for (i = 0; i < N * N - 1; i++){
        for (j = i + 1; j < N * N; j++){
            if (lst[j] && lst[i] && lst[i] > lst[j])
                cnt++;
        }
  }
  total = cnt;
  //printf("cnt is: %d",cnt);
  printf("%i\n",total);
  return total;	/* return the number we discussed in class today 3/31/2015*/
}

/* fif 0 1 2 4 5 6 3 8 9 10 7 12 13 14 11 15 astar */
struct node *initialize(int argc, char **argv){
  int i,j,k,npe,n,idx,gidx,inv;
   struct node *tp;

   tp=(struct node *) malloc(sizeof(struct node));
   idx = 1;
   for (j=0;j<N;j++)
     for (k=0;k<N;k++) tp->board[j][k]=atoi(argv[idx++]);
   for (k=0;k<N;k++) tp->board[N][k]=0;	/* set f,g,h of initial state to 0 */
   tp->next=NULL;
   start=tp;

   printf("init state: \n");
   print_a_node(start);

   tp=(struct node *) malloc(sizeof(struct node));
   gidx = 1;
   for (j=0;j<N;j++)
     for (k=0;k<N;k++) tp->board[j][k] = gidx++;
   tp->board[N-1][N-1] = 0;		/* empty tile=0 */
   for (k=0;k<N;k++) tp->board[N][k]=0;	/* set f,g,h of goal state to 0 */
   tp->next=NULL;
   goal=tp;

   printf("goal state: \n");
   print_a_node(goal);


   strcpy(strategy_s,argv[idx]);//just idx returned null for some reason

   if (strcmp(strategy_s,"dfs")==0) strategy=DFS;
   else if (strcmp(strategy_s,"bfs")==0) strategy = BFS;
   else if (strcmp(strategy_s,"best")==0) strategy=BEST;
   else if (strcmp(strategy_s,"bb")==0) strategy=BB;
   else if (strcmp(strategy_s,"astar")==0) strategy=ASTAR;
   else strategy=UNKNOWN;
   printf("strategy=%s\n",strategy_s);

   return start;
}

void exit_proc(char *msg){
   printf("Error: %s\n",msg);
   exit(1);
}

/*****************************************************************************
 End of file: fif.c. Fifteen Puzzle, Sequential A* 1 processor version.
*****************************************************************************/
