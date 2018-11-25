/*
 * System Programming Homework5
 * mysh.c
 * By Chang Yeon Jo
 * StudentID : 32144548
 * User ID : sys32144548
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

void help()
{
	printf("Simple Help\n");
	printf("--------------------------------------------------\n");
	printf("종료를 원하시면 exit 입력\n");
	printf("Directory 이동을 원하시면 cd 명령어 사용\n");
	printf("Background Processing을 원하시면 마지막에 '&' 입력\n");
	printf("Redirection을 원하시면 '>' or '>>'  or '<' 입력\n");
	printf("--------------------------------------------------\n");
}

void changeDirectory(char* tokens[])
{
	//입력된 디렉토리 경로가 없을경우 환경변수 home을 가져와서 home으로 이동
	if(!tokens[1])
		chdir(getenv("HOME"));
	else if(tokens[1])
		chdir(tokens[1]);
	else{
		printf("잘못된 입력입니다\n");
		printf("사용법 : cd [Directory Name]\n");
	}
}

int tokenize(char* buf, char* delims, char* tokens[], int maxTokens)
{
	int token_count = 0;
	char* token;

	token = strtok(buf, delims);
	while(token!=NULL && token_count < maxTokens)
	{
		tokens[token_count] = token;
		token_count++;	
		token = strtok(NULL, delims);
	} //tokens에 null값이 들어간 경우 빠져 나옴(Parsing이 끝남)
	tokens[token_count] = NULL;

	return token_count;
}

bool run(char* line)
{
	bool bg = false;// & 연산자 관련
	int search=0; int re=0; int reDouble=0; int reInput=0;
	//search : 문자열 찾기, re : >, reDouble : >>, reInput : <
	int fd; 
	int status;
	int token_count;
	pid_t child;
	char delims[] = " 	\n"; //토큰을 " "(공백)과 tab, '\n' 으로 구별
	char* tokens[256];

	
	token_count = tokenize(line, delims, tokens, sizeof(tokens)/sizeof(char*));

	//내장 기능  exit, help, cd  실행
	if(token_count == 0) //아무것도 입력이 없을 경우 
		return true;

	if(strcmp(tokens[0], "exit")==0) //exit(종료) 입력시
		return false;

	if(strcmp(tokens[0], "help")==0){ //help(도움말) 입력시
		help();
		return true;
	}
	if(strcmp(tokens[0], "cd")==0){ //cd(디렉토리 변경) 입력시
		changeDirectory(tokens);
		return true;
	}

	//Background Processing과 Redirection을 해야 하는지 검사 
	for(search=0;search<token_count;search++)
	{
		if(strcmp(tokens[search], "&")==0){
			bg = true;
			tokens[search]=NULL;
			break;
		}
		if(strcmp(tokens[search], ">")==0){
			re = search;
			tokens[search]=NULL;
			break;
		}
		if(strcmp(tokens[search], ">>")==0){
			reDouble = search;
			tokens[search]=NULL;
			break;
		}
		if(strcmp(tokens[search], "<")==0){
			reInput = search;
			tokens[search]=NULL;
			break;
		}
	}		

	child = fork();
	if(child<0){ //fork error
		printf("fork error\n");
		_exit(1);
	}
	else if(child==0){ //Child Task
		if(re){// > 입력시 출력 재지정
			fd = open(tokens[re+1], O_WRONLY|O_TRUNC|O_CREAT, 0664);
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
		}
		if(reDouble){// >> 입력시출력 재지정(기존에 것에  이어서 출력)
			fd = open(tokens[reDouble+1], O_WRONLY|O_APPEND|O_CREAT, 0664);
			close(STDOUT_FILENO);
			dup2(fd, STDOUT_FILENO);
		}
		if(reInput){// < 입력시 입력 재지정
			fd = open(tokens[reInput+1], O_RDONLY);
			close(STDIN_FILENO);
			dup2(fd, STDIN_FILENO);
		}
		execvp(tokens[0], tokens);
		printf("execvp Error\n"); //이 문장 출력시 execvp가 제대로 안됨
		_exit(0);
	}
	else{ //Parent Task
		if(!bg) //bg==1인 경우 Background로 처리하기 위해 wait를 하지 않음
			waitpid(child, &status, 0);
	}

	return true;
}

int main()
{
	char line[1024];
	printf("도움말을 원하시면 'help'를 입력하세요.\n");
	while(1){
		printf("%s $ ", get_current_dir_name());
		fgets(line, sizeof(line)-1, stdin);
		if(run(line) == false) break;
	}
	
	return 0;
}	
