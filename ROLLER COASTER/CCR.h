#define CCR_DECLARE(label)\
	pthread_mutex_t label ## _mtx;\
	pthread_cond_t label ## _q1;\
	pthread_cond_t label ## _q2;\
	int label ## _n1;\
	int label ## _n2;\

#define CRR_INIT(label)\
	pthread_mutex_init (&label ## _mtx,NULL);\
	pthread_cond_init (&label ## _q1, NULL);\
	pthread_cond_init (&label ## _q2,NULL);\
	label ## _n1 =0;\
	label ## _n2 =0;\

#define CCR_EXEC(label, cond, body) \
	check=pthread_mutex_lock(&label ## _mtx);\
	if(check){\
		printf("error mutex_lock\n");\
	}\
	while(!cond) {\
		label ## _n1++;\
		if(label ## _n2 > 0){\
			label ## _n2--;\
			check=pthread_cond_signal(&label ## _q2);\
			if(check){\
				printf("error signal q2\n");\
			}\
		}\
		check=pthread_cond_wait(&label ## _q1, &label ## _mtx);\
		if(check){\
			printf("error wait q1\n");\
		}\
		label ## _n2++;\
		if(label ## _n1 >0){\
			label ## _n1--;\
			check=pthread_cond_signal(&label ## _q1);\
			if(check){\
				printf("error signal q1\n");\
			}\
		}\
		else{\
			check=pthread_cond_signal(&label ## _q2);\
			if(check){\
				printf("error signal q2\n");\
			}\
		}\
		check=pthread_cond_wait(&label ## _q2, &label ## _mtx);\
		if(check){\
			printf("error wait q2\n");\
		}\
	}\
	body\
	if(label ## _n1 > 0){\
		label ## _n1--;\
		check=pthread_cond_signal(&label ## _q1);\
		if(check){\
			printf("error signal q1\n");\
		}\
	}\
	else if (label ## _n2 > 0){\
		label ## _n2--;\
		check=pthread_cond_signal(&label ## _q2);\
		if(check){\
			printf("error signal q2\n");\
		}\
	}\
	check=pthread_mutex_unlock(&label ## _mtx);\
	if(check){\
		printf("error mutex_unlock\n");\
	}\

