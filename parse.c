#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int PM_parseFrom(FILE * fp, char comment){
	char pname[1025];
    char pdata[1025];
    char c;
    int i;
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
        if(c!=comment && c!=' ' && c!=EOF){
            while(c!=' ' && c!='='){
                pname[i] = c;
				c = fgetc(fp);
                i++;	
            }
			printf("%s\n", pname);    
        }
        i=0;
        if(c == '='){
            c = fgetc(fp);
            while(c==' ' || c=='#'){
                c = fgetc(fp);
                if(c=='#'){
                    while(c!='\n'){
                        c = fgetc(fp);
                    }
                }
            }
            while(c!=';'){
                pdata[i]=c;
				printf("%c\n", c);
				c = fgetc(fp);
                i++;
            }
			c = fgetc(fp);
        }
        i=0;
       	strcpy(pname, "\0");
		strcpy(pdata, "\0");
	}while(c!=EOF);
	return(1);
}

int main(){
	FILE * fp;
	int i=0;
	fp = fopen("test.txt", "r");
	i=PM_parseFrom(fp, '#');
	return(0);
}
