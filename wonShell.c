#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_LEN_LINE    100         // command line max length
#define LEN_HOSTNAME    30          // hostname max size
#define Arr_SIZE 12                 ..

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
    char *args[] = {command,NULL, NULL};

    char *cmd;
    char *token;
    char *cd_path;
    char *option;
    char builtin_cmd[MAX_LEN_LINE];


    int ret, status;

    pid_t pid, cpid;
    
    // child process count
    int count = 0;

    while (true) {
        char *s;
        int len;
        int cmd_num;
        int process_num=0;
        // hostname initialize
        char hostname[LEN_HOSTNAME + 1];
        memset(hostname, 0x00, sizeof(hostname));
        gethostname(hostname, LEN_HOSTNAME);

        // current working directory
        char pwd[BufferSize];
        getcwd(pwd,BufferSize);
        
        // console upgrade , hostname,username,pwd  everytime show 
        printf("\033[0;35m\033[1m" "%s@%s:" COLOR_YELLOW "%s $" INIT_COLOR ,getpwuid(getuid())->pw_name,hostname,pwd);      
        //printf("MyShell $ ");
        s = fgets(command, MAX_LEN_LINE, stdin);
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        
        
        // hostanme,username은 Myshell에 command 입력 후 동작
        // hostname();
        // ----- hostname,username --- end-----

        // getpid는 Myshell에 command 입력 후 매번 동작
        if (strcmp(command,"pid")==0)  pid_call();
        

        // ----- getpid --- end ---

        len = strlen(command);
        
        //printf("string length : %d\n", len);      
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }



        cmd = strtok(command," ");
        if (strcmp(cmd,"cd")==0)
            {
                cmd = strtok(NULL," ");
                if(cmd==NULL) 
                {
                    printf("No change Directory, stay here[%s]\n",pwd);
                    continue;
                }
                cd_path = cmd;
                if(chdir(cd_path)<0)
                {
                    fprintf(stderr, "Can't find [%s] directroy \n",cd_path);
                    continue;
                }
                else
                {
                    printf("change directory [%s]\n",cd_path);
                    continue;
                }
            }
        // exit 구문 추가 
        if (strcmp(command,"exit")==0)
        {
            printf("Exit Program\n");
            break;     // break; or exit(0); 
        }

        if (strcmp(command,"clear")==0 || strcmp(command,"clr")==0)
        {
            printf("\033[2J\033[1;1H");
            continue;
        }
        

        // printf("input command : [%s]\n", command);

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
    
            //printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                if(WEXITSTATUS(status)==0){
                    printf("child process normal working\n");
                }
                else{
                    printf("child process abnormal end\n");
                }
                // WEXITSTATUS(status)는 자식프로세스가 정상 종료되었을 때 반환 값   
            }
        }
        else {  /* child */
            while(true){
                

                // 실행해본 명령어(작동가능): ls, date, pwd, sleep
                if (strcmp(cmd,"hostname")==0)
                {
                    printf("hostname: %s\n", hostname);
                    break;
                }
                if (strcmp(cmd,"username")==0)
                {
                    printf("username: %s\n", getpwuid(getuid())->pw_name);
                    break;
                }


                // command 로 /bin/ls 입력시 오류가 나와서 해결
                char *ptr = strstr(cmd,"/bin/");
                if(ptr!=NULL)
                {   
                    cmd = strtok(NULL," ");
                    option = cmd;
                    args[1] = option;
                    execve(args[0],args,NULL);
                    continue;
                }
                //-----------------------------------

                strcpy(builtin_cmd,"/bin/");
                strcat(builtin_cmd, cmd);
                args[0]=builtin_cmd;

                cmd = strtok(NULL," ");
                option = cmd;
                args[1] = option;       // ls -al 같은 옵션 적용가능


                ret = execve(args[0], args, NULL);          // 외부 프로그램을 실행시키는 명령어 예를들어 /bin/ls를 입력하면 여기서 실행

                
            //printf("I'm the child. My PID = %d\n",getpid());
                if (ret < 0) {
                    fprintf(stderr, "Not found command\n");

                    return 1;
                    }
                
                }
    return 0;
    }
}
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



void explain_shell()
{

}
