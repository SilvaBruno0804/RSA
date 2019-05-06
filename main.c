#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define ACCURACY 5
#define SINGLE_MAX 10000
#define EXPONENT_MAX 1000
#define BUF_SIZE 1024


int modpow(long long a, long long b, int c) {
	int res = 1;
	while(b > 0) {
		if(b & 1) {
			res = (res * a) % c;
		}
		b = b >> 1;
		a = (a * a) % c;
	}
	return res;
}


int jacobi(int a, int n) {
	int twos, temp;
	int mult = 1;
	while(a > 1 && a != n) {
		a = a % n;
		if(a <= 1 || a == n) break;
		twos = 0;
		while(a % 2 == 0 && ++twos) a /= 2; /* Factor out multiples of 2 */
		if(twos > 0 && twos % 2 == 1) mult *= (n % 8 == 1 || n % 8 == 7) * 2 - 1;
		if(a <= 1 || a == n) break;
		if(n % 4 != 1 && a % 4 != 1) mult *= -1;
		temp = a;
		a = n;
		n = temp;
	}
	if(a == 0) return 0;
	else if(a == 1) return mult;
	else return 0;
}

int solovayPrime(int a, int n) {
	int x = jacobi(a, n);
	if(x == -1) x = n - 1;
	return x != 0 && modpow(a, (n - 1)/2, n) == x;
}


int probablePrime(int n, int k) {
	if(n == 2) return 1;
	else if(n % 2 == 0 || n == 1) return 0;
	while(k-- > 0) {
		if(!solovayPrime(rand() % (n - 2) + 2, n)) return 0;
	}
	return 1;
}


int randPrime(int n) {
	int prime = rand() % n;
	n += n % 2;
	prime += 1 - prime % 2;
	while(1) {
		if(probablePrime(prime, ACCURACY)) return prime;
		prime = (prime + 2) % n;
	}
}


int gcd(int a, int b) {
	int temp;
	while(b != 0) {
		temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

int randExponent(int phi, int n) {
	int e = rand() % n;
	while(1) {
		if(gcd(e, phi) == 1) return e;
		e = (e + 1) % n;
		if(e <= 2) e = 3;
	}
}


int inverse(int n, int modulus) {
	int a = n, b = modulus;
	int x = 0, y = 1, x0 = 1, y0 = 0, q, temp;
	while(b != 0) {
		q = a / b;
		temp = a % b;
		a = b;
		b = temp;
		temp = x; x = x0 - q * x; x0 = temp;
		temp = y; y = y0 - q * y; y0 = temp;
	}
	if(x0 < 0) x0 += modulus;
	return x0;
}


int readFile(FILE* fd, char** buffer, int bytes) {
	int len = 0, cap = BUF_SIZE, r;
	char buf[BUF_SIZE];
	*buffer = malloc(BUF_SIZE * sizeof(char));
	while((r = fread(buf, sizeof(char), BUF_SIZE, fd)) > 0) {
		if(len + r >= cap) {
			cap *= 2;
			*buffer = realloc(*buffer, cap);
		}
		memcpy(&(*buffer)[len], buf, r);
		len += r;
	}

	if(len + bytes - len % bytes > cap) *buffer = realloc(*buffer, len + bytes - len % bytes);
	do {
		(*buffer)[len] = '\0';
		len++;
	}
	while(len % bytes != 0);
	return len;
}

int encode(int m, int e, int n) {
	return modpow(m, e, n);
}


int decode(int c, int d, int n) {
	return modpow(c, d, n);
}


int* encodeMessage(int len, int bytes, char* message, int exponent, int modulus) {
	int *encoded = malloc((len/bytes) * sizeof(int));
	int x, i, j;
	for(i = 0; i < len; i += bytes) {
		x = 0;
		for(j = 0; j < bytes; j++) x += message[i + j] * (1 << (7 * j));
		encoded[i/bytes] = encode(x, exponent, modulus);
#ifndef MEASURE
		//printf("%d ", encoded[i/bytes]);
#endif
	}
	return encoded;
}


int* decodeMessage(int len, int bytes, int* cryptogram, int exponent, int modulus) {
	int *decoded = malloc(len * bytes * sizeof(int));
	int x, i, j;
	for(i = 0; i < len; i++) {
		x = decode(cryptogram[i], exponent, modulus);
		for(j = 0; j < bytes; j++) {
			decoded[i*bytes + j] = (x >> (7 * j)) % 128;
#ifndef MEASURE

#endif
		}
	}
	return decoded;
}

int main(void) {
	int p, q, n, phi, e, d, bytes, len;
	int *encoded, *decoded;
	char *buffer;
	FILE *f;
	FILE *fpTargetFile = fopen("C:\\Users\\bruno\\Pictures\\output.txt", "wb");
	srand(time(NULL));
	while(1) {
		p = randPrime(SINGLE_MAX);
		printf("Primeiro Primo, p = %d ... \n", p);
		//getchar();

		q = randPrime(SINGLE_MAX);
		printf("Segundo primo, q = %d ... \n", q);
		//getchar();

		n = p * q;
		printf("Modulo n = pq = %d ...\n ", n);
		if(n < 128) {
			printf("erro modulo ");
			//getchar();
		}
		else break;
	}
	if(n >> 21) bytes = 3;
	else if(n >> 14) bytes = 2;
	else bytes = 1;
	//getchar();

	phi = (p - 1) * (q - 1);
	printf("Totiente Phi = %d ... \n", phi);
	//getchar();

	e = randExponent(phi, EXPONENT_MAX);
	printf("Expoente = %d\nPublic key is (%d, %d) ... \n", e, e, n);
	//getchar();

	d = inverse(e, phi);
	printf("Expoente privado d = %d\nPrivate key is (%d, %d) ... \n", d, d, n);
	//getchar();

	printf("Opening file \"text.txt\" for reading\n");
	f = fopen("C:\\Users\\bruno\\Pictures\\teste.txt", "rb");
	if(f == NULL) {
		printf("Failed to open file \"text.txt\"\n");
		return EXIT_FAILURE;
	}
	len = readFile(f, &buffer, bytes);
	printf("\n%d\n", len);
	fclose(f);

	encoded = encodeMessage(len, bytes, buffer, e, n);
	printf("\nEncoding finished successfully ... \n");
	//getchar();


	printf("Decoding encoded message ... \n");
	//getchar();
	decoded = decodeMessage(len/bytes, bytes, encoded, d, n);
    //printf("%s", decoded);
	printf("\nFinished RSA demonstration!\n");
     for(int i = 0; i < len; i++){
            fwrite(&decoded[i], 1, 1, fpTargetFile);
     }
     return 0;
	free(encoded);
	free(decoded);
	free(buffer);
	return EXIT_SUCCESS;
}
