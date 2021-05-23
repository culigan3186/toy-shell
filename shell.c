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



int main(void)
{
    char command[MAX_LEN_LINE];
    char *args[] = {command, NULL};
    int ret, status;
    pid_t pid, cpid;
    
    while (true) {
        char *s;
        int len;
        
        printf("MyShell $ ");
        s = fgets(command, MAX_LEN_LINE, stdin);
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        
        // hostanme,username은 Myshell에 command 입력 후 동작
        char hostname[LEN_HOSTNAME + 1];
        memset(hostname, 0x00, sizeof(hostname));
        printf("username: %s\n", getpwuid(getuid())->pw_name);  

        gethostname(hostname, LEN_HOSTNAME);
        printf("hostname: %s\n", hostname);
        
        // ----- hostname,username --- end-----
        

        // getpid는 Myshell에 command 입력 후 매번 동작
        printf("----> My PID = %d\n", getpid());
        printf("----> My Parent's PID = %d\n", getppid());
        
        // ----- getpid --- end ---

        len = strlen(command);
        printf("%d\n", len);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }

        // exit 구문 추가 
        if (strcmp(command,"exit")==0) exit(0);     // break; 도 가능
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
            }
        }
        else {  /* child */
            ret = execve(args[0], args, NULL);
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");   
                return 1;
            }
        } 
    }
    return 0;
}
