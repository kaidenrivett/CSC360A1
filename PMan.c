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

//// Function Prototypes
int tokenizeInput(char** input);
void processHandler();
void inputHandler();
void procRemoval(pid_t pid);

// linked list data structure to store the processes
typedef struct proc {
	struct proc *next;
	pid_t pid;
	char proc_path[256];
	// this may need to be an integer instead
	bool run;
} proc;
proc* proc_list = NULL;

proc * findProc(pid_t pid){
	proc * p = proc_list;
	while(p!=NULL){
		if(p->pid == pid){
			return p;
		}
		p = p->next;
	}
	return NULL;
}

///////////////////// 			MAIN
int main() {

	while(1){
		char * user_input[256];
		int token_succ = tokenizeInput(user_input);
		processHandler();
		if(token_succ == 1){
			// handleInput(user_input);
		}
}
return 0;
}


int tokenizeInput(char** input){
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

void processHandler() {
	pid_t pid;
	int state;

	while(1) {
		pid = waitpid(-1, &state, WCONTINUED | WUNTRACED | WNOHANG);
		if(pid>0){
			if(WIFSTOPPED(state)){
				printf("Background process %d has been stopped.\n", pid);
				proc* p = findProc(pid);
				p->run = false;
			} else if(WIFCONTINUED(state)){
					printf("Background process %d has started.\n", pid);
					proc* p = findProc(pid);
					p->run = true;
		} else if(WIFSIGNALED(state)){
			printf("Background process %d has been killed.\n", pid);
			procRemoval(pid);
		} else if(WIFEXITED(state)){
			printf("Background process %d has been terminated.\n", pid);
			procRemoval(pid);
		}
	} else {break;}
}
}

void procRemoval(pid_t pid){
	if(findProc(pid) == NULL){
		return;
	}
	proc * p = proc_list;
	proc * p1 = NULL;
	while(p!=NULL){
		if(p->pid = pid){
			if(p == proc_list){
				proc_list = proc_list->next;
			} else {
				p1->next = p->next;
			}
			free(p);
			return;
		}
		p1 = p;
		p = p->next;
	}
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
