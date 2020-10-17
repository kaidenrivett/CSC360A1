#define _GNU_SOURCE

///////////////////////////////////////////////////////////////////////////
/////////////////////// IMPORTED LIBRARIES ///////////////////////////////
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>			 				// printf(), perror()
#include <stdlib.h>			 				// malloc()
#include <unistd.h>			 				// fork(), execvp()
#include <string.h>     				// strcmp(), strcpy(), strlen(), strtok()
#include <ctype.h>       				// isdigit()
#include <signal.h>							// kill(), SIGTERM, SIGKILL, SIGSTOP, SIGCONT
#include <sys/wait.h>						// waitpid()
#include <sys/types.h>					// pid_t
#include <readline/readline.h>  // readline()

//////////////////////////////////////////////////////////////////////////
/////////////////////// FUNCTION PROTOTYPES /////////////////////////////
////////////////////////////////////////////////////////////////////////

// functions dealing with user input
int tokenizeInput(char** input); // prompts user for input and tokenizes input
int commandInspector(char* input); // determines whether user input is valid or not
void inputHandler(char** input); // decides and executes a user inputted command

// functions dealing with assignment specified commands
void bgEntry(char** input); // add a new process id to the linked list
void bgList(); // displays all running procces id's
void bgKill(pid_t pid); // terminates a process id
void bgStop(pid_t pid); // temporarily stops a process id
void bgStart(pid_t pid); // resumes a previously stopped process id
void pStat(pid_t pid); // displays status of process id

// functions dealing with processes in linked lists
void procMerge(pid_t pid, char* proc_name, char* cwd); // add the process name to the linked list
void procUpdate(); // informs current status of processes to the user
void procRemoval(pid_t pid); // remove the process name from the linked list

// linked list to store processes
typedef struct process{
	pid_t pid;
	int state;
  char proc_path[512];
	char* proc_name;
	struct process* next;
}process;
process* proc_nameList = NULL;

// possible user commands that can be inputted
char* user_commands[] = {"bg","bglist","bgkill","bgstop","bgstart","pstat"};

// searching linked list for process pid
process* searchProc(pid_t pid){
	process* proc = proc_nameList;
	while(proc != NULL){
		if(proc->pid == pid){
			return proc;
		}
		proc = proc->next;
	}
	return NULL;
}

int main(){
	while(1){
		char* cmd_input[1024];
		int ready = tokenizeInput(cmd_input);
		// update the linked list with newly added or removed processe
		procUpdate();
		// if tokenizing the input was successfull, linked list will be updated and the input
		// will be passed on to the input handler
		if(ready){
			inputHandler(cmd_input);
		}
		procUpdate();
	}
	return 0;
}

int tokenizeInput(char** input){
	// using readline method with similar implementation as rsi.c and the makefile
	char * rline = readline("PMan: > ");
	// exit PMan shell
	if(!strcmp(rline, "exit")){
		printf("Exiting PMan. Goodbye.\n");
		free(rline);
		exit(EXIT_SUCCESS);
	}
	if(!strcmp(rline, "")){
		return 0;
	}
	char* t = strtok(rline, " ");

	// determining number of elements in an array with implementation from Stackoverflow: How can I find the number of elements in an array?
	int numArrElements = sizeof(rline)/sizeof(rline[0]);
	for(int i = 0; i < numArrElements; i++){
		input[i] = t;
		t = strtok(NULL, " ");
		if(!t){
			break;
		}
	}
	return 1;
}

// determines whether the pid passed exists
int commandInspector(char* input){
	if(input == NULL){
		return 0;
	}
	for(int i = 0; i < strlen(input); i++){
		if(!isdigit(input[i])){
			return 0;
		}
	}
	return 1;
}

// executes command input to do specified task
void inputHandler(char** input){
	int cmd = -1;
	// index through  all the user commands
	for(int i = 0; i < 6; i++){
		if(!strcmp(input[0], user_commands[i])){
			cmd = i;
			break;
		}
	}
	switch(cmd){
		// case 0 refers to "bg" command
		case 0:{
			if(input[1] != NULL){
				bgEntry(input);
				break;
			}
			printf("Error: user input invalid\n");
			return;

		}
		// case 1 refers to "bglist" command
		case 1:{
			bgList();
			break;
		}
		//case 2 refers to "bgkill" command
		case 2:{
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				if(pid != 0){
					bgKill(pid);
				}
				break;
			}
			printf("Error: pid invalid\n");
			return;
		}
		// case 3 refers to "bgstop" command
		case 3:{
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStop(pid);
				break;
			}
			printf("Error: pid invalid\n");
			return;
		}
		// case 4 refers to "bgstart" command
		case 4:{
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStart(pid);
				break;
			}
			printf("Error: pid invalid\n");
			return;
		}
		// case 5 refers to "pstat" command
		case 5:{
			if(input[1] != NULL){
				pid_t pid = atoi(input[1]);
				pStat(pid);
				break;
			}
			printf("Error: pid invalid\n");
			return;
		}
		// default takes over whenever when an input that is not from the user commands list is used
		default:{
			printf("PMan: > %s:    command not found\n", input[0]);
			break;
		}
	}
}
// created with reference to given A1_Samplecode.c file from Connex
void bgEntry(char** input){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		// created with reference to demo2.c file we were given on connex
		if(strcmp(input[1], "./inf") == 0){
		char *tag = (char *)malloc(sizeof(char));
		char *interval = (char *)malloc(sizeof(char));
		tag = input[2];
		interval = input[3];
		char *argv_execvp[] = {"inf", tag, interval, NULL};
		if(execvp("./inf", argv_execvp) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	}
		/// normal input commands such as args and const etc
		char* user_cmd = input[1];
		if(execvp(user_cmd, &input[0]) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {
		// implemented method of checking zombie processes from A1_Sample code on connex
		int status;
		int retVal = waitpid(pid, &status, WNOHANG | WUNTRACED| WCONTINUED);
		if(retVal == -1){
			printf("waitpid failed\n");
			exit(1);
		}
		// retrieve current working directory
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		procMerge(pid, input[1], cwd);
		printf("Background process %d has started.\n", pid);
		process* proc = searchProc(pid);
		proc->state = 1;
		usleep(100);
		sleep(1);
	} else {
		perror("fork failed");
		exit(EXIT_FAILURE);
}
}
// Displays all active and inactive processes
void bgList(){
	int active_proc = 0;
	int total_proc = 0;
	printf("\n------->   Active Process Names   <-------\n");
	process* proc = proc_nameList;
	// display all running processes
	while(proc != NULL){
		if(proc->state){
			printf("%d:     %s/%s\n", proc->pid,proc->proc_path,proc->proc_name);
			active_proc++;
		}
		total_proc++;
		proc = proc->next;
	}
	if(active_proc != total_proc){
		printf("\n------->   Inactive Process Names   <-------\n");
		proc = proc_nameList;
		// display all non-running processes
		while(proc != NULL){
			if(!proc->state){
				printf("%d:     %s/%s\n", proc->pid,proc->proc_path,proc->proc_name);
			}
			proc = proc->next;
		}
	}
	printf("Total background jobs:     %d\n\n", active_proc);
}
// allows user to kill a specified process with pid
void bgKill(pid_t pid){
	if(searchProc(pid) == NULL){
		perror("Error: pid invalid\n");
		return;
	}
	// send the signal to terminate the process with pid
	if(kill(pid, SIGTERM) == 0){
		usleep(100);
	}
	else{
		perror("Error: bgkill failed.\n");
		exit(EXIT_FAILURE);
	}
}

// will temporarily stop the process with pid
void bgStop(pid_t pid){
	if(searchProc(pid) == NULL){
		perror("Error: pid invalid\n");
		return;
	}
	// send the signal to stop the process with pid
	if(kill(pid, SIGSTOP) == 0){
		usleep(100);
	}
	else{
		perror("Error: bgstop failed.\n");
		exit(EXIT_FAILURE);
	}
}
// will start the temporarily stopped process with pid
void bgStart(pid_t pid){
	if(searchProc(pid) == NULL){
		perror("Error: pid invalid\n");
		return;
	}
	if(kill(pid, SIGCONT) == 0){
		printf("Background process %d has started.\n", pid);
		process* proc = searchProc(pid);
		proc->state = 1;
		usleep(100);
	}
	else{
		perror("Error: bgstart failed\n");
		exit(EXIT_FAILURE);
	}
}
// displays the process stats and statuses of the process with pid
void pStat(pid_t pid){
	if(searchProc(pid) == NULL){
		printf("Error: Process %d does not exist.\n", pid);
		return;
	}
	char system_stat[64];
	char system_status[64];
	// using sprintf to read stats and status into appropriate char arrays
	sprintf(system_status, "/proc/%d/status", pid);
	sprintf(system_stat, "/proc/%d/stat", pid);
	// implemented reading only certain lines from a file from stackoverflow: C Programming - Read specific line from text file
	FILE* file = fopen(system_stat, "r");
	if(file != NULL){
		int line_number = 1;
		char retVal = 0;
		char info[100];
		do{
			retVal = fscanf(file, "%s", info);
			if(line_number == 2){
				printf("comm:   %s\n", info);
			}
			if(line_number == 3){
				printf("state:   %s\n", info);
			}
			if(line_number == 14){
				// convert into float and divide by sysconf(_SC_CLK_TCK) so time is displayed properly
				float utime = atof(info)/sysconf(_SC_CLK_TCK);
				printf("utime:   %f\n", utime);
			}
			if(line_number == 15){
				// convert into float so time is displayed properly
				float stime = atof(info)/sysconf(_SC_CLK_TCK);
				printf("stime:   %f\n", stime);
			}
			if(line_number == 24){
				printf("rss:     %s\n", info);
			}
			line_number++;
		} while(retVal != EOF);
				fclose(file);
	}
	else{
		printf("Error: could not read stat file.\n");
	}
	// collect ctxt_switches data, tokenize it, and store it into info array
	file = NULL;
	file = fopen(system_status, "r");
	if(file != NULL){
		char info[256];
		while(fgets(info, sizeof(info), file)){
			// tokenize info file and locate the voluntary_ctxt_switches and nonvoluntary_ctxt_switches
			// information we need
			char* t = strtok(info, "\t");
			if(strcmp(t,"voluntary_ctxt_switches:") == 0) {
				// tokenize with tab character as delimiter and then tokenize
				// with newline character
				printf("voluntary_ctxt_switches:\t%s\n",strtok(strtok(NULL, "\t"), "\n"));
			}
			else if(strcmp(t,"nonvoluntary_ctxt_switches:") == 0) {
				printf("nonvoluntary_ctxt_switches:\t%s\n",strtok(strtok(NULL, "\t"), "\n"));
			}
		}
		fclose(file);
	} else{
		printf("Status information file could not be read.\n");
	}
}

// merging new process into linked list
// this was created with reference to GeeksforGeeks: Linked List | Set 2 (Inserting a node)
void procMerge(pid_t pid, char* proc_name, char* cwd){
	process* proc = (process*)malloc(sizeof(process));
	proc->pid = pid;
	proc->proc_name = proc_name;
	strcpy(proc->proc_path, cwd);
	proc->state = 1;
	proc->next = NULL;
	// if linkedlist is empty make new process as head of the linked list
	if(proc_nameList == NULL){
		proc_nameList = proc;
	}
	else{
		// create temporary node, set it to next and store new process in that location
		process* tmp = proc_nameList;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = proc;
	}
}

// the process update function used implementation from demo2.c for the macros
void procUpdate(){
	pid_t pid;
	int status;
	while(1){
		pid = waitpid(-1, &status, WCONTINUED | WNOHANG | WUNTRACED);
		if(pid > 0){
			if(WIFSTOPPED(status)){
				printf("Background process %d has stopped.\n", pid);
				printf("Stopped by signal %d\n", WSTOPSIG(status));
				// change state of process in list to not running
				process* proc = searchProc(pid);
				proc->state = 0;
			}
			else if(WIFCONTINUED(status)){
				printf("Background process %d has been started.\n", pid);
				// change state of process in list to running
				process* proc = searchProc(pid);
				proc->state = 1;
			}
			else if(WIFSIGNALED(status)){
				printf("Background process %d has been killed.\n", pid);
				printf("Killed by signal %d\n", WTERMSIG(status));
				// remove process from linked list
				procRemoval(pid);
			}
			else if(WIFEXITED(status)){
				printf("Background process %d has been terminated.\n", pid);
				printf("Normal, status code = %d\n", WEXITSTATUS(status));
				// remove process from linked list
				procRemoval(pid);
			}
		}
		else{
			break;
		}
	}
}

// linked list removal was created with reference to Geeks for Geeks: Linked List | Set 3 (Deleting a node)
void procRemoval(pid_t pid){
	process* proc1 = proc_nameList;
	process* proc2 = NULL;
	// if pid is not present in list then return
	if(searchProc(pid) == NULL){
		return;
	}
	// search for pid to be removed while keeping track of the previous pid
	// so we can change proc_nameList to next
	while(proc1 != NULL){
		if(proc1->pid == pid){
			if(proc1 == proc_nameList){
				proc_nameList = proc_nameList->next;
			}
			else{
				proc2->next = proc1->next;
			}
			// free removed node
			free(proc1);
			return;
		}
		proc2 = proc1;
		proc1 = proc1->next;
	}
}
