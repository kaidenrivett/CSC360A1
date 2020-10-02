#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>

//// Function Protypes ////

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




//proc created for linked list of proc_namees
typedef struct process{
	pid_t pid;			//proc_name id
	char* proc_name;		//proc_name name
	char proc_path[512];		//proc_path to proc_name
	int running; 		//0 = off, 1 = on
	struct process* next;	//link to next proc
}process;

//reference for linked list head
process* proc_nameList = NULL;

//list of accepted commands
char* user_commands[] = {
	"bg",
	"bglist",
	"bgkill",
	"bgstop",
	"bgstart",
	"pstat",
	"exit",
};

// search for proc id in linked list
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
		char* input[1024];
		int ready = tokenizeInput(input);
		procUpdate();
		if(ready){
			inputHandler(input);
		}
		procUpdate();
	}
	return 0;
}

int tokenizeInput(char** input){
	char * rline = readline("PMan: > ");
	if(!strcmp(rline, "exit")){
		printf("Exiting PMan. Goodbye.\n");
		return -1;
		free(rline);
	}
	if(!strcmp(rline, "")){
		return 0;
	}
	int i;
	char* t = strtok(rline, " ");
	for(i = 0; i < sizeof(rline)/sizeof(rline[0]); i++){
		input[i] = t;
		t = strtok(NULL, " ");
		if(!t){
			break;
		}
	}
	return 1;
}

int commandInspector(char* input){
	if(input == NULL){
		return 0;
	}
	int i;
	for(i = 0; i < strlen(input); i++){
		if(!isdigit(input[i])){
			return 0;
		}
	}
	return 1;
}

void inputHandler(char** input){
	int command = -1;
	int i;
	// index through the available user commands
	for(i = 0; i < 7; i++){
		if(!strcmp(input[0], user_commands[i])){
			command = i;
			break;
		}
	}
	switch(command){
		// case 0 relates to bg command
		case 0:{
			if(input[1] != NULL){
				bgEntry(input);
				break;
			}
			printf("Error: invalid command to background\n");
			return;
		}
		// case 1 relates to bglist
		case 1:{ //bgList
			bgList();
			break;
		}
		// case 2 relates to bgkill
		case 2:{//bgkill <proc_name id>
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				if(pid != 0){
					bgKill(pid);
				}
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		// case 3 relates to bgstop
		case 3:{//bgstop <proc_name id>
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStop(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		// case 4 relates to bstart
		case 4:{//bgstart <proc_name id>
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStart(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		// case 5 relates to pstat
		case 5:{//pstat <proc_name id>
			if(input[1] != NULL){
				pid_t pid = atoi(input[1]);
				pStat(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		// case 6 relates to exiting the PMan
		// case 6:{
		// 	if(strcmp(input[1],"exit")){
		// 		printf("Exiting PMan. Goodbye.\n");
		// 		exit(EXIT_SUCCESS);
		// 	}
		// }
		default:{
			printf("PMan: > %s:\tcommand not found\n", input[0]);
			break;
		}
	}
}

// created bgEntry and zombie process with reference to Tutorial A1_sampleCode.c in Assignment1 File from connex


///// Check file on connex on how to implement execvp when you use ./inf and have 2 arguments passed.

void bgEntry(char** input){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		char* cmd = input[1];
		if(execvp(cmd, &input[0]) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {

		// check for zombie process
		int status;
		int retVal = waitpid(pid, &status, WNOHANG | WUNTRACED| WCONTINUED);
		if(retVal == -1){
			printf("Error: waitpid failed\n");
			exit(1);
		}
		// using cwd to get current working directory
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		procMerge(pid, input[1], cwd);
		printf("Background process %d has started.\n", pid);
		process* proc = searchProc(pid);
		proc->running = 1;
		usleep(100);
		sleep(1);
	}
	else {
		perror("fork failed");
		exit(EXIT_FAILURE);
}
}

//shows all running proc_name ids
void bgList(){
	int total_proc = 0;
	int active_proc = 0;
	printf("------- Active Process Names -------\n");
	process* proc = proc_nameList;
	while(proc != NULL){
		if(proc->running){
			printf("%d:\t%s/%s\n", proc->pid,proc->proc_path,proc->proc_name);
			active_proc++;
		}
		total_proc++;
		proc = proc->next;
	}
	if(active_proc != total_proc){
		printf("------- Inactive Process Names -------\n");
		proc = proc_nameList;
		while(proc != NULL){
			if(!proc->running){
				printf("%d:\t%s/%s\n\n", proc->pid,proc->proc_path,proc->proc_name);
			}
			proc = proc->next;
		}
	}
	printf("Total background jobs active:\t%d\n\n", active_proc);
}

//terminates a proc_name id
void bgKill(pid_t pid){
	if(searchProc(pid) == NULL){
		printf("Error: invalid pid\n");
		return;
	}
	if(kill(pid, SIGTERM) == 0){
		usleep(100);
	}
	else{
		printf("Error: failed to execute bgkill.\n");
	}
}

//stops a running proc_name id
void bgStop(pid_t pid){
	if(searchProc(pid) == NULL){
		printf("Error: invalid pid\n");
		return;
	}
	if(kill(pid, SIGSTOP) == 0){
		usleep(100);
	}
	else{
		printf("Error: failed to execute bgstop.\n");
	}
}

//starts a stopped proc_name id
void bgStart(pid_t pid){
	if(searchProc(pid) == NULL){
		printf("Error: invalid pid\n");
		return;
	}
	if(kill(pid, SIGCONT) == 0){
		printf("Background process %d has started.\n", pid);
		process* proc = searchProc(pid);
		proc->running = 1;
		usleep(100);
	}
	else{
		printf("Error: failed to execute bgstart\n");
	}
}

//gives stats and status of a proc_name id
void pStat(pid_t pid){
	if(searchProc(pid) == NULL){
		printf("Error: Process %d does not exist.\n", pid);
		return;
	}
	char statPath[32];
	char statusPath[32];
	sprintf(statPath, "/proc/%d/stat", pid);
	sprintf(statusPath, "/proc/%d/status", pid);
	FILE* fp = fopen(statPath, "r");
	if(fp != NULL){
		char ret = 0;
		char data[100];
		int i = 1;
		do{
			ret = fscanf(fp, "%s", data);
			if(i == 2){
				printf("comm:\t%s\n", data);
			}
			if(i == 3){
				printf("state:\t%s\n", data);
			}
			if(i == 14){
				float utime = atof(data)/sysconf(_SC_CLK_TCK);
				printf("utime:\t%f\n", utime);
			}
			if(i == 15){
				float stime = atof(data)/sysconf(_SC_CLK_TCK);
				printf("stime:\t%f\n", stime);
			}
			if(i == 24){
				printf("rss:\t%s\n", data);
			}
			i++;
		}while(ret != EOF);
		fclose(fp);
	}
	else{
		printf("Error: could not read stat file.\n");
	}
	fp = NULL;
	fp = fopen(statusPath, "r");
	if(fp != NULL){
		char data[100];
		while(fgets(data, sizeof(data), fp)){
			char* c = strtok(data, "\t");
			// change this to !strcmp()?
			if(strcmp(c, "voluntary_ctxt_switches:") == 0){
				printf("voluntary_ctxt_switches:\t%s\n", strtok(strtok(NULL, "\t"), "\n"));
			}
			// change this to !strcmp()?
			else if(strcmp(c, "nonvoluntary_ctxt_switches:") == 0){
				printf("nonvoluntary_ctxt_switches:\t%s\n", strtok(strtok(NULL, "\t"), "\n"));
			}
		}
		fclose(fp);
	}
	else{
		printf("Error: could not read status file.\n");
	}
}

//add input proc_name to linked list
void procMerge(pid_t pid, char* proc_name, char* cwd){
	process* proc = (process*)malloc(sizeof(proc));
	proc->pid = pid;
	proc->proc_name = proc_name;
	strcpy(proc->proc_path, cwd);
	proc->running = 1;
	proc->next = NULL;
	if(proc_nameList == NULL){
		proc_nameList = proc;
	}
	else{
		process* tmp = proc_nameList;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = proc;
	}
}

void procUpdate(){
	pid_t pid;
	int status;
	while(1){
		pid = waitpid(-1, &status, WCONTINUED | WNOHANG | WUNTRACED);
		if(pid > 0){
			if(WIFSTOPPED(status)){
				printf("Background proc_name %d was stopped.\n", pid);
				process* proc = searchProc(pid);
				proc->running = 0;
			}
			else if(WIFCONTINUED(status)){
				printf("Background proc_name %d was started.\n", pid);
				process* proc = searchProc(pid);
				proc->running = 1;
			}
			else if(WIFSIGNALED(status)){
				printf("Background proc_name %d was killed.\n", pid);
				procRemoval(pid);
			}
			else if(WIFEXITED(status)){
				printf("Background proc_name %d was terminated.\n", pid);
				procRemoval(pid);
			}
		}
		else{
			break;
		}
	}
}

//remove proc_name from linked list
void procRemoval(pid_t pid){
	if(searchProc(pid) == NULL){
		return;
	}
	process* proc1 = proc_nameList;
	process* proc2 = NULL;
	while(proc1 != NULL){
		if(proc1->pid == pid){
			if(proc1 == proc_nameList){
				proc_nameList = proc_nameList->next;
			}
			else{
				proc2->next = proc1->next;
			}
			free(proc1);
			return;
		}
		proc2 = proc;
		proc = proc->next;
	}
}
