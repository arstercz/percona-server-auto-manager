#ifndef MYSQL_REGEXP_FILTER_CUS_H_INCLUDED
#define MYSQL_REGEXP_FILTER_CUS_H_INCLUDED

/*
 support sql filter feature to ensure that only
 execute allowed sql.
 arstercz 2018-03-18
*/
#ifdef __cplusplus
extern "C" {
#endif

#define RTRUE  1
#define RFALSE 0
#define ITEM   2
#define LEN    100
#define NUM    2

struct rule_item {
  char *item;
  int flag;
};
typedef struct rule_item RuleItem;

struct rule_set {
  RuleItem rules[ITEM];
  char *comment;
};
typedef struct rule_set RuleSet;

struct match_res {
  int match;
  char *comment;
};
typedef struct match_res MatchRes;

struct meta_info {
  char *db;
  char *table;
};
typedef struct meta_info MetaInfo;

MatchRes regexp_filter_custom(const char *sql);
int regexp_filter_sql(char *pattern, const char *sql);
char *regexp_filter_match(char *pattern, const char *sql);
MetaInfo regexp_get_meta(char *dbname, char *tableName);

#ifdef __cplusplus
}
#endif

#endif /* ! MYSQL_REGEXP_FILTER_CUS_H_INCLUDED */
