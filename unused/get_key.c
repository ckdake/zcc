#include "get_key.h"

static void die_if_fault_occurred (xmlrpc_env* env) {
  if (env->fault_occurred) {
    fprintf(stderr,
            "XML-RPC Fault: %s (%d)\n",
            env->fault_string,
            env->fault_code);
    exit(1);
  }
}

BIGNUM* get_key(const char* host, const char* scheme) {

  xmlrpc_env env;
  xmlrpc_server_info* server;
  xmlrpc_value* result;    
  const char* key;
  int hex2bn_result = 0;
  BIGNUM* retval;

  /* Start up our XML-RPC client library. */
  xmlrpc_client_init(XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION);
  xmlrpc_env_init(&env);

  /* Make a new object to represent our XML-RPC server. */
  server = xmlrpc_server_info_new(&env, SERVER_URL);
  die_if_fault_occurred(&env);

  /* Set up our authentication information. */
  xmlrpc_server_info_set_basic_auth(&env, server, "jrandom", "secret");
  die_if_fault_occurred(&env);

  result = xmlrpc_client_call_server(&env, server, "zcc.getkey", "(ss)", host, scheme);
  die_if_fault_occurred(&env);

  /* Dispose of our server object. */
  xmlrpc_server_info_free(server);
    
  /* Get the information and print it out. */
  xmlrpc_read_string(&env, result, &key);
  die_if_fault_occurred(&env);
    
  /* Dispose of our result value. */
  xmlrpc_DECREF(result);

  /* Shut down our XML-RPC client library. */
  xmlrpc_env_clean(&env);
  xmlrpc_client_cleanup();

  hex2bn_result = BN_hex2bn(&retval, key);

  return retval;

}
