// #include "romangol.h"
// #include "liarod.h"
#include "AVX_SM4.h"
#include <time.h>
int main(int argc,char **argv)
{
	// plain: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// key:   01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
	// cipher: 68 1e df 34 d2 06 96 5e 86 b3 e9 4f 53 6e 42 46

	/*
		to achieve avx2 best performance, encrypt 8 blocks at the same time, the data can be devided into 16 blocks each, each block contains 128bit
		gmssl has not fully developed the application of the avx2-encryption process, some of the functions may not work properly.
	*/

	u1 key[SM4_KEY_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
	u1 p[16 * SM4_BLOCK_SIZE] = {
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
		0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
	};

	printf("\n\n-----------------------------------------\nFor each Test, encrypt 256 bytes for 1000000/16 rounds");
	srand(time(NULL));
	for (int i = 0; i < 16 * SM4_BLOCK_SIZE; i++) {
		p[i] = rand() % 0xff;
	}
	u1 *c = p;
	u4 i;								// loop var
	int *pp;
	u4 rkey[SM4_RND_KEY_SIZE / sizeof(u4)];
	SM4_key_schedule(key, rk);		// since the key only has 128bit, there is no need of SIMD
	sms4_avx2_encrypt_init(rk);

	
	int round=1000000;

	clock_t t = clock();
	
	for (int j = 0; j <  round; j++) {
		for (i = 0, pp = (int*)c; i < 256 / (SM4_BLOCK_SIZE * 16); i++, pp += SM4_BLOCK_SIZE * 16 / 4) {
			sms4_avx2_encrypt_16blocks(p, pp, rkey); //	1 block = 128 bit
		}
		for (i = 0, pp = (int*)c; i < 256 / (SM4_BLOCK_SIZE * 16); i++, pp += SM4_BLOCK_SIZE * 16 / 4) {
			sms4_avx2_decrypt_16blocks(p, pp, rkey); //	1 block = 128 bit
		}
		
	}
	clock_t avg = clock() - t;
	double tt = (double)(avg)/(CLOCKS_PER_SEC);
	double speed =(double) (round*16)/(1024*1024*tt);
	printf("time: %f s\nspeed: %f \n",tt,speed);
	
}
