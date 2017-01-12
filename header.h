#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


#ifndef BOOL_DEF
#define BOOL_DEF
typedef int Boolean;
enum {false, true};
#endif


struct ParameterList{
	char * data;
	struct ParameterList * next;
};

typedef struct ParameterList ParameterList;

enum param_s{INT_TYPE, REAL_TYPE, BOOLEAN_TYPE, STRING_TYPE, LIST_TYPE};
typedef enum param_s param_t;

typedef union param_value
{
	int int_val;
	float real_val;
	Boolean bool_val;
	char * str_val;
	struct ParameterList * list_val;
}param_value;

struct node{
	int required;
	int initialized;
	param_t type;
	char * key;
	union param_value value;
	struct node * next;
};

typedef struct node node;

struct ParameterManager{
	int size;
	struct node ** table;
};

typedef struct ParameterManager ParameterManager;


ParameterManager * PM_create(int size);
int PM_destroy(ParameterManager * p);
int PM_parseFrom(ParameterManager * p, FILE *fp, char comment);
int PM_manage(ParameterManager * p, char *pname, param_t ptype, int required);
int PM_hasValue(ParameterManager * P, char * pname);
int hash(ParameterManager * p, char * key);
int numcheck(char * string);
char * PL_next(ParameterList * l);
void hashRemove(ParameterManager * hashTable, int i);
ParameterList * createList(char * data);
ParameterList * createNode(ParameterList * head, char * data);
union param_value PM_getValue(ParameterManager * p, char * pname);

