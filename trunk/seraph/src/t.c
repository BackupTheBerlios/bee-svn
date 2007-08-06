#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


pid_t* child_pid;
int running =1 ;
void onSigTerm(int sig)
{
    kill(-*child_pid,SIGTERM);
    running = 0;
    printf("killed[%d]\n", *child_pid);
    if(!*child_pid) {
        printf("ERROR\n");
        exit(127);
    }
}

void main() {
    pid_t pid =0;
    child_pid = malloc(sizeof( pid_t ));
    int status;

    pid = fork() ;

    if(!pid){
        int p;
        printf("child1\n");
        if(setpgid(0,0)) { printf("error\n"); exit(127); }
        p=fork();
        if(p){exit(0);}
        execlp("/bin/sleep", "sleep", "100",0);
    }
    else{
        *child_pid = pid;
        sleep(1);
        signal(SIGTERM,onSigTerm);
    }
    while(running)
        waitpid(-1, &status,WNOHANG);
        ;
}
