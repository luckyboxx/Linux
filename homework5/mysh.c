/* System Programming Homework5 -> mysh.c / By Cho Chang Yeon / StudentID : 32144548 / User ID : sys32144548 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

void help()
{
	printf("/***************Simple Shell***************/\n");
	printf("You can use it just as the conventional shell\n\n");
	printf("Some examples of the built-in commands\n");
	printf("exit\t: exit this shell\n");
	printf("help\t: show this help\n");
	printf("&\t: background processing\n");
	printf(">\t: redirection\n");
	printf("/*****************************************/\n");
}

int tokenize(char* buf, char* delims, char* tokens[], int maxTokens){
	int token_count = 0;
	char* token;

	token = strtok(buf, delims);
	while(token != NULL && token_count < maxTokens)
	{
		tokens[token_count] = token;
		token_count++;	
		token = strtok(NULL, delims);
	}
	// parshing
	tokens[token_count] = NULL;
	return token_count;
}

bool run(char* line){
	int i;
	int fd;
	int re = 0;
	int bg = 0;
	int re_check = 0;
	int bg_check = 0;
	int token_count;
	pid_t child;
	char delims[] = " \r\t\n";
	char* tokens[128];
	
	token_count = tokenize(line, delims, tokens, sizeof(tokens)/sizeof(char*));

	if(token_count == 0) //아무런 입력이 없을 경우 
		return true;
	
	// tokens[0]에 위치한 문자열과 exit를 서로 비교하여 두 문자열이 같다면 false를 반환
	if(strcmp(tokens[0], "exit") == 0) // 터미널에 exit를 입력하면 shell을 종료한다.
		return false;
	

	// tokens[0]에 위치한 문자열과 help를 서로 비교하여 두 문자열이 같다면 help()함수를 호출  
	if(strcmp(tokens[0], "help") == 0){ // 터미널에 help를 입력하면 화면에 도움말이 출력된다.
		help();
		return true;
	}
	// handling redirection, pipe and background processing
	//Background Processing과 Redirection을 해야 하는지 검사 
	

	for(i = 0; i < token_count; i++)
	{
		if(strcmp(tokens[i],">") == 0)
		{
			re = i;
			re_check = 1;
			break;
		}
	
		if(strcmp(tokens[i],"&") == 0)
		{
			bg = i;
			bg_check = 1;
			break;
		}

	}	

	child = fork();
	if(child < 0){ //fork error
		printf("fork error\n");
		return false;
	}
	else if(child == 0){ //Child Task
		if(bg_check) {
			tokens[bg] = '\0';
		}
	
		if(re_check == 1){// > 입력시 출력 재지정
			fd = open(tokens[re + 1], O_WRONLY|O_TRUNC|O_CREAT, 0664);
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
			tokens[re] = '\0';
		}
		execvp(tokens[0], tokens);
		printf("execvp error\n"); //이 문장 출력시 execvp가 제대로 안됨
		
		return false;
	}
	else if(bg_check == false) {
		wait(NULL);
	}
	
	return true;
}	

int main()
{
	char line[1024];
	while(1){
		printf("%s $ ", get_current_dir_name());
		fgets(line, sizeof(line) - 1, stdin);
		if(run(line) == false)
			break;
	}
	
	return 0;
}
