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
#define Arr_SIZE 12

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

#define Delimit " \t\r\n\a"
//Function
void call_hostname();
void pid_call();

int main(void)
{   
    
    char command[MAX_LEN_LINE];
    char *args[] = {command, NULL};
    char **cmd_array= malloc(Arr_SIZE*sizeof(char));

    char *cmd;
    char *token;

    char builtin_cmd[MAX_LEN_LINE];
    char *param[30];

    int ret, status;

    pid_t pid, cpid;
    
    // child process count
    int count = 0;

    while (true) {
        char *s;
        int len;
        int cmd_num;
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

        


        // multi command separate 
        cmd_num = 0;
        token = strtok(command,";");
        while(token!=NULL){
            cmd_array[cmd_num]=token;
            cmd_num++;
            token=strtok(NULL,";");
        }
        printf("total command : %d \n",cmd_num);
        // ------------ end -----------------

        cmd = strtok(command," ");
        // exit 구문 추가 
        if (strcmp(command,"exit")==0)
        {
            printf("Exit Program\n");
            break;     // break; or exit(0); 
        }
        /*
        if (!strcmp(command,"cd")){
            cmd = strtok(NULL," ");
            if (chdir(cmd)<0)
            {
                fprintf(stderr, "Can't Change Directory %s\n", cmd);
                continue;
            }
            else
            {
                printf("change Directory : %s\n",cmd);
                continue;
            }
        }
        */
        // exit end ---

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
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("Exit status is %d\n", WEXITSTATUS(status));
                // WEXITSTATUS(status)는 자식프로세스가 정상 종료되었을 때 반환 값   
            }
        }
        else {  /* child */
            /*
            if (!strcmp(command,"ls"))
            {
                args[0] = "/bin/ls";
                args[1] = NULL;
            }

            if (!strcmp(command,"pwd"))
            {
                args[0] = "bin/pwd";
                args[1] = NULL;
            }
            */
            //read_command(command,param);

            // 실행해본 명령어(작동가능): ls, cd, 
            strcpy(builtin_cmd,"/bin/");
            strcat(builtin_cmd, cmd);
            args[0]=builtin_cmd;
            cmd = strtok(NULL," ");

            if(cmd == "-al")
            {
                execl("/bin/ls","ls","-al",(char*)NULL);
            }
    
            ret = execve(args[0], args, NULL);          // 외부 프로그램을 실행시키는 명령어 예를들어 /bin/ls를 입력하면 여기서 실행

            
            printf("I'm the child. My PID = %d\n",getpid());
            if (ret < 0) {
                fprintf(stderr, "execve failed\n");

                return 1;
            }
         } 
    }
    return 0;
}

void shell_builtin_cmd(){

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

void read_command(char cmd[], char *par[])
{
    char line[1024];
    int count=0;
    char *array[100], *pch;
    int i=0,j;
    while(true)
    {
        int c = fgetc (stdin);
        line[count++] = (char) c;
        if (c =='\n') break;
    }
    if (count == 1) return;
    pch = strtok(line,"\n");
    
    while(pch!=NULL){
        array[i++] = strdup(pch);
        pch = strtok(NULL,"\n");
    }
    strcpy (cmd,array[0]);
    for (int j =0;j<i;j++)
    {
        par[j] = array[j];
    }
    par[i] = NULL;
}