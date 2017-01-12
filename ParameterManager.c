#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ParameterManager.h"

/*Reads from a file stream. If a "comment" is hit, skips to new line. if a spaces is hit, grabs
a new character until it is not a space. once a the cursor hits a noncomment non space character, it puts it in pname.
begins looking for an = sign. once an equals sign is found, the cursor looks for data to read. reads in the data and puts
the data in the appropriate parameter node.*/
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
		/*space and endline case.*/
	    if(c==' ' || c=='\n'){
            while(c==' ' || c=='\n'){
                c = fgetc(fp);
            }
        }
		/*comment case.*/
        if(c==comment){
            while(c!='\n'){
                c = fgetc(fp);
            }
	         c = fgetc(fp);
        }
		/*a name is found.*/
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
		/*an = sign, start looking for data to read.*/
        if(c == '='){
            c = fgetc(fp);
            if(pdata==NULL){
			    pdata = malloc(sizeof(char)*size);
            }
			/*skipping all the garbage*/
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
			/*data found loop until a ;*/
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
				/*gets the appropriate paramater and returns zero if it was not found.*/
                if(param!=NULL){
                    while(strcmp(param->key, pname)!=0){
                        if(param->next == NULL && strcmp(param->key, pname)!=0){
                            return(0);
                        }
						param = param->next;
                    }
                }
				/*checks if parameter was previously intialized*/
                if(param->initialized==1){
                   return(0);
                }
				/*parses the data based on the type entered for that particullar parameter. if the data doesnt match the type, returns 0*/
                switch(param->type){
                    case 0:
                        if(numcheck(pdata)){
                            param->value.int_val = atoi(pdata);
                            param->initialized=1;
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
                            param->initialized=1;
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
                            param->initialized=1;
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
                            pdata[i] = '\0';
                            param->value.str_val = malloc(sizeof(char)*(strlen(pdata)));
                            strcpy(param->value.str_val, pdata);
                            param->initialized=1;
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
                            param->initialized=1;
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
	/*once EOF has been reached, checks to make sure all required params have been initialized.*/
    while(i<p->size){
        if(p->table[i]!=NULL){
            param = p->table[i];
            while(param!=NULL){
                if(param->initialized==0 && param->required==1){
                    return(0);
                }
                param = param->next;
            }
        }
        i++;
    }
    free(pname);
    free(pdata);
	return(1);
}

/*Constructor for ParameterManager creates a hash table with a size entered by the user
  and returns a pointer to the table.*/
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

/*Destroys the parameter list and all data associated with it by iterrating through all spots in the table. 
returns 0 if memory was not able to be free and 1 upon success*/
int PM_destroy(ParameterManager * p){
	int i=0;
	while(i<p->size){
		hashRemove(p, i);
        i++;
	}
    if(p->table != NULL && p!=NULL){
	    free(p->table);
	    free(p);
        return(1);
    }
	return(0);
}

/*takes the hashtable at i and frees the key and either list val or strval if those are initialized. otherwise
it will free the node and continue through the chain in that location*/
void hashRemove(ParameterManager * hashTable, int i){
	node * curr;
    node * temp;
    curr = hashTable->table[i];
	while(curr!=NULL){
        if(curr->key!=NULL){
		    free(curr->key);
        }
        if(curr->type==3){
		    free(curr->value.str_val);
        }
        if(curr->type==4){
            freeList(curr->value.list_val);
        }
        if(curr!=NULL){
            temp = curr;
            curr = curr->next;
		    free(temp);
        }
	}
}

/*Returns the next key in a ParamList*/
char * PL_next(ParameterList * l){
    if(l->next==NULL){
        return(NULL);
    }
    *l = *(l->next);
    return(l->data);
}

/*Checks if the initialized flag has been set, if so returns one else returns 0.*/
int PM_hasValue(ParameterManager * P, char * pname){
	node * object;
	int hashSize = hash(P, pname);
	object = P->table[hashSize];
	do{
		if(strcmp(pname, object->key)==0){	
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

/*Takes the hashtable, paramter name, type and a required as arguments. hashes pname and checks if a parameter exists with that name already.
if the name was found returns 0. Trys to malloc memory for the node if memory cannot be malloced returns 0. sets the required, and key of the node and adds it
to the front of the chain in the table if there is a collision at the hash location. returns one upon success*/
int PM_manage(ParameterManager * p, char *pname, param_t ptype, int required){
	node * object;
	int hashSize = hash(p,pname);
	object = p->table[hashSize];
	while(object){
		if(strcmp(object->key, pname)){
			return(0);
		}
		object = object->next;
	}

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

/*Hash function idea from the following website: http://stackoverflow.com/questions/7666509/hash-function-for-string
Takes in the key and outputs a number assosiated with the key to place it in the hash table.*/
int hash(ParameterManager * p, char * key){
   int hash = 0;
    int n,i;
    for(i=0; i<strlen(key); i++){
        if(isalpha(key[i]))
            n = key[i]-'a'+1;
        hash = ((hash<<3)+n)%p->size;
    }
    return hash;
}

/*hashes pname and looks in the hashtable for that particular value. if it is found it returns the union val. if it is not found
it returns an empty union value.*/
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

/*Checks to make sure the float or int data entered is a number, by making sure each character is
a number character.*/
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

/*Creates a list and returns the head of the list. Scans through data received
until a quote is found and grabs the string in the middle until an end quote is found. once found,
sends the data to create a node add it to the back of the list and return the head.*/
ParameterList * createList(char * data){
    char * list;
    int size = 3;
    int i=0;
    int j=0;
    ParameterList * head = NULL;
    ParameterList * temp = NULL;
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
			size = 3;
        }
        i++;
    }
    temp = head;
    head = NULL;
    head = malloc(sizeof(ParameterList));
    head->next = temp;
	free(list);
    return(head);
}

/*Takes in the head of the list and string data as arguments. If the head is null,
initializes the head and returns it. otherwise initializes a new nod and adds it to the
back of the list.*/
ParameterList * createNode(ParameterList * head, char * data){
    ParameterList * item;
    ParameterList * curr;
    curr = head;
    item = malloc(sizeof(ParameterList));
    item->data = malloc(strlen(data)+1);
    if(head == NULL){
		strcpy(item->data, data);;
        item->next = NULL;
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

/*Goes through each node of a list starting at the head and frees all necessary data until
there are no more items in the list.*/
void freeList(ParameterList* head){
    ParameterList * temp;
    ParameterList * del;
    temp = head;
    while(temp!=NULL){
        del = temp;
        temp = temp->next;
        if(del->data!=NULL){
            free(del->data);
        }
        free(del);
    }
    head = NULL;
}


