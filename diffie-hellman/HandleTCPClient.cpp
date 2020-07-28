#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */

#include <stdlib.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <err.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <openssl/evp.h>
#define RCVBUFSIZE 3000 /* Size of receive buffer */
using namespace std;

void *
emalloc (size_t sz)
{
    void *tmp = malloc (sz);

    if (tmp == NULL && sz != 0)
        errx (1, "malloc %lu failed", (unsigned long)sz);
    return tmp;
}

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

void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket, char *ip)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */

    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");
	
    while (recvMsgSize > 0) {
	ENGINE *engine = NULL;
	OpenSSL_add_all_algorithms();
	ENGINE_load_builtin_engines();
	engine = ENGINE_get_default_DH();
	DH *server;
	int port_int;
	char *port_string = echoBuffer;
	string buffer = echoBuffer;
	char *key_buffer = echoBuffer+buffer.find(":", 0)+1;
	port_int=(int)strtoul(port_string, NULL, 10);
	char port[5];
	itoa(port_int, port,10); 
/*	printf("Port = %d\n\n", port_int);
	printf("Port = %s\n\n", port);
	printf("Keys = %s!\n\n", key_buffer);
	printf("Whol = %s!\n\n", echoBuffer); 
*/	 const char *p =
            "FFFFFFFF" "FFFFFFFF" "C90FDAA2" "2168C234" "C4C6628B" "80DC1CD1"
            "29024E08" "8A67CC74" "020BBEA6" "3B139B22" "514A0879" "8E3404DD"
            "EF9519B3" "CD3A431B" "302B0A6D" "F25F1437" "4FE1356D" "6D51C245"
            "E485B576" "625E7EC6" "F44C42E9" "A637ED6B" "0BFF5CB6" "F406B7ED"
            "EE386BFB" "5A899FA5" "AE9F2411" "7C4B1FE6" "49286651" "ECE65381"
            "FFFFFFFF" "FFFFFFFF";
        const char *g = "02";

	server = DH_new_method(engine);


        BN_hex2bn(&server->p, p);
        BN_hex2bn(&server->g, g);

      if (!DH_generate_key(server))
                printf( "DH_generate_key failed for server");


	int size;
	size = strlen(BN_bn2hex(*(&server->pub_key)));

	  /* Echo message back to client */
      if (send(clntSocket, BN_bn2hex(*(&server->pub_key)),size, 0) == 0)
            DieWithError("send() failed");


	BIGNUM *pub_key;
        pub_key = BN_new();
	BN_hex2bn(&pub_key, key_buffer);
      if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
   		DieWithError("Recv() failed");

      	unsigned char *key;
      	key = (unsigned char *)emalloc(DH_size(server));
   	int key_size;
   	key_size = DH_compute_key(key, pub_key, server);
       
        
	string l = "/root/cs6250/zcc/keys/";
	l += ip;
/*	l += ":";
	l += port;*/
	l += ".txt";
	const char *location = l.c_str();
  	ofstream file (location);
  	if (file.is_open()) {
        	for(int i=0; i<key_size; ++i) {
                	file <<  bitset<8>(key[i]) << '\n';
		}
        	file.close();
        }


  }

    close(clntSocket);    /* Close client socket */
}

