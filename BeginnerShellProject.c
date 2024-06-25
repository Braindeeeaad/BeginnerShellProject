/*
 ============================================================================
 Name        : NewProyect.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {


	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}






#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void){
	int bufsize = LSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char)*bufsize); // need to allocate space dynamically for reading lines
	int c;

	if(!buffer){
		fprintf(stderr, "lsh: allocation error\n"); //prints out error command
		exit(EXIT_FAILURE);
	}
	while(1){
		//read  a character
		c = getChar(); //store oru char reading as an int temp.

		//if we hit EOF, replace it with a null character and return
		if(c == EOF || c=='\n'){ //EOF is an int which we can compare with our c(int)
			buffer[position] = '\0';
			return buffer;
		} else{
			buffer[position] = c;

		}
		position++;

		//if we have exceeded the buffer, reallocate
		if(position >= bufsize){
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if(!buffer){
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);

			}

		}
	}

	return NULL;
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
char **lsh_split_line(char *line){
	int bufsize = LSH_TOK_BUFSIZE , position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "lsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM); //returns pointer to first token
	while(token!=NULL){
		tokens[position] = token;
		position++;

		if(position>=bufsize){
			bufsize+=LSH_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if(!tokens){
				fprintf(stderr, "lsh: allocation error\n");
				exit(EXIT_FAILURE);
			}

		}
		token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int lsh_launch(char **args){

	pid_t pid, wpid;
	int status;

	pid = fork();
	if(pid==0){
		//child process, b/c fork returns 0 if child class
		//exec command is called in the boolean operator to make a new program/process
		if(execvp(args[0],args)==-1){//if exec command returns -1, we know there was an error
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if(pid<0){
		//Error forking
		perror("lsh");
	} else{
		// Parent process
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status)); //Exits wait period for parent after child process is terminated
	}
	return 1; //return 1 to signal to calling function that we should prompt for input again
}
/*

 function decleraiton of builtin commands

 */
int lsh_cd(char ** args);
int lsh_help(char ** args);
int lsh_exit(char ** args);

/*

 List of builtin commands, followed by their corresponding functions

 */
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[])(char **) = {
	&lsh_cd,
	&lsh_help,
	&lsh_exit
};

int lsh_num_builtins(){
	return sizeof(builtin_str) / sizeof(char *);
}
/*

 Builtin functions implementations

 */
int lsh_cd(char **args){
	if(args[1]==NULL){
		fprintf(stderr, "lsh: expected argument to \"cd\"\n");
	}else{
		if(chdir(args[1]) != 0){
			perror("lsh");
		}
	}
	return 1;
}

int lsh_help(char **args){
	int i;
	printf("Indra's LSH\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for(i = 0; i< lsh_num_builtins();i++){
		printf("  %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	return 1;

}

int lsh_exit(char **args){
	return 0;
}

int lsh_execute(char **args){
	int i;

	if(args[0] == NULL){
		// An empty command was entered
		return 1;
	}

	for(i = 0; i< les_num_builtins(); i++){
		if(strcmp(args[0],builtin_str[i]) == 0){ //compares first command to each string in builtin_str array
			return(*builtin_func[i])(args); //returns the function prompted by the first command
		}
	}
	//if none of the builtin commands are tripped, then the args is put through the ls_launch function
	return lsh_launch(args);
}

void lsh_loop(void){
	char *line;
	char **args;
	int status;
	do{
		printf(">");
		line = lsh_read_line();
		args = lsh_split_line(line);
		status = lsh_execute(args);

		free(line);
		free(args);
	}while(status);


}
