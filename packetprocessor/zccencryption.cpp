#include <linux/netfilter.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/netfilter_ipv4.h>
extern "C" {
	#include "libipq/libipq.h"
}

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <openssl/aes.h>
#include <sstream>
#include <iomanip>

#include "zccencryption.h"


#define DEBUG 0

using namespace std;

#define BUFSIZE 2048

typedef unsigned char u8;
typedef const u8 cu8;

AES_KEY aes_key;

/**
 * One encryption method
*/
struct enc_results_t *one_encrypt(const char* in, unsigned char* out, size_t len) {
	enc_results_t* enc_results = new enc_results_t;
	size_t i = 0;
	for (i = 0; i < len; i++) {
		out[i]=(unsigned char)in[i]+1;
	}
	if (DEBUG) {
		printf("The encrypted payload is %s\n", out);
	}
	enc_results->results=out;
	enc_results->size = len;
	return enc_results;
}

/**
 * One decryption method
*/
struct enc_results_t *one_decrypt(const char* in, unsigned char* out, size_t len) {
	enc_results_t* dec_results = new enc_results_t;
	size_t i = 0;
	for (i = 0; i < len; i++) {
		out[i]=(unsigned char)in[i]-1;
	}
	if (DEBUG) {
		printf("The decrypted payload is %s\n", out);
	}
	dec_results->results=out;
	dec_results->size = len;
	return dec_results;
}


/**
 * AES encryption method
*/
struct enc_results_t * aes_encrypt(const char* in, unsigned char* out, size_t len, AES_KEY *key) {
	enc_results_t* enc_results = new enc_results_t;
	// if (len % 16) exit(1); // not block size multiple
	size_t i = 0;
      	for (i = 0; i < len; i += 16) {
		AES_encrypt((cu8*)in + i, (u8*)out + i, key);
	}
	if (DEBUG) {
	printf("The new size is %d\n", i);
	}
	enc_results->results = out;
	printf("The result is %s\n", out);
	enc_results->size = i;
	return enc_results;
}

/** 
 * AES decryption method
 */
struct enc_results_t * aes_decrypt(const char* in, unsigned char* out, size_t len, AES_KEY *key) {
	enc_results_t* enc_results = new enc_results_t;
	// if (len % 16) exit(1); // not block size multiple
	size_t i = 0;
      	for (i = 0; i < len; i += 16) {
		AES_decrypt((cu8*)in + i, (u8*)out + i, key);
	}
	if (DEBUG) {
		printf("The new size is %d\n", i);
	}
	enc_results->results = out;
	enc_results->size = i;
	return enc_results;
}
  
/**
 * Used to load AES key from the file
*/    
unsigned char ActualValue(const char *p_input) {
	unsigned char value = 0;

	if (p_input[0] == '1') value += 128;
	if (p_input[1] == '1') value += 64;
	if (p_input[2] == '1') value += 32;
	if (p_input[3] == '1') value += 16;
	if (p_input[4] == '1') value += 8;	
	if (p_input[5] == '1') value += 4;
	if (p_input[6] == '1') value += 2;
	if (p_input[7] == '1') value += 1;

	return value;
}

/**
 * Needed for itoa definition
*/
char *strrev(char *str) {
	char *p1, *p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}

	return str;
}

/**
 * Used to get port from TCP header
*/
char *itoa(int n, char *s, int b) {
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	int i=0, sign;    
	if ((sign = n) < 0)
		n = -n;

	do {
		s[i++] = digits[n % b];
	} while ((n /= b) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	return strrev(s);
}

/**
 * Loads a key into the key structure.
 * Return 1 on success, 0 on failure
 * Params
 *  - des_addr - The IP address we want to talk to
 *  - des_port - The port we want to talk to
*/
int zcc_get_key(unsigned char des_addr[4], int des_port) {
	string l = "/root/cs6250/zcc/keys/";
	string ip;
	char *key = NULL;
	char port[5];
	char des[5];
	string key_temp_string, line="";
	int key_length = 0;
	
	itoa(des_port, port, 10);
	itoa((int)des_addr[0], des, 10);
	ip += des;
	ip += ".";
	itoa((int)des_addr[1], des, 10);
	ip += des;
	ip += ".";
	itoa((int)des_addr[2], des, 10);
	ip += des;
	ip += ".";
	itoa((int)des_addr[3], des, 10);
	ip += des;
	
	l += ip;
	l += ".txt";

  	const char *location = l.c_str();
  	ifstream file (location);
	int count = 0;
	if (file.is_open()) {
		while (!file.eof()) {
			getline(file, line);
			if (count > 0) {		
				char c = ActualValue(line.c_str());
				key_temp_string += c;
			}
			count++;
		}

		key = (char *)key_temp_string.c_str();
		key_length = key_temp_string.length();
		file.close();
  	} else {
		string command = "/root/cs6250/zcc/diffie-hellman/DHClient ";
		command += ip.c_str();
		command += " ";
		command += port;
		command += " 3333";
		system(command.c_str());
	
	  	ifstream file (location);
  		if (file.is_open()) {
	                while (!file.eof()) {
        	                getline(file, line);
               		         if (count > 0) {
                	                char c = ActualValue(line.c_str());
                       		         key_temp_string += c;
                       		 }
                       		 count++;
                	}

                	key = (char *)key_temp_string.c_str();
                	key_length = key_temp_string.length();
                	file.close();
		} else {
			printf("Could not create key\n");
			return 0;
		}
	}

	unsigned char keybuf[128];
	memset(keybuf, 0, 128);
	memcpy(keybuf, key, (key_length > 128 ? 128 : key_length));

	if (0 != AES_set_encrypt_key(keybuf, 128, &aes_key)) {
		printf("Error setting new key\n");
		return 0;
	}
	
	return 1;
}



/**
 * The call to encrypt a packet
*/
extern "C" struct enc_results_t * zcc_encrypt (int total_len, void *ip_checksum_ptr, unsigned char dst_addr[4], int src_port, int dst_port, void *tcp_checksum_ptr, void *payload_ptr, int unsigned payload_length ) {

	enc_results_t* enc_results = NULL;

	/* Cast any of the pointers we are passed */
	__u16 *ip_checksum = (__u16 *)ip_checksum_ptr;
	__u16 *tcp_checksum = (__u16 *)tcp_checksum_ptr;
	unsigned char *payload = (unsigned char *)payload_ptr;

	if (DEBUG) {
	/* Print out the data passed to us */
	printf("\n---------------\n");
	printf("Total Length: %d\n", total_len);
	printf("IP Checksum : %lu\n", (unsigned long)ip_checksum);
	printf("Dest Address: %d.%d.%d.%d\n", dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);
	printf("Destin Port : %d\n", dst_port);
	printf("Source Port : %d\n", src_port);
	printf("TCP Checksum: %lu\n", (unsigned long)tcp_checksum);
	printf("Payload Len : %u\n", payload_length);
	printf("Payload:\n%s\n", payload);
	printf("---------------\n");
	}
	if (!zcc_get_key(dst_addr, dst_port)) {
		fprintf(stderr, "We could not get a key.  Aborting encryption\n");
		return 0;
	}

	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char)*2000);
	bzero(buf, 2000);
	memcpy(buf, payload, payload_length);
	
/*
	//Uncomment this to use AES
	enc_results = aes_encrypt((const char *)payload, buf, payload_length, &aes_key);
*/

	//Uncomment this to use One
	enc_results = one_encrypt((const char *)payload, buf, payload_length);

	if (DEBUG) {
		printf("Encrypted Payload:\n%s\n\n", enc_results->results);
	}
	return enc_results;
}



extern "C" struct enc_results_t * zcc_decrypt (int total_len, void *ip_checksum_ptr, unsigned char dst_addr[4], int src_port, int dst_port, void *tcp_checksum_ptr, void *payload_ptr, int unsigned payload_length ) {

	enc_results_t* dec_results = NULL;

	/* Cast any of the pointers we are passed */
	__u16 *ip_checksum = (__u16 *)ip_checksum_ptr;
	__u16 *tcp_checksum = (__u16 *)tcp_checksum_ptr;
	unsigned char *payload = (unsigned char *)payload_ptr;
	if (DEBUG) {
	/* Print out the data passed to us */
	printf("\n---------------\n");
	printf("Total Length: %d\n", total_len);
	printf("IP Checksum : %lu\n", (unsigned long)ip_checksum);
	printf("Dest Address: %d.%d.%d.%d\n", dst_addr[0], dst_addr[1], dst_addr[2], dst_addr[3]);
	printf("Destin Port : %d\n", dst_port);
	printf("Source Port : %d\n", src_port);
	printf("TCP Checksum: %lu\n", (unsigned long)tcp_checksum);
	printf("Payload Len : %u\n", payload_length);
	printf("Payload:\n%s\n", payload);
	printf("---------------\n");
	}
	if (!zcc_get_key(dst_addr, dst_port)) {
		fprintf(stderr, "We could not get a key.  Aborting encryption\n");
		return 0;
	}

	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char)*2000);
	bzero(buf, 2000);
	memcpy(buf, payload, payload_length);

/*
	//Uncomment this to use AES
	dec_results = aes_decrypt((const char *)payload, buf, payload_length, &aes_key);
*/

	//Uncomment this to use One
	dec_results = one_decrypt((const char *)payload, buf, payload_length);

	if (DEBUG) {
		printf("Decrypted Payload:\n%s\n\n", buf);
	}
	return dec_results;
}
