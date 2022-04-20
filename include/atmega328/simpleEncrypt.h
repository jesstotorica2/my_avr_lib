/*

	simpleEncrypt.h


	Simple byte by byte encryption. Fast and lightweight. More complex than Caesar
	encryption but much less complex than any legitimate encryption scheme (RSA, AES, etc.)
	Should work fine for sending data over LAN, capitalize on the fact that listener doesnt know
	encryption technique. Decryption would be somewhat trivial if they did. Shouldn't use any sensitive 
	passwords for encrypting as it may be easy to back calculate these.
*/

#ifndef ENCRYPT_H
#define ENCRYPT_H
  
#include "mystdlib.h"

/*#define MAX_CVAL 255
#define MIN_CVAL 0
#define NUM_VALS (MAX_CVAL - MIN_CVAL + 1)
*/


class simpleEncryptor
{

	public:
		//simpleEncryptor(){}
		//~simpleEncryptor(){}
		void encrypt(uint8_t* data, uint16_t blen, const uint8_t* key, uint16_t key_len);
		void decrypt(uint8_t* data, uint16_t blen, const uint8_t* key, uint16_t key_len);

	//private:
		/*
		char badd(char x, char y);
		char bsub(char x, char y);
		int data_valid(char x);
		*/
};
#endif
