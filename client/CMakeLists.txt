# Copyright (c) 2006, 2017, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/mysys_ssl
  ${ZLIB_INCLUDE_DIR}
  ${SSL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/libmysql
  ${CMAKE_SOURCE_DIR}/regex
  ${CMAKE_SOURCE_DIR}/sql
  ${CMAKE_SOURCE_DIR}/strings
  ${MY_READLINE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)


## We will need libeay32.dll and ssleay32.dll when running client executables.
COPY_OPENSSL_DLLS(copy_openssl_client)

ADD_DEFINITIONS(${SSL_DEFINES})
IF(UNIX)
  IF(WITH_MEMCACHED_RECORD)
    MYSQL_ADD_EXECUTABLE(mysql regexp_filter.c set_memcached.c completion_hash.cc mysql.cc readline.cc sql_string.cc)
  ELSE()
    MYSQL_ADD_EXECUTABLE(mysql regexp_filter.c completion_hash.cc mysql.cc readline.cc sql_string.cc)
  ENDIF(WITH_MEMCACHED_RECORD)
ENDIF(UNIX)
TARGET_LINK_LIBRARIES(mysql perconaserverclient)
IF(UNIX)
  IF(WITH_MEMCACHED_RECORD)
    TARGET_LINK_LIBRARIES(mysql ${MY_READLINE_LIBRARY} -lmemcached)
  ELSE()
    TARGET_LINK_LIBRARIES(mysql ${MY_READLINE_LIBRARY})
  ENDIF(WITH_MEMCACHED_RECORD)
ENDIF(UNIX)

MYSQL_ADD_EXECUTABLE(mysqltest mysqltest.cc COMPONENT Test)
SET_SOURCE_FILES_PROPERTIES(mysqltest.cc PROPERTIES COMPILE_FLAGS "-DTHREADS")
TARGET_LINK_LIBRARIES(mysqltest perconaserverclient regex)
ADD_DEPENDENCIES(mysqltest GenError)

MYSQL_ADD_EXECUTABLE(mysqlcheck mysqlcheck.c)
TARGET_LINK_LIBRARIES(mysqlcheck perconaserverclient)

IF(UNIX)
  IF(WITH_MEMCACHED_RECORD)
    MYSQL_ADD_EXECUTABLE(mysqldump set_memcached.c mysqldump.c ../sql-common/my_user.c)
  ELSE()
    MYSQL_ADD_EXECUTABLE(mysqldump mysqldump.c ../sql-common/my_user.c)
  ENDIF(WITH_MEMCACHED_RECORD)
ENDIF(UNIX)
TARGET_LINK_LIBRARIES(mysqldump perconaserverclient)
IF(UNIX)
  IF(WITH_MEMCACHED_RECORD)
    TARGET_LINK_LIBRARIES(mysqldump -lmemcached)
  ELSE()
    TARGET_LINK_LIBRARIES(mysqldump)
  ENDIF(WITH_MEMCACHED_RECORD)
ENDIF(UNIX)

MYSQL_ADD_EXECUTABLE(mysqlimport mysqlimport.c)
TARGET_LINK_LIBRARIES(mysqlimport perconaserverclient)
SET_SOURCE_FILES_PROPERTIES(mysqlimport.c PROPERTIES COMPILE_FLAGS "-DTHREADS")

MYSQL_ADD_EXECUTABLE(mysql_upgrade mysql_upgrade.c)
TARGET_LINK_LIBRARIES(mysql_upgrade perconaserverclient)
ADD_DEPENDENCIES(mysql_upgrade GenFixPrivs)

MYSQL_ADD_EXECUTABLE(mysqlshow mysqlshow.c)
TARGET_LINK_LIBRARIES(mysqlshow perconaserverclient)

MYSQL_ADD_EXECUTABLE(mysql_plugin mysql_plugin.c)
TARGET_LINK_LIBRARIES(mysql_plugin perconaserverclient)

MYSQL_ADD_EXECUTABLE(mysqlbinlog mysqlbinlog.cc sql_string.cc
                     ../sql/rpl_filter.cc ../sql/sql_list.cc)
TARGET_LINK_LIBRARIES(mysqlbinlog perconaserverclient)

MYSQL_ADD_EXECUTABLE(mysqladmin mysqladmin.cc)
TARGET_LINK_LIBRARIES(mysqladmin perconaserverclient)

MYSQL_ADD_EXECUTABLE(mysqlslap mysqlslap.c)
SET_SOURCE_FILES_PROPERTIES(mysqlslap.c PROPERTIES COMPILE_FLAGS "-DTHREADS")
TARGET_LINK_LIBRARIES(mysqlslap perconaserverclient)

MYSQL_ADD_EXECUTABLE(mysql_config_editor mysql_config_editor.cc)
TARGET_LINK_LIBRARIES(mysql_config_editor perconaserverclient)

# "WIN32" also covers 64 bit. "echo" is used in some files below "mysql-test/".
IF(WIN32)
  MYSQL_ADD_EXECUTABLE(echo echo.c)
ENDIF(WIN32)

SET_TARGET_PROPERTIES (
  mysql_plugin 
  mysql_upgrade
  mysqlcheck
  mysqldump
  mysqlimport
  mysqlshow
  mysqlslap
PROPERTIES HAS_CXX TRUE)

ADD_DEFINITIONS(-DHAVE_DLOPEN)

