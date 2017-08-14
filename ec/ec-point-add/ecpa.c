#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#define RABIN 15	// Miller-Rabin variable for testing if a number is prime
#define OPTIONS "1) Point addition on curve\n2) Check if point is on curve\n3) Point Multiplication (naive)\n4) Get all points (Brute Force)\n0) Quit\n\nChoice: "

short on_curve(mpz_t a, mpz_t b, mpz_t p, mpz_t Px, mpz_t Py);
short protector(mpz_t x);
unsigned long hasse(mpz_t p);
void get_all_p(mpz_t a, mpz_t b, mpz_t p);
void point_add(mpz_t a, mpz_t b, mpz_t p, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t Rx, mpz_t Ry, mpz_t infy);
void point_mul(mpz_t a, mpz_t b, mpz_t p);
void proof(mpz_t a, mpz_t b, mpz_t p);
void point_add_input(mpz_t a, mpz_t b, mpz_t p);
void condition(mpz_t a, mpz_t b, mpz_t p);

unsigned long **all_p;
unsigned long posctr=0;

// returns 0 if point is on curve, else 1
short on_curve(mpz_t a, mpz_t b, mpz_t p, mpz_t Px, mpz_t Py){
	mpz_t x,
	  	  y,
	  	  xtwo;

	mpz_init(x);
	mpz_init(y);
	mpz_init(xtwo);

	// paste x and compare with y
	mpz_pow_ui(x,Px,3);
	mpz_mul(xtwo,a,Px);
	mpz_add(xtwo,xtwo,b);
	mpz_add(x,x,xtwo);
	mpz_mod(x,x,p);
	mpz_pow_ui(y,Py,2);
	mpz_mod(y,y,p);

	if(!mpz_cmp(x,y)){
		mpz_clear(x);
		mpz_clear(y);
		mpz_clear(xtwo);
		return 0;
	}

	mpz_clear(x);
	mpz_clear(y);
	mpz_clear(xtwo);
	return 1;
}

// protect stack, if x is longer than long max, return 1 and don't mpz_get_ui
short protector(mpz_t x){
	if(mpz_cmp_ui(x,LONG_MAX)>0)
		return 1;
	return 0;
}

// hasse's theorem to get expected maximum amount of points P (hasse's upper bound)
unsigned long hasse(mpz_t p){
	mpz_t tmp,
		  E_max;
	unsigned long E_up=0;

	mpz_init(tmp);
	mpz_init(E_max);

	// Hasse's upper bound p+1+2*sqrt(p)
	mpz_add_ui(E_max,p,1);
	mpz_sqrt(tmp,p);
	mpz_mul_ui(tmp,tmp,2);
	mpz_add(E_max,E_max,tmp);

	if(protector(E_max)){
		mpz_clear(E_max);
		mpz_clear(tmp);
		return 0;
	}
	E_up=mpz_get_ui(E_max);

	//gmp_printf("Hasse's test upper bound: %ld\n", E_up);
	mpz_clear(E_max);
	mpz_clear(tmp);
	return E_up;
}

void get_all_p(mpz_t a, mpz_t b, mpz_t p){
	mpz_t Px,
		  Py;

	mpz_init(Px);
	mpz_init(Py);

	// security trigger
	short brk=0;

	unsigned long E;
	E=hasse(p);
	if(!E){ // check hasse() protector for info
		gmp_printf("Error, expected amount of points too high!\n");
	}
	else{
		//printf("E: %d\n", E);
		// allocate space for the points
		all_p=calloc(E,sizeof(unsigned long));
		for(unsigned long i=0; i<E; i++){
			all_p[i]=calloc(2,sizeof(unsigned long));
		}
		// try if points are on curve and if yes, save them
		for(unsigned long i=0; mpz_cmp_ui(p,i)>0; i++){
			for(unsigned long j=0; mpz_cmp_ui(p,j)>0; j++){
				if((posctr+1)==E){
					gmp_printf("OUT OF BOUNDS; REALLOC NEEDED\n");
					brk=1;
					break;
				}
				mpz_set_ui(Px,i);
				mpz_set_ui(Py,j);
				if(!on_curve(a,b,p,Px,Py)){
					all_p[posctr][0]=i;
					all_p[posctr][1]=j;
					posctr++;
				}
			}
			if(brk)
				break;
		}
	}
	if(!brk){
		gmp_printf("Found %ld points:\n", posctr+1);
		for(unsigned long i=0; i<posctr; i++){
			gmp_printf("(%ld,%ld), ",all_p[i][0],all_p[i][1]);
		}
		gmp_printf("O\n");
	}
}

void point_add(mpz_t a, mpz_t b, mpz_t p, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t Rx, mpz_t Ry, mpz_t infy){
	mpz_t sRx,
		  sRy,
		  s;

	mpz_init(sRx);
	mpz_init(sRy);
	mpz_init(s);

	// check for infinity
	mpz_sub(infy,p,Qy);
	if((!mpz_cmp(Px,Qx))&&(!mpz_cmp(Py,infy))){
		gmp_printf("\nQ is the inverse of P\nThe result is infinite!\n");
		mpz_set_ui(Rx,0);
		mpz_set_ui(Ry,0);
	}else{
		// Calculate s:

		// Point doubling
		if((!mpz_cmp(Px, Qx))&&(!mpz_cmp(Py, Qy))){
			mpz_pow_ui(sRx, Px, 2);
			mpz_mul_ui(sRx, sRx, 3);
			mpz_add(sRx, sRx, a);
			mpz_mod(sRx, sRx, p);
			mpz_mul_ui(sRy, Py, 2);
			mpz_mod(sRy, sRy, p);
			mpz_invert(sRy, sRy, p);
			// finally, merge s
			mpz_mul(s, sRx, sRy);
			mpz_mod(s, s, p);
		}
		// Point addition
		else{
			mpz_sub(sRy,Qy,Py);
			if(mpz_cmp_ui(sRy,0)<0)
				mpz_add(sRy,p,sRy);
			mpz_sub(sRx,Qx,Px);
			if(mpz_cmp_ui(sRx,0)<0)
				mpz_add(sRx,p,sRx);
			mpz_invert(sRx,sRx,p);
			// merge s
			mpz_mul(s,sRy,sRx);
			mpz_mod(s,s,p);
		}

		// Calculate Rx:
		mpz_pow_ui(Rx, s, 2);
		mpz_sub(Rx, Rx, Px);
		mpz_sub(Rx, Rx, Qx);
		mpz_mod(Rx, Rx, p);

		// Calculate Ry:
		mpz_sub(Ry,Px,Rx);
		mpz_mul(Ry,s,Ry);
		mpz_sub(Ry,Ry,Py);
		mpz_mod(Ry,Ry,p);

		
		// free variables only needed for R calculation
		mpz_clear(sRx);
		mpz_clear(sRy);
		mpz_clear(s);
	}
}

void point_mul(mpz_t a, mpz_t b, mpz_t p){
	int iter=1, target=0;

	mpz_t Px,
	  	  Py,
		  Qx,
		  Qy,
		  Rx,
		  Ry,
		  infy;

	char *tmp, *garbage;	// temp and garbage vars for input handling
	tmp=calloc(1,sizeof(long)+1);
	long in;

	mpz_init(Px);
	mpz_init(Py);
	mpz_init(Qx);
	mpz_init(Qy);
	mpz_init(Rx);
	mpz_init(Ry);
	mpz_init(infy);

	gmp_printf("Please enter an start point\n");
	gmp_printf("P(x): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Px,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("P(y): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Py,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("What point is this eg. 2P, 4P etc (from 2 to #E)...\n(x)P: ");
	fgets(tmp, sizeof(long)+1, stdin);
	iter=strtol(tmp, &garbage, sizeof(long)+2);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("What point do you want to Multiplicate to?\n(x)P: ");
	fgets(tmp, sizeof(long)+1, stdin);
	target=strtol(tmp, &garbage, sizeof(long)+2);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	if((iter>target)||iter<0||target<0){
		gmp_printf("Wrong input, ABORT!\n");
		EXIT_FAILURE;
	}
	if(iter<2){
		iter=2;
		gmp_printf("Start to small, set to 2\n");
	}


	
	mpz_set(Qx,Px);
	mpz_set(Qy,Py);
	for(int i=iter; i<target+1; i++){
		//printf("iter: %d, target: %d\n", iter, target);
		//printf("i: %d\n", i);
		//puts("DEBUG2.x");
		point_add(a,b,p,Px,Py,Qx,Qy,Rx,Ry,infy);
		gmp_printf("%dP(%Zd,%Zd)\n",i,Rx,Ry);
		mpz_set(Px,Rx);
		mpz_set(Py,Ry);
	}

	mpz_clear(Px);
	mpz_clear(Py);
	mpz_clear(Qx);
	mpz_clear(Qy);
	mpz_clear(Rx);
	mpz_clear(Ry);
	mpz_clear(infy);
}

// Check if an point P is on the elliptic curve
void proof(mpz_t a, mpz_t b, mpz_t p){
	mpz_t Px,
	  	  Py;

	char *tmp, *garbage;	// temp and garbage vars for input handling
	tmp=calloc(1,sizeof(long)+1);
	long in;

	mpz_init(Px);
	mpz_init(Py);
	

	gmp_printf("P(x): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Px,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("P(y): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Py,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	

	if(!on_curve(a,b,p,Px,Py))
		gmp_printf("Point P(%Zd,%Zd) is on the elliptic curve\n", Px, Py);
	else
		gmp_printf("Point P(%Zd,%Zd) is NOT on the elliptic curve\n", Px, Py);

	mpz_clear(Px);
	mpz_clear(Py);
}

void point_add_input(mpz_t a, mpz_t b, mpz_t p){
	// point parameters
	mpz_t Px,
		  Py,
		  Qx,
		  Qy,
		  Rx,
		  Ry,
		  infy;

	char *tmp, *garbage;
	tmp=calloc(1,sizeof(long)+1);
	long in;

	mpz_init(Px);
	mpz_init(Py);
	mpz_init(Qx);
	mpz_init(Qy);
	mpz_init(Rx);
	mpz_init(Ry);
	mpz_init(infy);

	// define points to be added
	gmp_printf("P(x): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Px,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("P(y): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Py,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("Q(x): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Qx,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage

	gmp_printf("Q(y): ");
	fgets(tmp, sizeof(long)+1, stdin);
	in=strtol(tmp, &garbage, sizeof(long)+2);
	mpz_set_ui(Qy,in);
	memset(tmp, 0, sizeof(long)+1);	// zerofill tmp to avoid garbage


	
	point_add(a, b, p, Px,Py,Qx,Qy,Rx,Ry,infy);

	// resulting point
	gmp_printf("\nR(%Zd,%Zd)\n",Rx,Ry);

	mpz_clear(Px);
	mpz_clear(Py);
	mpz_clear(Qx);
	mpz_clear(Qy);
	mpz_clear(Rx);
	mpz_clear(Ry);
	mpz_clear(infy);
}

/*check if ec conditions are met 
* 1. if x = 4*a^3 + 27*b^2 mod p is zero
* or 2. if p is not prime the conditions
* are not met */
void condition(mpz_t a, mpz_t b, mpz_t p){
	// temporary auxiliary variables
	mpz_t x,
		  tmp_a,
		  tmp_b;
	mpz_init(x);	
	mpz_init(tmp_a);
	mpz_init(tmp_b);
	// calculate condition 1.
	mpz_pow_ui(tmp_a,a,3);
	mpz_mul_ui(tmp_a,tmp_a,4);
	mpz_pow_ui(tmp_b,b,2);
	mpz_mul_ui(tmp_b,tmp_b,27);
	mpz_add(x,tmp_a,tmp_b);
	mpz_mod(x,x,p);

	// if parameters aren't ok, free space and quit
	if((!mpz_cmp_ui(x,0))||(!mpz_probab_prime_p(p,RABIN))){		// check condition 1 & 2
		gmp_printf("Elliptic Curve condition not met. Abort\n");
		mpz_clear(a);
		mpz_clear(b);
		mpz_clear(p);
		mpz_clear(tmp_a);
		mpz_clear(tmp_b);
		mpz_clear(x);
		EXIT_FAILURE;
	}
	// free the condition variables space
	mpz_clear(tmp_a);
	mpz_clear(tmp_b);
	mpz_clear(x);
}

int main(int argc, char *argv[]){
	// curve parameters
	mpz_t a,
		  b,
		  p;


	char *tmp, *garbage;	// for input and temp
	tmp=calloc(1,sizeof(long));
	long in;

	mpz_init(a);
	mpz_init(b);
	mpz_init(p);

	// Get curve parameters from user input
	gmp_printf("a: ");
	fgets(tmp, sizeof(long), stdin);
	in=strtol(tmp, &garbage, 10);
	mpz_set_ui(a,in);
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage

	gmp_printf("b: ");	
	fgets(tmp, sizeof(long), stdin);
	in=strtol(tmp, &garbage, 10);
	mpz_set_ui(b,in);
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage

	gmp_printf("p: ");
	fgets(tmp, sizeof(long), stdin);
	in=strtol(tmp, &garbage, 10);
	mpz_set_ui(p,in);		
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage
	
	free(tmp);

	condition(a,b,p);	// check if ec conditions are met

	short opt=1;	// option trigger
	char *tmp_opt;
	tmp_opt=calloc(1,sizeof(short));

	// menu
	while(opt){
		gmp_printf("\n---------------------------\ny^2 = x^3 + %Zd*x + %Zd mod %Zd\n", a,b,p);	// print curve
		gmp_printf("%s", OPTIONS);	// print options
		fgets(tmp_opt, 10, stdin);
		opt=strtol(tmp_opt, &garbage, 10);
		if(!opt)
			break;
		else if(opt==1)
			point_add_input(a,b,p);
		else if(opt==2)
			proof(a,b,p);
		else if(opt==3)
			point_mul(a,b,p);
		else if(opt==4)
			get_all_p(a,b,p);
		else
			gmp_printf("Not an valid option!\n");
		memset(tmp_opt, 0, sizeof(short));
	}

	// free the used variables
	gmp_printf("Farewell!\n");
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(p);
	if(all_p)
		free(all_p);
	return 0;
}