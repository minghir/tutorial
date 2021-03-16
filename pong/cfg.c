#include <stdio.h>
#include "cfg.h"


int read_var(char* var_name,char *file_name){
	int var;
	char v_name[60];
	FILE *fp = fopen(file_name,"r");

	while(fscanf(fp,"%s %d\n",v_name,&var) != EOF){
		if (strcmp(var_name,v_name) == 0)
			break;
	}
	
	fclose(fp);
	return var;
}
