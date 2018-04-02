
# Percona Server auto manager

`Percona Server auto manager` is a branch of `percona-server-5.6.39-83.1` bringing `memcached record` and `sql filter`.

Documentation: [percona-server-5.6.39-83.1](http://www.percona.com/doc/percona-server/5.6)

*note*: the memcached feature maybe usefull when you want login mysql server with `one-time-one-password` way, such as [google totp](https://highdb.com/%E5%A6%82%E4%BD%95%E5%AE%9E%E7%8E%B0-mysql-%E7%9A%84%E4%B8%80%E6%AC%A1%E4%B8%80%E5%AF%86%E7%99%BB%E5%BD%95/). and the `sql filter` can be helpful when you want restrict the developers only execute allowed sql statement, you can disabled this with `--skip-sql-filter` option.

## Dependency

The memcached option depended on the following package:
```
libmemcached-devel
```

## How to compile

you can compile the source with the same way in [percona-source-install](https://www.percona.com/doc/percona-server/5.6/installation.html#installing-percona-server-from-a-source-tarball), the memcached record feature was off by default, you can disabled this with the following option:
```
cmake . -DWITH_MEMCACHED_RECORD=OFF
```
the sql filter feature was embedded into `client/mysql` with `--sql-fiter` option, default is on, you can disabled this by `--skip-sql-filter` when you connect mysql server.

## How does it work?

### memcached option

We add the store_userpass_mem function to the source code src/client/mysql.cc and src/client/mysqldump.c so that we can send mysql username and password to memcached server, and you can user --help option to see the --memcached-server option when you compile the source code.
```
# /opt/percona5.6.39/bin/mysql --help|grep -P 'memcached|threshold|filter'
  --memcached-server=name 
  --table-threshold=# table size(MB) threshold for invoke pt-osc tool, default
  --sql-filter        whether enable sql filter, default is true(1)
                      (Defaults to on; use --skip-sql-filter to disable.)
memcached-server                  localhost:11211
table-threshold                   200
sql-filter                        TRUE
```
the username and password will be send to memcached server before the mysql or mysqldump really connect to the mysql server.

### sql filter

We add the following rules before sending the actual sql queries to MySQL Server, return immediately if mathed these rules, otherwise send to MySQL Server:
```
1. select statement must have where/limit keywords;
2. update/delete statement must have where keyword;
3. disable 'update/delete ..where..(order by|limit)' syntax;
4. disable 'drop database/drop schema' syntax;
5. disable 'create index' syntax;
6. disable descreased ALTER syntax, this means you can 'add' column, but not 'drop|change|modify|rename' column;
7. disable 'grant all' syntax;
8. disable 'revoke' syntax;
9. disable 'load' syntax;
10. disabled descreased DDL syntax. this means you can not 'purge/truncate/drop' table;
11. disabled 'set ...' syntax, except 'set names ...';
12. disabled if table size is greater than --table-threshold value, default is 200(MB);
```

## How to use?

login mysql server with a right/wrong password:
```
# /opt/percona5.6.39/bin/mysql -h 10.0.21.5 -u arstercz -P 3305 -p --memcached-server "10.0.21.5:11211"
Enter password: 
ERROR 2003 (HY000): Can't connect to MySQL server on '10.0.21.5' (113)
```
read from memcached:
```
# memcached-tool 10.0.21.5:11211 dump
Dumping memcache contents
  Number of buckets: 1
    Number of items  : 1
    Dumping bucket 1 - 1 total items
    add arstercz 0 1520839412 10
    xxxx123456
```

when you connect to mysql server, the sql filter was triggerd automaticly:
```
mysql arstercz@[10.0.21.5:3305 (none)] > alter table checksums add column sss varchar(50);                   

        [WARN] - Must 'use <database>' before alter table, current database is null.

mysql arstercz@[10.0.21.5:3305 (none)] > use percona
Database changed
mysql arstercz@[10.0.21.5:3305 percona] > alter table checksums drop column sss varchar(50);   

        [WARN]
         +-- alter table checksums drop column sss varchar(50)
         Caused by: disable descreased ALTER syntax.
this sql syntax was disabled by administrator

mysql arstercz@[10.0.21.5:3305 percona] > select * from checksums;

        [WARN]
         +-- select * from checksums
         Caused by: no where/limit for select clause
this sql syntax was disabled by administrator

mysql arstercz@[10.0.21.5:3305 percona] > delete from checksums;

        [WARN]
         +-- delete from checksums
         Caused by: no where for delete/update clause
this sql syntax was disabled by administrator

mysql arstercz@[10.0.21.5:3305 percona] > alter table test.user_info add column sss varchar(50);

        [WARN] - the test.user_info size is 4240MB, disallowed by administrator

```
