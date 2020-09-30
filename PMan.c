#define _GNU_SOURCE

#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
// #include "header.h"

// linked list data structure to store the processes
typedef struct proc {
	pid_t pid;
	struct proc *next;
	char proc_path[256];
	bool run;
} proc;
proc* proc_list = NULL;

int tokenize_Input(char** input){
	char * rline = readline("PMan: > ");
	if(strcmp(rline, "exit") != 0){
		return -1;
	}
	if(strcmp(rline, "") != 0){
		return 0;
	}
	int i;
	char * t = strtok(rline, " ");
	int numArrElements = sizeof(rline)/sizeof(rline[0]);
	for(i = 0; i < numArrElements; i++) {
		input[i] = t;
		t = strtok(NULL, " ");
		if(!t){
			break;
		}
	}
	return 1;
}

int main() {

	while(1){
		char * user_input[256];
		int token_succ = tokenize_Input(user_input);
		if(token_succ){
			// executeInput(user_input);
			printf("token was a success\n");
		}
}
return 0;
}


int bg_entry(char **argv) {
	pid_t pid;
	pid = fork();
	if(pid == 0){
		if(execvp(argv[1], argv) < 0){
			printf("Error: execution of %s failed \n", argv[1]);
			exit(-1);
			// perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	} else if(pid > 0){
		printf("Pid is greater than 0, : %d\n", getpid());
		// store information of the background child process in data structures here
	} else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}
