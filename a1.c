#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

int PM_parseFrom(ParameterManager * p, FILE *fp, char comment){
    node * param = NULL;
    char * pname = NULL;
    char * pdata = NULL;
    char c;
	int size = 3;
    int i = 0;
	int hashVal = 0;
    c =  fgetc(fp);
	do{
	    if(c==' ' || c=='\n'){
            while(c==' ' || c=='\n'){
                c = fgetc(fp);
            }
        }
        if(c==comment){
            while(c!='\n'){
                c = fgetc(fp);
		        printf("%c\n", c);
            }
	         c = fgetc(fp);
        }
        if(c!=comment && c!=' '){
            if(pname == NULL){
			    pname = malloc(sizeof(char)*size);
            }
            while(c!=' ' && c!='=' && c!='\n' && c!=EOF){
				if(i==size){
					size = i+1;
					pname = realloc(pname, size);	
				}
				pname[i] = c;
				c = fgetc(fp);
                i++;	
            }
            pname[i] = '\0';
        }
        size=3;
        if(c == '='){
            c = fgetc(fp);
            if(pdata==NULL){
			    pdata = malloc(sizeof(char)*size);
            }
            while(c==' ' || c==comment){
                c = fgetc(fp);
                if(c==comment){
                    while(c!='\n'){
                        c = fgetc(fp);
                    }
                    c = fgetc(fp);
                }
            }
            i=0;
            while(c!=';'){
                if(i==size){
					size = i+1;
					pdata = realloc(pdata, size);
				}
				pdata[i] = c;
				c = fgetc(fp);
                i++;
            }
            pdata[i] = '\0';
			if(pdata!=NULL){
				hashVal = hash(p, pname);
                param = p->table[hashVal];
                printf("%d", hashVal);
                if(param!=NULL){
                    while(strcmp(param->key, pname)!=0){
                        if(param->next == NULL && strcmp(param->key, pname)!=0){
                            return(0);
                        }
						param = param->next;
                    }
                }
				printf("%s\n",pname);
				printf("%s\n",pdata);
                switch(param->type){
                    case 0:
                        if(numcheck(pdata)){
                            param->value.int_val = atoi(pdata);
                            i=0;
                            size=3;
                        }
                        else{
                            free(pdata);
                            free(pname);
                            return(0);
                        }
                    break;
                    case 1:
                        if(numcheck(pdata)){
                            param->value.real_val = atof(pdata);
                            i=0;
                            size=3;
                        }
                        else{
                            free(pdata);
                            free(pname);
                            return(0);
                        }
                    break;
                    case 2:
                        if(!strcmp(pdata, "true") || !strcmp(pdata, "false")){
                            if(!strcmp(pdata, "true")){
                                param->value.bool_val = true;
                            }
                            else{
                                param->value.bool_val = false;
                            }
                            i=0;
                            size=3;
                        }
                        else{
                            free(pdata);
                            free(pname);
                            return(0);
                        }
                    break;
                    case 3:
                        if(pdata[0] == '"'){
                            i=0;
                            do{
                                pdata[i] = pdata[i+1];
                                i++;                                
                            }while(pdata[i+1]!='"');
                            pdata[i+1] = '\0';
                            param->value.str_val = malloc(sizeof(char)*(strlen(pdata)));
                            strcpy(param->value.str_val, pdata);
                            i=0;
                            size=3;
                        }
                        else{
                            free(pdata);
                            free(pname);
                            return(0);
                        }
                    break;
                    case 4:
			            if(pdata[0] == '{'){
                            param->value.list_val = createList(pdata);
                            i=0;
                            size=3;
                        }
                        else{
                            free(pdata);
                            free(pname);
                            return(0);
                        }
                    break;
                }
			}
			c = fgetc(fp);
        }
	}while(c!=EOF);
	return(1);
}

/*Constructor for ParameterManager creates a hash table with a size entered by the user*/
ParameterManager * PM_create(int size){
	ParameterManager * hashtable = NULL;
	int i;
	
	if(size<1){
		return NULL;
	}
	if((hashtable=malloc(sizeof(ParameterManager))) == NULL){
		return NULL;
	}
	if((hashtable->table = malloc(sizeof(node*)*size)) == NULL){
	}
	for(i=0; i < size; i++){
		hashtable->table[i]=NULL;
	}
	hashtable->size = size;

	return hashtable;
}

int PM_destroy(ParameterManager * p){
	int i=0;
	while(i<p->size){
		hashRemove(p, i);
	}
    if(p->table != NULL && p!=NULL){
	    free(p->table);
	    free(p);
        return(1);
    }
	return(0);
}

void hashRemove(ParameterManager * hashTable, int i){
	node * curr;
	curr = hashTable->table[i];
	while(curr){
        if(curr->key!=NULL){
		    free(curr->key);
        }
        if(curr->value.str_val!=NULL){
		    free(curr->value.str_val);
        }
		free(curr);
		curr = curr->next;
	}
}

char * PL_next(ParameterList * l){
	return(l->next->data);
}

int PM_hasValue(ParameterManager * P, char * pname){
	node * object;
	int hashSize = hash(P, pname);
	object = P->table[hashSize];
	do{
		printf("%s, %s \n",pname, object->key);
		if(!strcmp(pname, object->key)){	
			if(object->initialized == 1){
				return(1);
			}
			else{
				return(0);
			}
		}
		object = object->next;
	}while(object);
	return(0);
}

int PM_manage(ParameterManager * p, char *pname, param_t ptype, int required){
	node * object;
	int hashSize = hash(p,pname);
    printf("%d\n", hash(p,pname));
	object = p->table[hashSize];
	/*while(object){
		if(strcmp(object->key, pname)){
			return(0);
		}
		object = object->next;
	}*/

	if(!(object=malloc(sizeof(node)))){
		return 0;
	}	
	if(!(object->key=malloc(sizeof(char)*strlen(pname)+1))){
		free(object->key);
		free(object);
		return 0;
	}
	
	object->required=required;
    object->type=ptype;
	strcpy(object->key, pname);
	object->next=p->table[hashSize];
	p->table[hashSize]=object;
	return(1);	
}

/*Hash function used from the following website:*/
int hash(ParameterManager * p, char * key){
	/*unsigned long int hashval;
	int i =0;
	
	while(hashval<ULONG_MAX && key[i]!='\0'){
		hashval = hashval << 8;
		hashval += key[i];
		i++;
	}
	return(hashval%p->size);*/
   int hash = 0;
    int n,i;
    for(i=0; i<strlen(key); i++){
        if(isalpha(key[i]))
            n = key[i]-'a'+1;
        hash = ((hash<<3)+n)%p->size;
    }
    return hash;
}

union param_value PM_getValue(ParameterManager * p, char * pname){
	int hashSize = hash(p, pname);
	node * object;
	object = p->table[hashSize];
	union param_value val;
	do{
		if(!strcmp(pname, object->key)){
			return(object->value);
		}
		object = object->next;
	}while(object);
	return(val);
}

int numcheck(char * string){
    int i = 0;
    while(i<strlen(string)){
        if(!isdigit(string[i]) && (string[i]!='-' || string[i]!='.')){
            return(0);
        }
        i++;
    }
    return(1);
}

ParameterList * createList(char * data){
    char * list;
    int size = 10;
    int i=0;
    int j=0;
    ParameterList * head = NULL;
    while(i<strlen(data)){
        if(data[i] == '"'){
            list = malloc(sizeof(char)*size);
            j=0;
            i++;
            while(data[i]!='"'){
                if(j==size){
                    size = j+1;
                    list = realloc(list, size);
                }
                list[j] = data[i];
                j++;
                i++;
            }
			list[j] = '\0';
            head = createNode(head, list);
            free(list);
        }
        i++;
    }
    return(head);
}

ParameterList * createNode(ParameterList * head, char * data){
    ParameterList * item;
    ParameterList * curr;
    curr = head;
    char * blah;
    printf("l345\n");
	printf("%s\n", data);
    item = malloc(sizeof(ParameterList));
    item->data = malloc(strlen(data)+1);
    printf("l365\n");
    fflush(stdout);
    if(head == NULL){
		strcpy(item->data, data);
		printf("BUTTS\n");
        item->next = NULL;
        printf("BUTTS\n");
        return(item);  
    }
	strcpy(item->data, data);
    while(curr->next!=NULL){
        curr = curr->next;
    }
    curr->next = item;
    item->next = NULL;
    return(head);
}

int main(){
	ParameterManager *params;   /* parameter manager for student record */
    ParameterList *plist;       /* list of marks parameter */
    char *item;                 /* item from list of marks (for iteration) */
    char *name;                 /* student name */
    int id;                     /* student ID */
    Boolean registered;         /* registration status */
    float avg = -1;             /* average (optional parameter) */
    int i;
    /*FILE * fp;*/
    
   /* fp = fopen("test.txt", "r");*/
    if (!(params = PM_create(10)))
    {
        /* ERROR CREATING PARAMETER MANAGER */
        exit(0);
    }

    /*
     * register parameters of interest
     */
    PM_manage(params,"name",STRING_TYPE,1);
    PM_manage(params,"id",INT_TYPE,1);
    PM_manage(params,"regstat",BOOLEAN_TYPE,1);
    PM_manage(params,"marks",LIST_TYPE,1);
    PM_manage(params,"average",REAL_TYPE,0);
    /*
     * parse parameters fromn standard input
     */
    /*printf("%d.\n", params->table[hash(params, "name")]->type);*/
    if (!PM_parseFrom(params,stdin,'#'))
    {
        /* ERROR PARSING PARAMETERS */
        exit(0);
    }

    /*
     * query parameter manager to get parameter values
     */
    name = PM_getValue(params,"name").str_val;

    id = PM_getValue(params,"id").int_val;

    registered = PM_getValue(params,"regstat").bool_val;

    plist = PM_getValue(params,"marks").list_val;

    if (PM_hasValue(params,"average"))
        avg = PM_getValue(params,"average").real_val;

    /*
     * output of parameters
     */
    printf("Parameters:\n");
    printf("name        = %s\n",name);
    printf("id          = %d\n",id);
    printf("registered? = %s\n",registered == true ? "yes" : "no");

    printf("marks       = ");
    /*while(item = PL_next(plist))
        printf("%s ",item);
    printf("\n");*/

    printf("average     = ");
    if (avg >= 0)
        printf("%f\n",avg);
    else
        printf("not entered\n");

    i = PM_destroy(params);
    printf("%d", i);
	return(0);
}

