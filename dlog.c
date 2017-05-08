#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>

/**********************************
* Lösung Aufgabe 5 c)
*
* ./dlog 7464 33115 35731
* x = 19637
*
* Lorenz Troiza
* 108014236463
***********************************/

int sqm(mpz_t result, mpz_t g, mpz_t e, mpz_t modulus){
	mpz_t A; // A to store temporary states
	mpz_init(A);
	mpz_set(A,g);
	mpz_set_ui(result,1);
	while(mpz_cmp_ui(e,0)>0){
		// As long as the last bit of the exponent is zero, square
		while(mpz_tstbit(e,0)==0){
				mpz_powm_ui(A,A,2,modulus);
				mpz_tdiv_q_2exp(e,e,1); // bit shift right the exponent
			}
			mpz_clrbit(e,0); // lower the exponent by one
			mpz_mul(result,result,A);
			mpz_mod(result,result,modulus);
	}
	mpz_clear(A);
	return 0;
}

int main(int argc, char *argv[]){

	// Check for wrong input
	for(int i=1; i<4; i++){
		if(!argv[i]){
			puts("Usage: ./dlog <ß> <a> <p>");
			switch(i){
				case 1:	puts("<base> missing"); break;
				case 2:	puts("<exponent> missing"); break;
				case 3:	puts("<prime> missing"); break;
			}
			exit(1);
		}
	}

	// general
	mpz_t g, r, e, a, t;
	int v=1;

	mpz_init(g);
	mpz_init(r);
	mpz_init(e);
	mpz_init(a);
	mpz_init(t);

	mpz_set_ui(g,atoi(argv[3]));
	mpz_set_ui(r,atoi(argv[1]));
	mpz_set_ui(e,atoi(argv[2]));
	mpz_set_ui(a,1);
	mpz_set_ui(t,0);

	do{
		sqm(t,r,a,g);
		if(mpz_cmp(r,e)>0)
			break;
		mpz_add_ui(a,a,v);
		v++;
	}while(mpz_cmp(t,e)!=0);

	mpz_sub_ui(a,a,1);
	gmp_printf("x = %Zd\n", a);

	mpz_clear(g);
	mpz_clear(r);
	mpz_clear(e);
	mpz_clear(a);
	mpz_clear(t);
		
	return 0;
}