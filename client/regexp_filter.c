/*
** disallow unsafe sql statement by match the 
** specified rules.
** arstercz 20180320
*/

#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/regexp_filter_custom.h>

static RuleSet rule[] = {
  {
    {
      {(char*)"^SELECT\\s+", RTRUE}, 
      {(char*)"(WHERE|LIMIT)\\s+", RFALSE},
    },
    (char*)"no where/limit for select clause",
  },
  {
    {
      {(char*)"(^DELETE|^UPDATE)\\s+", RTRUE},
      {(char*)"WHERE\\s+", RFALSE},
    },
    (char*)"no where for delete/update clause",
  },
  {
    {
      {(char*)"(^DELETE|^UPDATE)\\s+", RTRUE},
      {(char*)"WHERE.*(ORDER\\s+BY|LIMIT)", RTRUE},
    },
    (char*)"order by or limit is unsafe for delete/update with where clause",
  },
  {
    {
      {(char*)"^CREATE\\s+INDEX\\s+", RTRUE},
    },
    (char*)"disabled create index syntax.",
  },
  {
    {
      {(char*)"^ALTER\\s+.*(DROP|CHANGE|MODIFY|RENAME)\\s+", RTRUE},
    },
    (char*)"disable descreased ALTER syntax."
  },
  /*{
    {
      {(char*)"(^CREATE\\s+DATABASE|^CREATE SCHEMA|^DROP\\s+DATABASE|^DROP\\s+SCHEMA)\\s+", RTRUE},
    },
    (char*)"disabled create/drop database syntax.",
  },*/
  {
    {
      {(char*)"(^DROP\\s+DATABASE|^DROP\\s+SCHEMA)\\s+", RTRUE},
    },
    (char*)"disabled drop database syntax.",
  },
  {
    {
      {(char*)"(^PURGE|TRUNCATE|^DROP\\s+TABLE)\\s+", RTRUE},
    },
    (char*)"disabled descreased DDL syntax.",
  },
  {
    {
      {(char*)"^REVOKE\\s+", RTRUE},
    },
    (char*)"disabled revoke syntax.",
  },
  {
    {
      {(char*)"^GRANT\\s+ALL\\s+", RTRUE},
    },
    (char*)"disabled grant all syntax.",
  },
  {
    {
      {(char*)"^LOAD\\s+", RTRUE},
    },
    (char*)"disabled load syntax.",
  },
  {
    {
      {(char*)"^SET\\s+", RTRUE},
      {(char*)"\\sNAMES\\s+", RFALSE},
    },
    (char*)"disabled set ... syntax, except names.",
  },
};

MatchRes regexp_filter_custom(const char *sql)
{
  int match = 1;
  int i, j, nums;
  MatchRes match_response = {0, (char *)""};

  nums = sizeof(rule)/sizeof(rule[0]);
  for (i = 0; i < nums; i++) {
    int rule_size = sizeof(rule[i].rules)/sizeof((rule[i].rules)[0]);
    match = 1;
    for (j = 0; j < rule_size; j++) {
      char *list = rule[i].rules[j].item;
      int flag   = rule[i].rules[j].flag;
      if (list != NULL) 
      {
        int tag = regexp_filter_sql(list, sql);
        if (flag == RFALSE) {
          tag = ~tag;
        }
        match &= tag;
      }
    }
    if (match == 1)
    {
      match_response.match = match;
      match_response.comment = rule[i].comment;
      break;
    }
  }
  return match_response;
}

int regexp_filter_sql(char *pattern, const char *sql)
{
  int match = 0;
  regex_t regex;
  int regexinit = regcomp(&regex, pattern,
                          REG_EXTENDED | REG_ICASE | REG_NEWLINE);

  if (regexinit) {
    fprintf(stderr, "init regex for filter sql error\n");
  }
  else {
    int ret = regexec(&regex, sql, 0, NULL, 0);
    if ( REG_NOERROR != ret ) {
      ;
    }
    else {
      match = 1;
    }
  }
  regfree(&regex);
  return match;
}

char *regexp_filter_match(char *pattern, const char *sql)
{
  regmatch_t pmatch[NUM];
  regex_t regex;
  int regexinit;

  char *str_match;
  str_match = malloc(LEN + 1); 
  if (str_match == NULL)
    return NULL;

  regexinit = regcomp(&regex, pattern,
                      REG_EXTENDED | REG_ICASE | REG_NEWLINE);

  if (regexinit) {
    //fprintf(stderr, "init regex for filter sql error\n");
  }
  else {
    int ret = regexec(&regex, sql, NUM, pmatch, 0); 
    if ( REG_NOERROR != ret ) { 
      //fprintf(stderr, "metch failed");
      str_match[0] = '\0';
    }   
    else {
      int len = pmatch[1].rm_eo - pmatch[1].rm_so;
      if (len) {
        memcpy(str_match, sql + pmatch[1].rm_so, len);
      }
      str_match[len] = '\0';
    }   
  }
  regfree(&regex);
  return str_match;
}

MetaInfo regexp_get_meta(char *dbname, char *tableName)
{
    char *meta[2];
    char *listItem;
    int i = 0;
    MetaInfo metaitem;
    metaitem.db = dbname;
    metaitem.table = strdup(tableName);

    if (regexp_filter_sql((char *)"\\.", metaitem.table)) {
      while ((listItem = strsep(&metaitem.table, ".")) && i < 2)
      {
        meta[i] = listItem;
        i++;
      }
      if (meta != NULL)
      {
        metaitem.db = meta[0];
        metaitem.table = meta[1];
      }
    }
    return metaitem;
}
