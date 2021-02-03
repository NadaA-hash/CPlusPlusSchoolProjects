/* HW6 read cafefully and follow all the instructions/comments
   this program compiles and runs as is although it's not doing much.
   DO NOT CHANGE THE SEED. Changing seed gives a different sequence of
   numbers but we want the same numbers for comparison purposes.

   FLOATING point radix sort requires some substantial changes to this code
   float n, lst[N],tmp[N];
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>

#define OK 1
#define NOK 0
#define NELM 100		/* default 100 elements */
#define N 1048576		/* 2^30 or 1 meg elements  */
//#define N 2097152
//#define N 4194304
//#define N 8388608
//#define N 16777216
//#define N 33554432

void selection_sort();
void swap();

void merge_sort();
void msort_recursive();
void merge();

void int_radix_sort();
void float_radix_sort();
void swap();
void self_check();

int rand(void);
void srand();
int rand_r();
void init_lst();
void init_float_lst();
void print_lst();
void print_float_lst();

int n, lst[N], buf[N];
int tmp[N];
float fst[N], fuf[N];
int main(int argc,char **argv) {

  long int del_sec,del_msec;
  struct timeval tv_s,tv_e;

  if (argc>1) n = atoi(argv[1]);
  else n = NELM;
  printf("n=%d\n",n);
  init_lst(lst,n);
  //print_lst(lst,n);

  gettimeofday(&tv_s, NULL);
  printf("Elapsed time:\n");

  selection_sort(lst,n);
  gettimeofday(&tv_e, NULL);

  del_sec = tv_e.tv_sec + tv_e.tv_usec / 1e6 - tv_s.tv_sec - tv_s.tv_usec / 1e6;
  del_msec = tv_e.tv_sec * 1000 + tv_e.tv_usec / 1000 - tv_s.tv_sec * 1000 - tv_s.tv_usec / 1000;
  printf("\nSelection:\n%ld secs\n%ld milli secs\n", del_sec,del_msec);
  self_check(lst,n);
  self_check(buf,n);

  init_lst(lst,n);

  gettimeofday(&tv_s, NULL);
  merge_sort(lst, tmp, n);
  gettimeofday(&tv_e, NULL);

  del_sec = tv_e.tv_sec + tv_e.tv_usec / 1e6 - tv_s.tv_sec - tv_s.tv_usec / 1e6;
  del_msec = tv_e.tv_sec * 1000 + tv_e.tv_usec / 1000 - tv_s.tv_sec * 1000 - tv_s.tv_usec / 1000;
  printf("\nMerge:\n%ld secs\n%ld milli secs\n", del_sec,del_msec);
  self_check(lst,n);
  self_check(buf,n);

  init_lst(lst,n);
  gettimeofday(&tv_s, NULL);
  int_radix_sort(lst,tmp,n);
  gettimeofday(&tv_e, NULL);

  del_sec = tv_e.tv_sec + tv_e.tv_usec / 1e6 - tv_s.tv_sec - tv_s.tv_usec / 1e6;
  del_msec = tv_e.tv_sec * 1000 + tv_e.tv_usec / 1000 - tv_s.tv_sec * 1000 - tv_s.tv_usec / 1000;
  printf("\nInt Radix:\n%ld secs\n%ld milli secs\n", del_sec,del_msec);
  self_check(lst,n);
  self_check(buf,n);

  init_float_lst(fst,n);
  gettimeofday(&tv_s, NULL);
  float_radix_sort(fst,fuf,n);
  gettimeofday(&tv_e, NULL);

  del_sec = tv_e.tv_sec + tv_e.tv_usec / 1e6 - tv_s.tv_sec - tv_s.tv_usec / 1e6;
  del_msec = tv_e.tv_sec * 1000 + tv_e.tv_usec / 1000 - tv_s.tv_sec * 1000 - tv_s.tv_usec / 1000;
  printf("\nFloat Radix:\n%ld secs\n%ld milli secs\n", del_sec,del_msec);

  self_check(fst,n);
  self_check(fuf,n);

  return 0;
}

void swap(int *xpoint, int *ypoint){
	int temp = *xpoint;
	*xpoint = *ypoint;
	*ypoint = temp;
}

void selection_sort(int lst[],int n){
  int min;
  for (int i=0; i<n-1; i++){
	  min = i;
  	for (int j=i+1; j<n; j++){
		if (lst[j] < lst[min])
			min = j;
	}
	swap(&lst[min], &lst[i]);
  }
}

void merge_sort(int lst[], int buf[], int n){
	msort_recursive(lst, buf, 0, n-1);
}

//use recursion
void msort_recursive(int lst[], int buf[], int left, int right){
	int mid=0;

	if (right > left){
		mid = (right + left)/2;

		msort_recursive(lst, buf, left, mid);
		msort_recursive(lst, buf, mid+1, right);

		//merge two parts
		merge(lst, buf, left, mid+1, right);
	}
}

void merge(int lst[], int buf[], int left, int mid, int right){
		int i = left;
                int j = mid;
                int k = left;
                while ((i <= mid - 1) && (j <= right)) {
                        if (lst[i] <= lst[j])
                                buf[k++] = lst[i++];
                        else
                                buf[k++] = lst[j++];
                }
                while (i<=mid-1)
                        buf[k++] = lst[i++];
                while (j<= right)
                        buf[k++] = lst[j++];
                for (i = left; i <= right; i++)
                        lst[i]=buf[i];
}

//fix the bucket size to 256. run 4 passes where each pass processes 8 bits
//use lst and tmp only. do not use any more memory of size n.
void radix_sort(int group) {
  int i,j,mask,buckets;
  int flag,*src,*dst;

  int bits = (1<<group)/group;

  buckets = 1<<group;
  mask = buckets-1;//shift 1 to the left 'radix' number of times, then subtract 1 to get all digits turned into 1

  int cnt[buckets], map[buckets];

  src=lst;
  dst=buf;
  flag=OK;

  for (i=0; i<bits; i=i+group){
    //Intalizing count
    for (j = 0; j < buckets; j++)
	    cnt[j]=0;
    //Count
    for (j=0; j<n; j++){
	cnt[(src[j]>>i) & mask]++;
    }
    //Map
    map[0]=0;
    for (j=1; j<buckets; j++){
	    map[j]=map[j-1]+cnt[j-1];
    }

    //set src/dst to lst/buf depending on iter number
    if (flag == OK){
	    for (j=0; j < n; j++)
		    dst[map[(src[j]>>i) & mask]++] = src[j];
	flag=NOK;
    }
    else if (flag == NOK){
	for (j=0; j < n; j++)
               src[map[(dst[j]>>i) & mask]++] = dst[j];
	    flag=OK;
    }
  }

}


void int_radix_sort(int lst[], int buf[], int n){
	int radix = 8;//for 8 bits
	radix_sort(radix);
}

void float_radix_sort(int lst[], int buf[], int n){
	float value = 8.0;
	int *intptr=(int *)&value;
	int int_value = *intptr;
        //print_float_lst(lst,n);
	radix_sort(int_value);
	//print_float_lst(lst,n);

	/*for (int i = 0; i < n; i++){
		if (lst[i] <0)
			break;
	}*/
}

void print_lst(int *l,int n){
  int i;
  for (i=0; i<n; i++) {
    printf("%d ",l[i]);
  }
  printf("\n");
}

void print_float_lst(float *l, int n){
  int i;
  for (i=0; i<n; i++) {
    printf("%f ",l[i]);
  }
  printf("\n");

}

void init_lst(int *l,int n){
  int i;
  //  int seed = time(0) % 100;	/* seconds since 1/1/1970 */
  //  printf ("seed=%d\n", seed);
  srand(1234);			/* SEED */
  for (i=0; i<n; i++) {
    l[i] = rand();
  }
}

void init_float_lst(float *l, int n){
	int i;
	srand((unsigned int)1234);
	for (i=0; i<n; i++){
		l[i] = ((float)rand()/(float)(RAND_MAX));
	}
}
void self_check(int *lst,int n) {
  int i,j,flag=OK,*lstp;

  lstp = lst;
  for (i=0;i<n-1;i++)
     if (lstp[i] > lstp[i+1]) { flag = NOK; break; }

  if (flag == OK) printf("sorted\n");
  else printf("NOT sorted at %d\n",i);
}

/* End of file */
