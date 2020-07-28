#include <stdlib.h>
#include <stdio.h>

#define NAME       "ZCC XML-RPC Client"
#define VERSION    "1.0"
#define SERVER_URL "http://www.ithought.org/admin_server/KeyService.php"

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <openssl/dh.h>
#include <openssl/bn.h>

static void die_if_fault_occurred (xmlrpc_env* env);

BIGNUM* getkey(const char* host, const char* scheme);

