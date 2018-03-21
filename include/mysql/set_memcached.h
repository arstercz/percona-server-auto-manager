#ifndef MYSQL_SET_MEMCACHED_H_INCLUDED
#define MYSQL_SET_MEMCACHED_H_INCLUDED

/*
 Set mysql user and passcode to memcached for the
 wmotp support.
 arstercz 2018-03-13
*/
#ifdef __cplusplus
extern "C" {
#endif

bool store_userpass_mem(char *memcached_server, 
                        char *user, char *password);
#ifdef __cplusplus
}
#endif

#endif /* ! MYSQL_SET_MEMCACHED_H_INCLUDED */
