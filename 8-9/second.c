//kompilowac z -lcrypt
#define _GNU_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
void exit(int status);

int maxx=0;
int count=0;
int procent=0;
int ile;
typedef struct _Arg{
	int nr;
	char **slowa;
	int *_od;
	int *_do;
	char *cry;
	char *conf;
	char *haslo;
	char* path;
	char* salt;
} Argument;




void fromto(int lwierszy, int th, int **_od,int **_do){//obliczenie od kad do kad ma sprawdzac hasla
	int podzial = lwierszy/th;
	int i;
	*(*_od+0)=1;
	*_do[0]=podzial;
	//printf("%d  -  %d\n", *_od[0], *_do[0]);
	for(i=1;i<th;i++){
		*(*_od+i)=*(*_od+i-1)+podzial;
		*(*_do+i)=*(*_do+i-1)+podzial;
		//printf("%d  -  %d\n", *(*_od+i), *(*_do+i));
		if(*(*_do+th-1)<lwierszy){
			*(*_do+th-1)+=1;
		}
	}
}
void* watki(void* _arg){
	
	
	//--------
	Argument* arg = (Argument*)_arg;
	char **sll=arg->slowa;
	int nr=arg->nr;
	int *_od=arg->_od;
	int *_do=arg->_do;
	char *cry=arg->cry;
	char *salt=arg->salt;
	//--------

	//--------
	printf("Proces %d : utworzony\n", arg->nr);
	int i;
	//
	struct crypt_data data;
	data.initialized=0;
	//

	for(i=_od[nr]-1;i<_do[nr];i++){
		//printf("%s\n", sll[i]);
		pthread_mutex_lock( &mutex1 );
		count++;
		if(count%ile==0){
			procent=(count*100)/maxx;
			system("clear");
			printf("%s\n",cry);	//tutaj tutaj
			printf("Rozpoczynam skanowanie...\n");
			printf("Postep: %d%%\n",procent);		
		}
		pthread_mutex_unlock( &mutex1 );
		//printf("\n\n%s\n\n%s",crypt_r(sll[i],salt,&data),cry);	//tutaj tutaj
		if(strcmp(cry,crypt_r(sll[i],salt,&data))==0){
			

			system("clear");
			printf("Skanowanie ukonczone!\n");
			printf("Postep: 100%%\n");
			//printf("Slowo to %s\n",sll[i]);
			//printf("Haslo znalezione! <-------\n");
			pthread_mutex_lock( &mutex1 );
			arg->haslo=sll[i];
			pthread_mutex_unlock( &mutex1 );
			printf("Poszukiwane haslo to: %s\n",arg->haslo);
			exit(1);
		}
			//if(i%1000==0){
			//printf("Proces%d %d\n",nr,i);
			//}		
		//}
	}
	//printf("Proces %d : koniec\n", nr);
	


}
void* watkii(void* _arg){
	clockid_t cid;
	struct timespec ts;

	Argument* arg = (Argument*)_arg;
	char **sll=arg->slowa;
	int nr=arg->nr;
	int *_od=arg->_od;
	int *_do=arg->_do;
	char *cry=arg->cry;
	char *salt=arg->salt;
	//--------
	//printf("Proces %d : utworzony\n", arg->nr);
	int i;
	//
	struct crypt_data data;
	data.initialized=0;
	//

	for(i=_od[nr]-1;i<_do[nr];i++){
		//printf("%s\n", sll[i]);
		if(strcmp(cry,crypt_r(sll[i],salt,&data))==0){
		}
	}
	pthread_getcpuclockid(pthread_self(), &cid);
	clock_gettime(cid, &ts);
	printf("\t%ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);

}
void dla_clock(int th, char **argv, int ograniczenie){

		struct timeval t1, t2;
    		double elapsedTime;
		
		char *cry= argv[1];
		char *path= argv[2]; //sciezka do slownika
		char *salt= "$6$5MfvmFOaDU$";
		//-------zmienne
		struct stat sb;
		off_t len;
		char *p;
		int fd;
		char c[30];
		int i;
		int j;
		int *_od=malloc(sizeof(int)*th);
		int *_do=malloc(sizeof(int)*th);
		int podzial=0;
		int enter=0;
		
		//-------Dane z dokumentu do char array
		//(p[23] to pojedynczy znak)
		//fstat wyciąga statystyki z pliku. sb.st_size - ile znaków znajduje się w pliku (fd);
		//-------
		fd = open(path, O_RDONLY);
		fstat(fd, &sb);
		p= mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
		close(fd);
	
		//------Zliczenie wierszy w pliku 
		for(i=0;i<sb.st_size;i++){//zwraca liczbe wierszy;
			if(p[i]=='\n'){
				enter++;
			}
		}
		if(enter<ograniczenie){
			printf("Plik jest za maly i nie posiada %d slow\n", ograniczenie);
			exit(1);
		}

		//------Double array na cale slowa slowa[rows][columns]
		//slowa[0] zwruci całe slowo, slowa[0][12] zwruci 13 litere w slowie;
		char **slowa=(char**) calloc(ograniczenie, sizeof(char*));
		for(i=0;i<ograniczenie;i++){
			slowa[i]= (char*) calloc(30, sizeof(char));
		}
		//Tu dzielimy znaki z p[] na cale słowa do double array;
		int cc=0;
		j=0;
		for(i=0;i<ograniczenie;i++){
			while(p[cc]!='\n'){
				slowa[i][j]=p[cc];
				cc++;
				j++;
			}
			slowa[i][j+1]='\0';
			j=0;
			cc++;
		}
	//printf("%d\n",enter);

		//------Dzielimy na watki od kad do kad;
		fromto(ograniczenie, th, &_od, &_do);
			//printf("%d - %d\n", _od[0], _do[0]);
			//printf("%d - %d\n", _od[1], _do[1]);
			//printf("%d - %d\n", _od[2], _do[2]);
			//printf("%d - %d\n", _od[3], _do[3]);

		//------Tworzenie struktury dla watkow
		Argument *arg=malloc(sizeof(Argument)*th);
		for(i=0;i<th;i++){
			arg[i]._od=_od;
			arg[i]._do=_do;
			arg[i].slowa=slowa;
			arg[i].cry=cry;
			arg[i].path=path;
			arg[i].nr=i;
			arg[i].salt=salt;
			arg[i].conf=cry;
			arg[i].haslo=NULL;
		}

		gettimeofday(&t1, NULL);
		//------Tworzenie watkow
		pthread_t *tread;
		tread=(pthread_t *)malloc(sizeof(pthread_t)*th);
		printf("Ilosc watkow: %d\n",th);
		printf("Czasy:\n");
		for(i=0;i<th;i++){
			//printf("%d", arg[i].nr);
			pthread_create(&tread[i],NULL,watkii,&arg[i]);
			//pthread_getcpuclockid(tread[i], &cid);
			//clock_gettime(cid, &ts);
			//printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
		}


		for(i=0;i<th;i++){
			pthread_join(tread[i],NULL);
		}
		gettimeofday(&t2, NULL);
	//pthread_getcpuclockid(pthread_self(), &cid);
		elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;	
		elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
		elapsedTime=elapsedTime/1000;
		printf("Calkowity czas trwania: %0.3f\n",elapsedTime);
		printf("---------------------------------\n");

}

int main(int argc, char **argv){
	if(argv[3]==NULL){
		int i;
		int watki_online=sysconf(_SC_NPROCESSORS_ONLN);
		int ograniczenie=10000;
		printf("Sprawdzanie optymalej liczby watkow\n");
		int *th=malloc(watki_online*sizeof(int));
		for(i=1;i<=watki_online;i++){
			th[i-1]=i;
		}
		for(i=1;i<=watki_online;i++){
			dla_clock(th[i-1], argv, ograniczenie);
			printf("\n");
		}
		
		return 0;
	}
	if(argv[2]==NULL){
		printf("Za malo argumentow\nForm_haslo path l_th\n");
		exit(1);
	}
	if(argv[3]!=NULL){
		//-------zmienna do porownania(sformatowane haslo):
		char *cry= argv[1];
	//$6$5MfvmFOaDU$C40Ns2JUBObP9pgnKwEKN3GAa00ROXNmGwpJqTP96oTwUzzG71B3.k1pKgIqjOpJ4WiGv/8UgYavCtLqYfEHD0
	//$6$5MfvmFOaDU$/mnhojWFSmRMY7FWtlL.KetNI2G.ycHD8iywzwzOAUlHTQ0EedwwGQUycTELHMF81zHoLXqEiVzwZYgnNe8Y7/
	//$6$5MfvmFOaDU$dVBD5.9WY.SeEGyy1Dy7FBRqNaKPyNvJ/t9M.BuqdKFP.H7neVoa8q6lVNF/1nBupRzSVFxelhVmEXXiydSn8.
	
		//-------sciezka do pliku;
		char *path= argv[2]; //sciezka do slownika

		//-------liczba watkow
		int th= atoi(argv[3]); //liczba watkow
		if(th>sysconf(_SC_NPROCESSORS_ONLN)){
			th= sysconf(_SC_NPROCESSORS_ONLN);
		}
		//-------dosolenie
		char *salt= "$6$5MfvmFOaDU$";

		//-------zmienne
		struct stat sb;
		off_t len;
		char *p;
		int fd;
		char c[30];
		int i;
		int j;
		int enter=0;
		int *_od=malloc(sizeof(int)*th);
		int *_do=malloc(sizeof(int)*th);
		int podzial=0;
	
		//-------Dane z dokumentu do char array
		//(p[23] to pojedynczy znak)
		//fstat wyciąga statystyki z pliku. sb.st_size - ile znaków znajduje się w pliku (fd);
		//-------
		fd = open(path, O_RDONLY);
		fstat(fd, &sb);
		p= mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
		close(fd);
	
		//------Zliczenie wierszy w pliku 
		for(i=0;i<sb.st_size;i++){//zwraca liczbe wierszy;
			if(p[i]=='\n'){
				enter++;		
			}
		}

		//------Double array na cale slowa slowa[rows][columns]
		//slowa[0] zwruci całe slowo, slowa[0][12] zwruci 13 litere w slowie;
		char **slowa=(char**) calloc(enter, sizeof(char*));
		for(i=0;i<enter;i++){
			slowa[i]= (char*) calloc(30, sizeof(char));
		}
		//Tu dzielimy znaki z p[] na cale słowa do double array;
		int cc=0;
		j=0;
		for(i=0;i<enter;i++){
			while(p[cc]!='\n'){
				slowa[i][j]=p[cc];
				cc++;
				j++;
			}
			slowa[i][j+1]='\0';
			j=0;
			cc++;
		}
		maxx=enter;
	//printf("%d\n",enter);

		//------Dzielimy na watki od kad do kad;
		fromto(enter, th, &_od, &_do);
			//printf("%d - %d\n", _od[0], _do[0]);
			//printf("%d - %d\n", _od[1], _do[1]);
			//printf("%d - %d\n", _od[2], _do[2]);
			//printf("%d - %d\n", _od[3], _do[3]);

		//------Tworzenie struktury dla watkow
		Argument *arg=malloc(sizeof(Argument)*th);
		for(i=0;i<th;i++){
			arg[i]._od=_od;
			arg[i]._do=_do;
			arg[i].slowa=slowa;
			arg[i].cry=cry;
			arg[i].path=path;
			arg[i].nr=i;
			arg[i].salt=salt;
			arg[i].conf=cry;
			arg[i].haslo=NULL;
		}
		system("clear");
		printf("Rozpoczynam skanowanie...\n");
		printf("Postep: 0%%\n");
		ile=(maxx*2)/100; //co ile % ma wyswietlac;
		//printf("%d   %d\n",ile,maxx);

		//------Tworzenie watkow
		pthread_t *tread;
		tread=(pthread_t *)malloc(sizeof(pthread_t)*th);
		for(i=0;i<th;i++){
			//printf("%d", arg[i].nr);
			pthread_create(&tread[i],NULL,watki,&arg[i]);
			//pthread_join(tread[i],NULL);
		}


		for(i=0;i<th;i++){
			pthread_join(tread[i],NULL);
		}
		//printf("%d\n",enter/th);
		system("clear");
		printf("Skanowanie ukonczone!\n");
		printf("HASLA nie znaleziono :<\n");
	}


	return 0;
}
