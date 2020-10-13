//ADAMAKIS CHRISTOS 2148 
//CHRISTODOULOU DIMITRIS 2113
//OMADA 2

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "CCR.h"

#define N 5

//ka8olikes metablites pou xrisimopoioume gia ton sigxonismo
volatile int customer_wait=0, customer_in=0, all_customer=0, customer_exit=0, customer_enter=0;
volatile int train_situation=0;
volatile int start=0, start_finish=0, finish=0, next_customer=0, main_finish=0;

//orismos tou train_region
CCR_DECLARE(train_region);

// sinartisi train elegxei pote 8a ksekinisei to treno
void *thread_train(void *assignment){
	
	int check;
	
	while(1){
		//perimenei na gemisei
		CCR_EXEC(train_region, start==1,
		printf("-> -> ->\n");
		sleep(5);
		printf("[FINISH]\n");
		finish=1;)
		
		//perimenei na adiasei
		CCR_EXEC(train_region, start_finish==1,
		printf("<- <- <-\n");
		sleep(5);
		printf("[START]\n");
		if(customer_wait==0){
			train_situation=0;
		}
		//an den perimenie kanena teleiwnei
		if(customer_exit==all_customer){
			main_finish=1;
		})
		
		//ama perimenoun epibates ksipnaei tous epomenous
		CCR_EXEC(train_region, (customer_wait!=0), 
			next_customer++;
			start=0;
			finish=0;
			start_finish=0;)
		
	}
	
	
	return(NULL);
	
	
}

// sinartisi customer elegxei pote 8a epibibastei kai apobibastei o pelatis
void *thread_customer(void *assignment){
	
	int check;
	
	if(train_situation==1){
		customer_wait++;
		//perimei mexri na epistrepsei to treno gia na epibibastoun ooi epomenoi
		CCR_EXEC(train_region, next_customer>0,
			customer_wait--;
			next_customer++;
			//otan gemisei kanoume to next_customer==0 gia na kollisei to ccr
			if(next_customer==N+1){
				next_customer=0;
			})
	}
	
	//pernane oi epibates 
	CCR_EXEC(train_region, 1,
			customer_in++;
			customer_enter++;
			printf("passenger %d in\n", customer_enter);
			//gemise ksekinaei to treno
			if(customer_in == N){
				train_situation=1;
				start=1;
			})
	
	//perimei mexri na ftasei sto telos gia na arxisoun na apobibazontai
	CCR_EXEC(train_region, finish==1, 
			customer_in--;
			customer_exit++;
			printf("passenger %d out\n", customer_exit);
			//adeiase epistrefei
			if(customer_in==0){
				start_finish=1;
			})
	
	return(NULL);
	
}


int main (int argc, char * argv[]){
	
	pthread_t train,customer;
	int i, check;
	
	if(argc!=2){
		printf("error argument\n");
	}
	
	all_customer=atoi(argv[1]);
	
	//arxikopoiisi tou train_region
	CRR_INIT(train_region);
	
	pthread_create(&train , NULL , thread_train, NULL);
	
	
	for(i=0; i<all_customer; i++){
		sleep(1);
		pthread_create(&customer , NULL , thread_customer, NULL);
	}
	
	//perimenei mexri na teleiwsoun oloi oi epibates
	CCR_EXEC(train_region, main_finish==1, )
	
	return 0;
	
}
