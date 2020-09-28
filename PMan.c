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
// int bg_entry(char**);
// void bg_list(int);
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
		}
		printf("%s\n",cmd[0]);

if(strcmp(cmd[1], "bglist") == 0){
	printf("Yay it works!\n");
} else {
	for(int i = 0; i <strlen(*cmd); i++){
	printf("Oh no, you printed %s", cmd[i]);
}
}
return 0;
}







	FILE *fp;
	char line[128];
	NODE *current, *head;

	fp = fopen("Lipsum.txt", "r");

	while(fgets(line, sizeof(line), fp)){
		NODE *node = malloc(sizeof(NODE));
		node->data = strdup(line);
		node->next = NULL;

		if(head == NULL){
			current = head = node;
		} else {
			current = current->next = node;
		}
	}
	fclose(fp);

	// print the linked list

	/*for(current = head; current ; current=current->next){
		printf("%s", current->data);
	}
	return 0;*/
}

void bg_list() {
	printf("You are in the bglist!");

}
