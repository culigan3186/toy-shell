#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// gethostusername.c 에서 요구하는 include 파일
#include <pwd.h>

#define MAX_LEN_LINE    100
#define LEN_HOSTNAME    30

// shell Color define
#define COLOR_RED  "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN  "\x1b[32m"
#define INIT_COLOR  "\x1b[0m"

// shell Bold color define
#define BOLDGREEN   "\x1b[1;${32}m"

// Italicred color 
#define ITALICRED_RED  "\x1b[3;${31}m"

#define BufferSize  30

//Function
void call_hostname();
void pid_call();


int main(void)
{
    char command[MAX_LEN_LINE];
    char *args[] = {command, NULL};
    int ret, status;
    pid_t pid, cpid;
    
    // child process count
    int count = 0;

    while (true) {
        char *s;
        int len;

        // hostname initialize
        char hostname[LEN_HOSTNAME + 1];
        memset(hostname, 0x00, sizeof(hostname));
        gethostname(hostname, LEN_HOSTNAME);

        // current working directory
        char pwd[BufferSize];
        getcwd(pwd,BufferSize);
        
        // console upgrade , hostname,username,pwd  everytime show 
        printf(COLOR_GREEN "%s@%s:" COLOR_YELLOW "%s $" INIT_COLOR ,getpwuid(getuid())->pw_name,hostname,pwd);      
        //printf("MyShell $ ");
        s = fgets(command, MAX_LEN_LINE, stdin);
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        
        // hostanme,username은 Myshell에 command 입력 후 동작
        //hostname();
        // ----- hostname,username --- end-----
        
        // getpid는 Myshell에 command 입력 후 매번 동작
        pid_call();
        
        // ----- getpid --- end ---

        len = strlen(command);
        printf("%d\n", len);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }

        // exit 구문 추가 
        if (strcmp(command,"exit")==0) break;     // break; or exit(0); 
        // exit end ---

        printf("[%s]\n", command);
      
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } 
        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0);
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status));
                // WEXITSTATUS(status)는 자식프로세스가 정상 종료되었을 때 반환 값   
            }
        }
        else {  /* child */
            ret = execve(args[0], args, NULL);          // 외부 프로그램을 실행시키는 명령어 예를들어 /bin/ls를 입력하면 여기서 실행
            printf("I'm the child. My PID = %d\n",getpid());
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");
                count++;    // add
                return 1;
            }
        } 
        printf("end command line, child count %d\n",count); // add
    } 
    return 0;
}


void call_hostname()
{
    char hostname[LEN_HOSTNAME + 1];
    memset(hostname, 0x00, sizeof(hostname));
    printf("username: %s\n", getpwuid(getuid())->pw_name);  

    gethostname(hostname, LEN_HOSTNAME);
    printf("hostname: %s\n", hostname);
}

void pid_call()
{
    printf("----> My PID = %d\n", getpid());
    printf("----> My Parent's PID = %d\n", getppid());
}