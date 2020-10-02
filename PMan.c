#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>

#define INPUTSIZE



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
typedef struct proc{
	pid_t pid;			//proc_name id
	char* proc_name;		//proc_name name
	char proc_path[512];		//proc_path to proc_name
	int running; 		//0 = off, 1 = on
	struct proc* next;	//link to next proc
}proc;

//reference for linked list head
proc* proc_nameList = NULL;

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
proc* searchProc(pid_t pid){
	proc* p = proc_nameList;
	while(p != NULL){
		if(p->pid == pid){
			return p;
		}
		p = p->next;
	}
	return NULL;
}


int main(){
	while(1){
		char* input[1024];
		int success = tokenizeInput(input);
		procUpdate();
		if(success){
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
	int a;
	char* t = strtok(rline, " ");
	for(a = 0; a < sizeof(rline)/sizeof(rline[0]); a++){
		input[a] = t;
		t = strtok(NULL, " ");
		if(!t){
			break;
		}
	}
	return 1;
}

//check if input is valid
int commandInspector(char* input){
	if(input == NULL){
		return 0;
	}
	int a;
	for(a = 0; a < strlen(input); a++){
		if(!isdigit(input[a])){
			return 0;
		}
	}
	return 1;
}

//executes command to do particular task
void inputHandler(char** input){
	int cmd = -1;
	int i;
	for(i = 0; i < 6; i++){
		if(!strcmp(input[0], user_commands[i])){
			cmd = i;
			break;
		}
	}
	switch(cmd){
		case 0:{//bg <command>
			if(input[1] != NULL){
				bgEntry(input);
				break;
			}
			printf("Error: invalid command to background\n");
			return;

		}
		case 1:{ //bgList
			bgList();
			break;
		}
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
		case 3:{//bgstop <proc_name id>
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStop(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		case 4:{//bgstart <proc_name id>
			if(commandInspector(input[1])){
				pid_t pid = atoi(input[1]);
				bgStart(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
		case 5:{//pstat <proc_name id>
			if(input[1] != NULL){
				pid_t pid = atoi(input[1]);
				pStat(pid);
				break;
			}
			printf("Error: invalid pid\n");
			return;
		}
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


//add a new proc_name id
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

		// check for zombie process here
		int status;
		int val = waitpid(pid, &status, WNOHANG | WUNTRACED| WCONTINUED);
		if(val == -1){
			printf("Error: waitpid failed\n");
			exit(1);
		}
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		procMerge(pid, input[1], cwd);
		printf("Background process %d has started.\n", pid);
		proc* p = searchProc(pid);
		p->running = 1;
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
	int active = 0;
	int total = 0;
	printf("=== Active Process Names ===\n");
	proc* p = proc_nameList;
	while(p != NULL){
		if(p->running){
			printf("%d:\t %s/%s\n", p->pid, p->proc_path, p->proc_name);
			active++;
		}
		total++;
		p = p->next;
	}
	if(active != total){
		printf("=== Inactive Process Names ===\n");
		p = proc_nameList;
		while(p != NULL){
			if(!p->running){
				printf("%d:\t %s/%s\n\n", p->pid, p->proc_path, p->proc_name);
			}
			p = p->next;
		}
	}
	printf("Total background jobs active:\t%d\n\n", active);
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
		proc* p = searchProc(pid);
		p->running = 1;
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
		int a = 1;
		do{
			ret = fscanf(fp, "%s", data);
			if(a == 2){
				printf("comm:\t%s\n", data);
			}
			if(a == 3){
				printf("state:\t%s\n", data);
			}
			if(a == 14){
				float utime = atof(data)/sysconf(_SC_CLK_TCK);
				printf("utime:\t%f\n", utime);
			}
			if(a == 15){
				float stime = atof(data)/sysconf(_SC_CLK_TCK);
				printf("stime:\t%f\n", stime);
			}
			if(a == 24){
				printf("rss:\t%s\n", data);
			}
			a++;
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
			if(strcmp(c, "voluntary_ctxt_switches:") == 0){
				printf("voluntary_ctxt_switches:\t%s\n", strtok(strtok(NULL, "\t"), "\n"));
			}
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
	proc* p = (proc*)malloc(sizeof(proc));
	p->pid = pid;
	p->proc_name = proc_name;
	strcpy(p->proc_path, cwd);
	p->running = 1;
	p->next = NULL;
	if(proc_nameList == NULL){
		proc_nameList = p;
	}
	else{
		proc* temp = proc_nameList;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = p;
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
				proc* p = searchProc(pid);
				p->running = 0;
			}
			else if(WIFCONTINUED(status)){
				printf("Background proc_name %d was started.\n", pid);
				proc* p = searchProc(pid);
				p->running = 1;
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
	proc* p = proc_nameList;
	proc* p2 = NULL;
	while(p != NULL){
		if(p->pid == pid){
			if(p == proc_nameList){
				proc_nameList = proc_nameList->next;
			}
			else{
				p2->next = p->next;
			}
			free(p);
			return;
		}
		p2 = p;
		p = p->next;
	}
}
