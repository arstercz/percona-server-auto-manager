/*
** set username and passcode to memcached
** for wmotp support
*/

#include <libmemcached/memcached.h>
#include <string.h>
#include <stdio.h>
#include "mysql.h"
#include <mysql/set_memcached.h>

my_bool store_userpass_mem(char *memcached_server, 
                        char *user, char *password)
{
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;

  if (!(memcached_server && *memcached_server))
    memcached_server = (char *)"localhost:11211";

  if (!(user && *user))
    user = (char *)"unknown";

  if (!(password && *password))
    password = (char *)"";

  memc = memcached_create(NULL);
  servers = memcached_servers_parse(memcached_server);
  rc = memcached_server_push(memc, servers);

  if (rc == MEMCACHED_SUCCESS)
    //fprintf(stderr, "Added server successfully\n");
    ;
  else {
    fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc, rc));
    return false;
  }

  rc = memcached_set(memc, user, strlen(user), password, strlen(password), (time_t)30, (uint32_t)0);

  if (rc == MEMCACHED_SUCCESS)
    //fprintf(stderr, "Key stored successfully\n");
    ;
  else {
    fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc, rc));
    return false;
  }

  memcached_free(memc);
  return true;
}
