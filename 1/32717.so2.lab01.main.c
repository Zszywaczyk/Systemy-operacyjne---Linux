//Patryk Chowratowicz

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/types.h>

extern char** environ;

int main(int argc, char **argv){

int ret;
int aflag=0;
int bflag=0;
char cvalue[30]="";

	while((ret = getopt(argc, argv, "gb:")) != -1){
	
		switch(ret){
			case 'g':
				aflag =1;
				break;
			case 'b':
				bflag=1;
				strcpy(cvalue, optarg);
				//printf("%s\n",optarg); //
				break;
			case '?':
			  	if (optopt == 'b')
					fprintf (stderr, "Option -%c requires an argument.\n",optopt);
				else
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
					return 1;
		default: abort ();

		}	

	}
//---------------------------------------------------------------------------------------------------
struct group *grp;
int j;
int ngroups=0; 
gid_t *groups;
struct passwd *pwd;
char tt[30]="/bin/";
strcat(tt, cvalue);
//groups = malloc(ngroups * sizeof (gid_t));

if(aflag==0&&bflag==0){
	while((pwd=getpwent())!=NULL){
		printf("L: ");
		printf("%s\n", pwd->pw_name);
	}
}


if(aflag==1&&bflag==0){
	while((pwd=getpwent())!=NULL){
		printf("L: ");
		printf("%s\n", pwd->pw_name);
		ngroups=0;
		if(getgrouplist(pwd->pw_name, pwd->pw_gid, groups, &ngroups)==-1){
				groups = malloc(ngroups * sizeof (gid_t));
				getgrouplist(pwd->pw_name, pwd->pw_gid, groups, &ngroups);
		}
		for (j = 0; j < ngroups; j++) {
			grp = getgrgid(groups[j]);
			if (grp != NULL){
				printf("\t\t|%s\n", grp->gr_name);
			}
		}	
	free(groups);
	}
}
if(aflag==0&&bflag==1){
	while((pwd=getpwent())!=NULL){
//printf("%s\t%s\n",pwd->pw_shell,tt);
		if(strcmp(tt, pwd->pw_shell)==0){
			//printf("%s\n", tt);
			//printf("%s\n", cvalue);
			printf("L: ");
			printf("%s\t%s\n", pwd->pw_name,pwd->pw_shell);
		}	
	}
}
if(aflag==1&&bflag==1){
	while((pwd=getpwent())!=NULL){
		if(strcmp(tt, pwd->pw_shell)==0){
			//printf("%s\n", tt);
			//printf("%s\n", cvalue);
			printf("L: ");
			printf("%s\t%s\n", pwd->pw_name,pwd->pw_shell);
			ngroups=0;	
			if(getgrouplist(pwd->pw_name, pwd->pw_gid, groups, &ngroups)==-1){
				groups = malloc(ngroups * sizeof (gid_t));
				getgrouplist(pwd->pw_name, pwd->pw_gid, groups, &ngroups);
			}
			for (j = 0; j < ngroups; j++) {
				grp = getgrgid(groups[j]);
				if (grp != NULL){
					printf("\t\t|%s\n", grp->gr_name);
				}
			}
	free(groups);
		}	
	}
}

return 0;

}
