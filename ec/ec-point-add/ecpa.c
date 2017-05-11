#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RABIN 15	// Miller-Rabin variable for testing if a number is prime
#define OPTIONS "1: Point addition on curve\n0: Quit\n"

void point_add(mpz_t a, mpz_t b, mpz_t p){
	puts("point add func here");


	// point parameters
	mpz_t Px,
		  Py,
		  Qx,
		  Qy;

	mpz_init(Px);
	mpz_init(Py);
	mpz_init(Qx);
	mpz_init(Qy);



	mpz_clear(Px);
	mpz_clear(Py);
	mpz_clear(Qx);
	mpz_clear(Qy);
}

/*check if ec conditions are met 
* 1. if x = 4*a^3 + 27*b^2 mod p is zero
* or 2. if p is not prime the conditions
* are not met */
void condition(mpz_t a, mpz_t b, mpz_t p){
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
	if(!mpz_cmp_ui(x,0)||!mpz_probab_prime_p(p,RABIN)){		// check condition 1 & 2
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


	char *tmp;	// for input
	tmp=calloc(1,sizeof(long));

	mpz_init(a);
	mpz_init(b);
	mpz_init(p);

	// Get curve parameters from user input
	puts("a:");
	fgets(tmp, sizeof(long), stdin);
	mpz_set_ui(a,atoi(tmp));
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage

	puts("b:");	
	fgets(tmp, sizeof(long), stdin);
	mpz_set_ui(b,atoi(tmp));
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage

	puts("p:");
	fgets(tmp, sizeof(long), stdin);
	mpz_set_ui(p,atoi(tmp));		
	memset(tmp, 0, sizeof(long));	// zerofill tmp to avoid garbage
	
	free(tmp);

	condition(a,b,p);	// check if ec conditions are met
	gmp_printf("y^2 = x^3 + %Zd*x + %Zd mod %Zd\n", a,b,p);	// print curve

	short opt=1;	// option trigger
	char *tmp_opt;
	tmp_opt=calloc(1,sizeof(short));

	// menu
	while(opt){
		gmp_printf("%s", OPTIONS);	// print options
		fgets(tmp_opt, sizeof(short), stdin);
		opt=atoi(tmp_opt);
		if(!opt)
			break;
		point_add(a,b,p);
		memset(tmp_opt, 0, sizeof(short));
	}

	// free the used variables
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(p);
	return 0;
}