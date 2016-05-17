//Implementation of a basic shell
//Project 5 CS 0449
//Jake Halloran (jph74@pitt.edu)
//Last Edited: 4/24/16

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/times.h>
#include<time.h>
#include<signal.h>

//function prototyping cause i hate main at bottom
void parse_command(char *, char **);
void run_function(char **);
void redirect_streams(char **);


int main(void){
	char * userCommands[50];
	char userInput[1000];
	int savedout;
	int savedin;
	int result = 0;
	struct tms *begin;
	struct tms *end;
	clock_t temp = 0;
	
	printf("\nHalloran Shell: \n");
	
	//Save copies of stdout and stdin
	savedout = dup(1);
	savedin = dup(0);
	
	//Initialize time holders 
	begin = malloc(sizeof(struct tms));
	end = malloc(sizeof(struct tms));
	end -> tms_utime = 0;
	end -> tms_stime = 0;
	
	//Loop forever, until user enters exit
	while(1){
		//Make sure input and output is reset each time in case of prior redirection
		freopen("/dev/tty", "a", stdout);
		dup2(savedin,0);
				
		//Line to indicate to user to do stuff
		printf("Command $ ");
		
		//Read a line of input from user
		fgets(userInput,1000,stdin);
		
		//Turn input line into commands
		parse_command(userInput,userCommands);
		
		//Run exit from shell
		if(strncmp(userCommands[0],"exit",4)==0){
			printf("\n");
			exit(0);
		}
		
		//Process cd within the shell
		else if(!strncmp(userCommands[0],"cd",2)){
			result = chdir(userCommands[1]);
			if(result!=0){
				printf("Error changing directory, path not found.\n");
			}
		}
		
		//Special case handling of cd with timing, just does cd, no timing
		else if(!strncmp(userCommands[0],"time",4)&&!strncmp(userCommands[2],"cd",2)){
			printf("Cannot time cd, not a program call, will complete directory change however.\n");
			result = chdir(userCommands[3]);
			if(result!=0){
				printf("Error changing directory, path not found.\n");
			}
		}
		
		//Special case handling of exit with timing, just does exit, no timing
		else if(!strncmp(userCommands[0],"time",4)&&!strncmp(userCommands[2],"exit",4)){
			printf("Cannot time exit, not a program call, will complete exit however.\n\n");
			exit(0);
		}
		
		//Call function to fork and run whaterver program user wants
		else{
			run_function(userCommands);
		}
	}
	
	return 0;
}

//Parse inputs into a list of commands
void parse_command(char * userInput, char* *userCommands){
	int inputFlag = 0, outputFlag = 0, delay = 0;
	
	*userCommands = strtok(userInput," \t\n,.()|&;");
	
	//Uses the less verbose time formatting of unix time
	if(!strncmp(*userCommands,"time",4)){
		*++userCommands = "-p";
	}
	
	while(*userCommands!=NULL){
		//Increment the position if possible
		if(!inputFlag&&!outputFlag&&!delay){
			*userCommands++;
		}
		
		//Reset the delay
		if(delay){
			delay = 0;
		}
		
		//Read the next command in
		*userCommands = strtok(NULL, " \t\n()|&;");
		
		//Loop break that was mostly useful for the previous implementation of this code
		if(*userCommands ==NULL){
			break;
		}
		
		//Flags stdin for redirection
		if(!inputFlag&&!strncmp(*userCommands,"<",1)){
			inputFlag = 1;
		}
		
		//Flags stdout for redirection
		else if(!outputFlag&&!strncmp(*userCommands,">",1)){
			outputFlag = 1;
		}
		
		//Redirects stdin
		else if(inputFlag){
			freopen(*userCommands,"r",stdin);
			inputFlag = 0;
			delay = 1;
		}
		
		//Redirects stdout
		else if(outputFlag){
			freopen(*userCommands,"w",stdout);
			outputFlag = 0;
			delay = 1;
		}
	}
	//Appends a null terminal for marking the end of the command list
	*++userCommands = "\0";
}

//Execute generic process with command list
void run_function(char **userCommands){
	pid_t process_id;
	int result;
	//If forking gets screwed up just exit before the processes get all jumbled
	if((process_id = fork()) <0){
		printf("Error forking process.\n");
		exit(1); //Exit the failed process
	}
	//If the new process can't be found, report it
	else if(process_id==0){
		if(execvp(*userCommands,userCommands)<0){
			printf("Error executing new process.\n");
			exit(1); //Exit the failed process
		}
	}
	//Wait for the new process to complete
	else{
		while(wait(&result) != process_id);
	}
}