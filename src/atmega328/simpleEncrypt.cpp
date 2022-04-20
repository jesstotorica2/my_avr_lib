/*

	simpleEncrypt.cpp

*/
#include "simpleEncrypt.h"

void simpleEncryptor::encrypt(uint8_t* data, uint16_t blen, const uint8_t* key, uint16_t key_len){
  uint16_t key_i = 0;
  uint16_t key_mod = 0;
  for( uint16_t i = 0; i<blen ; i++ ){
        key_i = i%key_len;
        /*if( data_valid(data[i]) && data_valid(key[key_i]) ){
          key_mod = (key[key_i] + i%9)%4;
          if(key_mod == 0)
            for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = badd(data[i],key[key_i]);
          else if(key_mod == 1)
            for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = bsub(data[i],key[key_i]);
          else if(key_mod == 2)
            for(uint16_t j = 0; j < 3; j++) data[i] = bsub(data[i],key[key_i]);
          else if(key_mod == 3)
            for(uint16_t j = 0; j < 5; j++) data[i] = badd(data[i],key[key_i]); 
        }*/
				key_mod = (key[key_i] + i%9)%4;
				if(key_mod == 0)
					for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = data[i] + key[key_i];
				else if(key_mod == 1)
					for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = data[i] - key[key_i];
				else if(key_mod == 2)
					for(uint16_t j = 0; j < 3; j++) data[i] = data[i] - key[key_i];
				else if(key_mod == 3)
					for(uint16_t j = 0; j < 5; j++) data[i] = data[i] + key[key_i]; 
  }
}

void simpleEncryptor::decrypt(uint8_t* data, uint16_t blen, const uint8_t* key, uint16_t key_len)
{
  uint16_t key_i = 0;
  uint16_t key_mod = 0;
  for( uint16_t i = 0; i<blen ; i++ )
	{
    key_i = i%key_len;
    /*if( data_valid(data[i]) && data_valid(key[key_i]) ){
        key_i = i%key_len;
        if( data_valid(data[i]) && data_valid(key[key_i]) )
				{
          key_mod = (key[key_i] + i%9)%4;
          if(key_mod == 0)
            for(int j = 0; j < key[key_i]%10; j++) data[i] = bsub(data[i],key[key_i]);
          else if(key_mod == 1)
            for(int j = 0; j < key[key_i]%10; j++) data[i] = badd(data[i],key[key_i]);
          else if(key_mod == 2)
            for(int j = 0; j < 3; j++) data[i] = badd(data[i],key[key_i]);
          else if(key_mod == 3)
            for(int j = 0; j < 5; j++) data[i] = bsub(data[i],key[key_i]);
        }
    }*/
		key_mod = (key[key_i] + i%9)%4;
		if(key_mod == 0)
			for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = data[i] - key[key_i];
		else if(key_mod == 1)
			for(uint16_t j = 0; j < key[key_i]%10; j++) data[i] = data[i] + key[key_i];
		else if(key_mod == 2)
			for(uint16_t j = 0; j < 3; j++) data[i] = data[i] + key[key_i];
		else if(key_mod == 3)
			for(uint16_t j = 0; j < 5; j++) data[i] = data[i] - key[key_i];
  }
}

/*
uint8_t badd(uint8_t x, uint8_t y){
  uint16_t sum = (x + y);
  if( sum > MAX_CVAL ) sum = (sum - MAX_CVAL - 1)%NUM_VALS + MIN_CVAL;
  return sum;
}


uint8_t bsub(uint8_t x, uint8_t y){
  uint16_t sum = x - y;
  if( sum < MIN_CVAL ) sum = MAX_CVAL - ((MIN_CVAL - sum - 1)%NUM_VALS);
  return sum;
}

int data_valid(char x){
  if( x >= MIN_CVAL && x <= MAX_CVAL )
    return -1;
  else
    return 0;
}
*/
