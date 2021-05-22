/*
 * Capstone Design - Jin Hur, Yeongki Kweon, Jooyoung Kim - Dankook univ, Dept.SW
 * false1.c - A simple parallel program with scalars to demo false sharing
 */

 //	compile as: gcc -DN=1000000 -D[GOOD|BAD] false1.c -lpthread -lrt -o false1
 //		
 //	run as: ./false1 <numthreads>	


#include <pthread.h>	
#include <stdio.h>
#include <stdlib.h>	// for atoi() etc

#ifndef 	N
#define 	N		100000000
#endif
#ifndef 	REPEAT
#define 	REPEAT	1
#endif
#define		INTSIZE		4
#define		CACHELINE	64
#define 	MAXTHREADS 	CACHELINE/INTSIZE

#ifdef 	GOOD
#define 	MSG 	"# False1: Good   : N=%d : Threads=%d \n"
#endif
#ifdef	BAD_FS
#define 	MSG 	"# False1: Bad-FS : N=%d : Threads=%d \n"
#endif

// GOOD 모드에서 쓰레드들은 서로 다른 함수를 호출, 해당 함수의 지역변수에 값을 갱신함으로써 거짓 공유 완화
void *func1(void *p); void *func2(void *p); void *func3(void *p);
void *func4(void *p); void *func5(void *p); void *func6(void *p);
void *func7(void *p); void *func8(void *p); void *func9(void *p);
void *func10(void *p); void *func11(void *p); void *func12(void *p);
void *func13(void *p); void *func14(void *p); void *func15(void *p);
void *func16(void *p);

// BAD_FS 모드에서 쓰레드들은 전역에 순차적으로 선언되어 있는 변수들에 값 갱신 => 거짓 공유 유발
// 16 * 4bytes = cache line size = 64bytes
int count1 = 0;		
int count2 = 0;	
int count3 = 0, count4 = 0, count5 = 0, count6 = 0;
int count7 = 0, count8 = 0, count9 = 0, count10 = 0;
int count11 = 0, count12 = 0, count13 = 0;
int count14 = 0, count15 = 0, count16 = 0;

int tcount = 0, numthreads = 1;

int main(int argc, char **argv)
{
	int 				i, j;
	pthread_t 			main_tid, tid[MAXTHREADS];
	int 				myid[MAXTHREADS], status;

	if (argc != 2) {
		printf("Usage: %s <num_threads> (max threads = 8)\n", argv[0]);
		exit(0);
	}
	numthreads = atoi(argv[1]);
	if (numthreads > MAXTHREADS) {
		printf("nthreads > MAXTHREADS (8)\n");
		exit(0);
	}

	for (i = 0; i < numthreads; i++)
		myid[i] = i;

	switch (numthreads) {
	case 16: pthread_create(&tid[MAXTHREADS - 1], NULL, func16, &myid[MAXTHREADS - 1]);
	case 15: pthread_create(&tid[MAXTHREADS - 2], NULL, func15, &myid[MAXTHREADS - 2]);
	case 14: pthread_create(&tid[MAXTHREADS - 3], NULL, func14, &myid[MAXTHREADS - 3]);
	case 13: pthread_create(&tid[MAXTHREADS - 4], NULL, func13, &myid[MAXTHREADS - 4]);
	case 12: pthread_create(&tid[MAXTHREADS - 5], NULL, func12, &myid[MAXTHREADS - 5]);
	case 11: pthread_create(&tid[MAXTHREADS - 6], NULL, func11, &myid[MAXTHREADS - 6]);
	case 10: pthread_create(&tid[MAXTHREADS - 7], NULL, func10, &myid[MAXTHREADS - 7]);
	case  9: pthread_create(&tid[MAXTHREADS - 8], NULL, func9, &myid[MAXTHREADS - 8]);
	case  8: pthread_create(&tid[MAXTHREADS - 9], NULL, func8, &myid[MAXTHREADS - 9]);
	case  7: pthread_create(&tid[MAXTHREADS - 10], NULL, func7, &myid[MAXTHREADS - 10]);
	case  6: pthread_create(&tid[MAXTHREADS - 11], NULL, func6, &myid[MAXTHREADS - 11]);
	case  5: pthread_create(&tid[MAXTHREADS - 12], NULL, func5, &myid[MAXTHREADS - 12]);
	case  4: pthread_create(&tid[MAXTHREADS - 13], NULL, func4, &myid[MAXTHREADS - 13]);
	case  3: pthread_create(&tid[MAXTHREADS - 14], NULL, func3, &myid[MAXTHREADS - 14]);
	case  2: pthread_create(&tid[MAXTHREADS - 15], NULL, func2, &myid[MAXTHREADS - 15]);
	case  1: pthread_create(&tid[MAXTHREADS - 16], NULL, func1, &myid[MAXTHREADS - 16]);
	}

	for (i = 0; i < numthreads; i++) {
		pthread_join(tid[i], NULL);
	}

	tcount += count1 + count2 + count3 + count4 + count5 + count6 + count7 + count8;
	tcount += count9 + count10 + count11 + count12 + count13 + count14 + count15 + count16;

#ifdef BAD_FS
	if (numthreads == 1) {
		printf("# False1: Good   : N=%d : Threads=%d \n", N, numthreads);
		return 0;
	}
#endif
	printf(MSG, N, numthreads);

	pthread_exit(NULL);
	return 0;
}

void * func1(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads;
	int 		myend = ((myid + 1) * (long long)N) / numthreads;

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;   // 함수 지역변수 사용, 거짓 공유 회피
#else
			count1++;	 // 공유 영역의 변수 사용, 거짓 공유 발생
#endif   			
		}
	}
#ifdef GOOD
	count1 = mycount;
#endif
}
//------------------------------------------
void * func2(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;   
#else
			count2++;  
#endif
		}
	}
#ifdef GOOD
	count2 = mycount;
#endif
}
//------------------------------------------	
void * func3(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count3++;
#endif
		}
	}
#ifdef GOOD
	count3 = mycount;
#endif
}
//------------------------------------------
void * func4(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count4++;
#endif
		}
	}
#ifdef GOOD
	count4 = mycount;
#endif
}
//------------------------------------------	
void * func5(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count5++;
#endif
		}
	}
#ifdef GOOD
	count5 = mycount;
#endif
}
//------------------------------------------
void * func6(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count6++;
#endif
		}
	}
#ifdef GOOD
	count6 = mycount;
#endif
}
//------------------------------------------	
void * func7(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count7++;
#endif
		}
	}
#ifdef GOOD
	count7 = mycount;
#endif
}
//------------------------------------------	
void * func8(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count8++;
#endif
		}
	}
#ifdef GOOD
	count8 = mycount;
#endif
}
//------------------------------------------	
void * func9(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count9++;
#endif
		}
	}
#ifdef GOOD
	count9 = mycount;
#endif
}
//------------------------------------------	
void * func10(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count10++;
#endif
		}
	}
#ifdef GOOD
	count10 = mycount;
#endif
}
//------------------------------------------	
void * func11(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count11++;
#endif
		}
	}
#ifdef GOOD
	count11 = mycount;
#endif
}
//------------------------------------------	
void * func12(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count12++;
#endif
		}
	}
#ifdef GOOD
	count12 = mycount;
#endif
}
//------------------------------------------	
void * func13(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count13++;
#endif
		}
	}
#ifdef GOOD
	count13 = mycount;
#endif
}
//------------------------------------------	
void * func14(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count14++;
#endif
		}
	}
#ifdef GOOD
	count14 = mycount;
#endif
}
//------------------------------------------	
void * func15(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count15++;
#endif
		}
	}
#ifdef GOOD
	count15 = mycount;
#endif
}
//------------------------------------------	
void * func16(void *p)
{
	int 		myid = *((int *)p);
	int 		i, j, mycount = 0;
	int 		mystart = (myid * (long long)N) / numthreads; 	
	int 		myend = ((myid + 1) * (long long)N) / numthreads;	

	for (i = 0; i < REPEAT; ++i) {
		for (j = mystart; j < myend; j++) {
#ifdef GOOD
			mycount++;
#else
			count16++;
#endif
		}
	}
#ifdef GOOD
	count16 = mycount;
#endif
}

}