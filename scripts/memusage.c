/*****************************************************************
 * Memstat program - shows maximum memory usage of given program *
 * Handy tool made by Bartosz [ponury] Ponurkiewicz 2009         *
 * GPL Licence or whatsoever                                     *
 *****************************************************************/

#include <stdio.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <fcntl.h>
#include <sys/reg.h>

#if __WORDSIZE == 64
#define ORIG_XAX ORIG_RAX
#else
#define ORIG_XAX ORIG_EAX
#endif

pid_t child;
unsigned int    size   = 0;
unsigned int    VmSize = 0, CVmSize = 0;
unsigned int    VmRSS  = 0, CVmRSS  = 0;
unsigned int    VmData = 0, CVmData = 0;
unsigned int    VmStk  = 0, CVmStk  = 0;

void sig_trap(int sig){
    kill(child, 9);
    fprintf(stderr, "\n\n[!] Caught signal - %d\n", sig);
}

void sig_status(int unused){
//    printf("\n----------\n");
    if (!unused){
//      fprintf(stderr, "%4u KB of memory used\n", VmSize);
      FILE *f = fopen("usedmem", "w");
      if (!f) {
        fprintf(stderr, "Error, cannot open memusage file to write!\n");
        exit(1);
      }
      fprintf(f, "%d\n", VmSize);
      fclose(f);
//        printf("Max VmSize: %8u kB   (%4u MB)\n", VmSize, VmSize/1024);
//        printf("Max VmData: %8u kB\n", VmData);
//        printf("Max VmRSS : %8u kB\n", VmRSS);
//        printf("Max VmStk : %8u kB\n", VmStk);
    }
    else{
        fprintf(stderr, "VmSize: Max %8u kB\t\tCurrent: %8u kB\n",VmSize,CVmSize);
        fprintf(stderr, "VmData: Max %8u kB\t\tCurrent: %8u kB\n",VmData,CVmData);
        fprintf(stderr, "VmRSS : Max %8u kB\t\tCurrent: %8u kB\n",VmRSS,CVmRSS);
        fprintf(stderr, "VmStk : Max %8u kB\t\tCurrent: %8u kB\n",VmStk,CVmStk);
    }
}

int main(int argc, char **argv){
    FILE    *file;
    char    name[32];
    int     status;
    int     len, i=2;
    char    fname[256];
    char    closeERR = 0;
    char    closeOUT = 0;
    char    executed = 0;
    

//    printf("=== Bartosz [ponury] Ponurkiewicz\n");
//    printf("== memstat v1.0\n");
    if (argc < 2){
        fprintf(stderr, "Usage: %s -[O|E] <program_name> [args]\n", argv[0]);
        fprintf(stderr, "Ex   : %s /bin/ls\n", argv[0]);
        return 1;
    }
    if (argv[1][0] == '-'){
        for (len = 1; len < strlen(argv[1]); len++){
            if (argv[1][len] == 'O')
                closeOUT = 1;
            else if (argv[1][len] == 'E')
                closeERR = 1;
            else{
                fprintf(stderr, "Usage: %s -[O|E] <program_name> [args]\n", argv[0]);
                fprintf(stderr, "Ex   : %s /bin/ls\n", argv[0]);
                fprintf(stderr," -O = close STDOUT\n -E = close STDERR\n");
                return 1;
            }
        }
        argv++;
        argc--;
    }
//    printf("== testing: %s", argv[1]);
//    for (; i<argc; i++)
//        printf(" %s", argv[i]);
//    printf("\n\n");

    if ((child = fork())){
        signal(SIGINT, sig_trap);
        signal(SIGTSTP, sig_status);

        snprintf(fname, sizeof(fname), "/proc/%d/status", child);
        if (!(file = fopen(fname, "r"))){
            fprintf(stderr, "[-] No /proc/%d/status file!", child);
            return 1;
        }
        while (1){
            waitpid(child, &status, 0);

            if (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP){
                //fprintf(stderr, "[-] Program received signal %d\n",
                    //WSTOPSIG(status));
            }
            else if (WIFEXITED(status) || WIFSIGNALED(status))
                break;

            if (!executed && ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_XAX, NULL) == __NR_execve){
                if (ptrace(PTRACE_SYSCALL, child, 0, 0))
                    fprintf(stderr, "[-] PTRACE_SYSCALL failed\n");
                executed = 1;
                continue;
            }

            rewind(file);
            memset(name, 0, sizeof(name));
            while (fscanf(file, "%32s %u\n", name, &size) != EOF){
                len = strlen(name);
                if (len == 7){
                    if (!strcmp(name, "VmSize:") && (CVmSize = size, size > VmSize))
                        VmSize = size;
                    else if (!strcmp(name, "VmData:") && size > VmData)
                        VmData = size;
                }
                else if (len == 6){
                    if (!strcmp(name, "VmRSS:") && size > VmRSS)
                        VmRSS = size;
                    else if (!strcmp(name, "VmStk:") && size > VmStk)
                        VmStk = size;
                }
                size = 0;
            }
            if (ptrace(PTRACE_SYSCALL, child, 0, 0))
                fprintf(stderr, "[-] PTRACE_SYSCALL failed\n");
        }
        sig_status(0);
    }
    else{
        argv++;

        if (closeOUT){
            close(1);
            open("/dev/null", O_WRONLY);
        }
        if (closeERR){
            close(2);
            open("/dev/null", O_WRONLY);
        }
        
//        fprintf(stderr,"argc=%d\n",argc);
        
        if(argc>=4) {
        	close(1);
        	open(argv[2],O_WRONLY | O_CREAT,00644);
        	//fprintf(stderr,"1 %s numer=%d\n",argv[2],1);
        }
        
        if(argc>=3) {
        	close(0);
        	open(argv[1],O_RDONLY);
        	//fprintf(stderr,"0 %s numer=%d\n",argv[1],0);
        	//open(argv[1],O_RDONLY);
        } else {
	        close(0);
	    }
        signal(SIGTSTP, SIG_IGN);

        if (ptrace(PTRACE_TRACEME, 0, 0, 0))
            fprintf(stderr, "[-] PTRACE_TRACEME failed\n");
        if (execvp(argv[0], argv))
            fprintf(stderr, "[-] execvp failed!\n");
    }
    return 0;
}

