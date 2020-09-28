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
struct Node {
	char *data;
	struct Node *next;
};
typedef struct Node NODE;

struct node *head = NULL;
struct node *current = NULL;


/*
 * else if (strcmp(command_elems[0], "bgkill") == 0) {
    status_change((pid_t)atoi(command_elems[1]), 15);
  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (strcmp(command_elems[0], "bgstop") == 0) {
    status_change((pid_t)atoi(command_elems[1]), 19);
  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (strcmp(command_elems[0], "bgstart") == 0) {
    status_change((pid_t)atoi(command_elems[1]), 18);
  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (strcmp(command_elems[0], "pstat") == 0) {
    pstat((pid_t)atoi(command_elems[1]));
  }

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  else if (strcmp(command_elems[0], "exit") == 0) {
      printf("Exiting\n");
      return(0);

 */

// Funtion Prototypes
void bg_entry(char** argv);
// void bg_list();
// void status_change(pid_t, int);
// void pstat_entry(pid_t);
// void bg_process_update();
// void check_zombieProcess();


// char* read_line() {
// 	char *input = NULL ;
// 	char *prompt = "PMan: > ";
//
// 	input = readline(prompt);
//
// 	return input;
// }

	// char* tokenize_input(const char *input){
	// 	char *input, *token[256];
	// 	int i = 0;
	//
	//
	// 	printf("copied string is: %s", copied);
	//
	// 	token = strtok(input, " ");
	// 	printf("%s",token);
	// 	while(token != NULL){
	// 		printf("Token: %s", token);
	// 		token = strtok(NULL, " ");
	// 		return token;
	// 	}
	// }
	#define MAX_INPUT_SIZE 1024
	char *cmd[MAX_INPUT_SIZE];

int main() {

	while(1){

		// tokenize the user input
		int i = 0;
		char *input = readline("PMan: > ");
	  cmd[i] = strtok(input, " ");
		while(cmd[i]!=NULL){
			cmd[++i] = strtok(NULL, " ");
			// printf("%s\n",cmd[i]);
		}


if(strcmp(cmd[0], "bg") == 0){
	bg_entry(cmd);
} else {
	perror("User input needed");
}
return 0;
}


	// FILE *fp;
	// char line[128];
	// NODE *current, *head;
	//
	// fp = fopen("Lipsum.txt", "r");
	//
	// while(fgets(line, sizeof(line), fp)){
	// 	NODE *node = malloc(sizeof(NODE));
	// 	node->data = strdup(line);
	// 	node->next = NULL;
	//
	// 	if(head == NULL){
	// 		current = head = node;
	// 	} else {
	// 		current = current->next = node;
	// 	}
	// }
	// fclose(fp);

	// print the linked list

	/*for(current = head; current ; current=current->next){
		printf("%s", current->data);
	}
	return 0;*/
}

void bg_entry(char **argv) {
	char *cmd_in[MAX_INPUT_SIZE];
// 	size_t n = sizeof(argv);
// 	printf("Size of the argument array is: %d\n", n);
// 	if(sizeof(argv) == 4){
// 		printf("n equals 3!\n");
// 	cmd_in[0] = argv[1];
// 	cmd_in[1] = argv[2];
// 	cmd_in[2] = argv[3];
// } else {
// 	printf("n equals 2\n");
// 	cmd_in[0] = argv[1];
// 	cmd_in[1] = argv[2];
// }
	pid_t pid;
	pid = fork();
	if(pid == 0){
		if(execvp(argv[1], argv) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	} else if(pid > 0){
		// store information of the background child process in data structures here
	} else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}
