//ADAMAKIS CHRISTOS 2148 
//CHRISTODOULOU DIMITRIS 2113
//OMADA 2

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <error.h>

#include "CCR.h"

#define MAX 2

//orizoume to bridge
CCR_DECLARE(bridge);

// arxikopoiisi metablitwn pou xrisimopoioume sto sigxronismo
volatile int red_run=0, blue_run=0;
volatile int red_wait=0, blue_wait=0;
volatile int enter_blue=0, exit_blue=0;
volatile int enter_red=0, exit_red=0; 
volatile int red_pass=0, blue_pass=0;
volatile int priority_red = 0,priority_blue =0 , finish_main=0;

//sinarti gia na perasoun ta mple
void *blue_function(void *assignment){
	
	int check;
	
	//ka8e fora auksanoume ta wait kai an einai prwti fora arxikopoioume to priority
	CCR_EXEC(bridge, 1, 
			 if((priority_blue==0)&&(priority_red==0)){
				 priority_blue=1;
				 priority_red=0;
			 }
			 blue_wait++;)
	
	//elegxoume an mporei na perasei to autoinito
	CCR_EXEC(bridge, ((blue_run<MAX) && (red_run==0) && (priority_red==0)),
			 blue_wait--;
			 blue_run++;)
	
	
	enter_blue++;
	printf("+ + + enter bridge blue %d\n", enter_blue);
	sleep(2);
	exit_blue++;
	printf("- - - exit bridge blue %d\n",exit_blue);
	
	//ka8orizoume pws 8a perasoun ta autokoinita
	CCR_EXEC(bridge, 1,
		blue_pass++;
		blue_run--;
		//dinoume proteraiotita sta red 
		if((red_wait>0) && (blue_run==0) && (blue_pass>=2*MAX)){
			priority_red=1;
			priority_blue=0;
			blue_pass=0;
		}
		//dinoume proteraiotita sto blue
		else if((blue_run< MAX ) && (blue_wait > 0)&& (blue_pass<2*MAX)){
			priority_blue=1;
			priority_red=0;
		}
		//dinoume proteraiotita sta blue
		else if((blue_pass>=2*MAX) && (red_wait==0)){
			priority_blue=1;
			priority_red=0;
		}
		//dinoume proteraiotita sta red
		else if((blue_pass<2*MAX) && (red_wait>0)){
			priority_red=1;
			priority_blue=0;
			blue_pass=0;
		})
	
	//an exoun perasei ola teleiwnei
	if((red_wait==0) && (blue_wait==0) && (red_run==0) && (blue_run==0)){
		finish_main=1;
	}
	
	return NULL;
}


void *red_function(void *assignment){
	
	int check;
	
	//ka8e fora auksanoume ta wait kai an einai prwti fora arxikopoioume to priority
	CCR_EXEC(bridge, 1, 
			 if((priority_blue==0)&&(priority_red==0)){
				 priority_red=1;
				 priority_blue=0;
			 }
			 red_wait++;)
	
	//elegxoume an mporei na perasei to autoinito
	CCR_EXEC(bridge, ((red_run<MAX) && (blue_run==0) && (priority_blue==0)),
			 red_wait--;
			 red_run++;)
	
	
	enter_red++;
	printf("-> -> -> enter bridge red %d\n", enter_red);
	sleep(2);
	exit_red++;
	printf("<- <- <- exit bridge red %d\n",exit_red);
	
	//ka8orizoume pws 8a perasoun ta autokoinita
	CCR_EXEC(bridge, 1,
		red_pass++;
		red_run--;
		
		//dinoume proteraiotita sta mple 
		if((blue_wait>0) && (red_run==0) && (red_pass>= 2*MAX)){
			priority_blue=1;
			priority_red=0;
			red_pass=0;
		}
		//dinoume proteraiotita sta red
		else if((red_run< MAX ) && (red_wait > 0)&&(red_pass < 2*MAX)){
			priority_red=1;
			priority_blue=0;
		}
		//dinoume proteraiotita sta red
		else if((red_pass>=2*MAX) && (blue_wait==0)){
			priority_red=1;
			priority_blue=0;
		}
		//dinoume proteraiotita sto blue
		else if((red_pass<2*MAX) && (blue_wait>0)){
			priority_blue=1;
			priority_red=0;
			red_pass=0;
		})
	
	//an exoun perasei ola teleiwnei
	if ((red_wait==0) && (blue_wait==0) && (red_run==0) && (blue_run==0)){
		finish_main=1;
	}
	
	return NULL;
}



int main(int argc, char *argv[]) {
	
	int res;
	int file_read;
	pthread_t thread_red,thread_blue;
	char c;

	
	if(argc!=2){
		printf("error argument\n");
		return -1;
	}
	
	
	file_read=open(argv[1], O_RDONLY, 0);
	if(file_read==-1){
		printf("file not found.\n");
		return -1;
	}
	
// 	arxikopoioume tous simatoforous 
	CRR_INIT(bridge)
	
	while(read(file_read, &c, sizeof(char)) != 0){
		if(c=='b'){
// 			stelnoume ta dedomena stin sinartisi blue
			res = pthread_create(&thread_blue, NULL, blue_function, NULL);
			if (res){
				printf("error pthread controler \n");
			}
		}
		if(c=='r'){
// 			stelnoume ta dedomena stin sinartisi red
			res = pthread_create(&thread_red, NULL, red_function, NULL);
			if (res){
				printf("error pthread controler \n");
			}
		}
		
	}
	
	//perimenei mexri na ginei to finish_main=1 gia na teleiwsei
	while(1){
		if(finish_main==1){
			break;
		}
	}
	
	
	return 0;
	
}
