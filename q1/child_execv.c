#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main()
{
    pid_t   pid;
    int     status;

    pid = fork();
    if (pid < 0)
    {
        printf("Error: Fork!\n");
        return (1);
    }
    else if (pid == 0)
    {
        char *args[] = {NULL};
        execv("/home/kaan/Desktop/OS/q1/myprogram", args);
    }
    else 
    {
        wait(&status);
        printf("Child process finished execution.\n");
    }
    return (0);
}