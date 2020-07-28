#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/evp.h>
#include <err.h>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>

#define RCVBUFSIZE 3000 /* Size of receive buffer */

#define DEBUG 0

using namespace std;

void DieWithError(char *errorMessage);  /* Error handling function */

void *
emalloc (size_t sz)
{
    void *tmp = malloc (sz);

    if (tmp == NULL && sz != 0)
        errx (1, "malloc %lu failed", (unsigned long)sz);
    return tmp;
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *echoEncryptPort;
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Server Encrypted Port> [<Service Port>]\n",
               argv[0]);
       exit(1);
    }
    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoEncryptPort = argv[2];         /* Second arg: string to echo */
    if (argc == 4)
        echoServPort = atoi(argv[3]); /* Use given port, if any */
    else
        echoServPort = 3333;  /* 7 is the well-known port for the echo service */

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");


	ENGINE *engine = NULL;
	OpenSSL_add_all_algorithms();
        ENGINE_load_builtin_engines();
        engine = ENGINE_get_default_DH();
	DH *client;
         const char *p =
            "FFFFFFFF" "FFFFFFFF" "C90FDAA2" "2168C234" "C4C6628B" "80DC1CD1"
            "29024E08" "8A67CC74" "020BBEA6" "3B139B22" "514A0879" "8E3404DD"
            "EF9519B3" "CD3A431B" "302B0A6D" "F25F1437" "4FE1356D" "6D51C245"
            "E485B576" "625E7EC6" "F44C42E9" "A637ED6B" "0BFF5CB6" "F406B7ED"
            "EE386BFB" "5A899FA5" "AE9F2411" "7C4B1FE6" "49286651" "ECE65381"
            "FFFFFFFF" "FFFFFFFF";
        const char *g = "02";
        client = DH_new_method(engine);
	BN_hex2bn(&client->p, p);
        BN_hex2bn(&client->g, g);
	if (!DH_generate_key(client))
                printf( "DH_generate_key failed for client");

	string message = echoEncryptPort;
	message += ":";
	message += (BN_bn2hex(*(&client->pub_key)));

	if (DEBUG) {
	printf("Client->pub\n%s\n", BN_bn2hex(*(&client->pub_key)));
	}

	char * send_message = (char *)message.c_str();
	int size = message.length();

    /* Send the string to the server */
    if (send(sock, send_message, size, 0) != size)
        DieWithError("send() sent a different number of bytes than expected");
    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    if ((totalBytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0) 
  	DieWithError("recv() failed");

	
	BIGNUM *pub_key;
        pub_key = BN_new();
/* All this crap is because after adding port to the first message
* a random D0 got tacked onto the end of the key on this end
* So we hack it off here
*/
     string key_trim = echoBuffer;
/*     int length = key_trim.length();
     key_trim = key_trim.substr(0, length-2);
     char *new_string = (char *)(malloc(sizeof(char)*(length-2)));
  
*/
   char *new_string = (char *)key_trim.c_str();
	BN_hex2bn(&pub_key, new_string);
	
	if (DEBUG) {
		printf("\n\nServer->pub\n%s\n", BN_bn2hex(pub_key));
	}

   unsigned char *key;
   key = (unsigned char *)emalloc(DH_size(client));
   int key_size;
   key_size = DH_compute_key(key, pub_key, client);
    
  string l = "/root/cs6250/zcc/keys/";
  l += servIP;
/*  l += ":";
 l += echoEncryptPort;*/
  l += ".txt";
  const char *location = l.c_str();
  ofstream file (location);
  if (file.is_open()) {
	for(int i=0; i<key_size; ++i) {
		file <<  bitset<8>(key[i]) << '\n';
	}
	file.close();

  }
  

 close(sock);
    exit(0);
}

