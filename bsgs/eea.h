#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SWAP(x, y) {unsigned long int q; q=y; y=x; x=q;}
#define BSZ 100
#define MR(x, y) (x % y)
#define MQ(x, y, z) ((x-y)/z)
#define MS(x, y, z) (x-(y*z))
#define MT(x, y, z) (x-(y*z))

typedef struct letskillgod {
	unsigned long int r0;
	unsigned long int r1;
	unsigned long int s_fin;
	unsigned long int t_fin;
	unsigned long int gcd;
} in;

void eec(in *st){
	int i=1;
	unsigned long int p=st->r0;
	unsigned long int *s,*t,*r,*q;
	r = calloc(BSZ,sizeof(unsigned long int));
	s = calloc(BSZ,sizeof(unsigned long int));
	t = calloc(BSZ,sizeof(unsigned long int));
	q = calloc(BSZ,sizeof(unsigned long int));
	r[0]=st->r0;
	r[1]=st->r1;
	s[0]=1;	s[1]=0;
	t[0]=0;	t[1]=1;
	do{
		i++;
		r[i]=MR(r[i-2], r[i-1]);
		q[(i-1)]=MQ(r[i-2],r[i],r[i-1]);
		s[i]=MS(s[i-2],q[i-1],s[i-1]);
		t[i]=MT(t[i-2],q[i-1],t[i-1]);
	}while(r[i]!=0);
	st->s_fin=s[i-1];
	st->t_fin=t[i-1];
	if(st->t_fin<0){
		st->t_fin=p + st->t_fin;
	}
	st->gcd=r[i-1];
}

int eea(unsigned long int a, unsigned long int p){
	in *st=malloc(sizeof(in));
	st->r0=p;
    st->r1=a;
	if(st->r0<st->r1){
		SWAP(st->r0, st->r1);
	}
	eec(st);
	return st->t_fin;
}
