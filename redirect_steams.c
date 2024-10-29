#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


/*
    ./this <command1> <arg1> <arg2> ... /// <command2> <arg1> <arg2> ...

    <command>s MUST BE absolute path
*/

/*
1) get absolute path for the command
2) check if there's input file arv[1], if not leave as stdin
3) Extract cmd arv[2]
4) check if there's output file, arv[3], if not leave as stdout

// if first command in pipeline has input redirection
if (hasInputFile && is1stCommand) { 
  int fdin = open(inputFile, O_RDONLY, 0644);
  dup2(fdin, STDIN_FILENO);
  close(fdin);
}

// if last command in pipeline has output redirection
if (hasOutputFile && isLastCommand) { 
  int fdout = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  dup2(fdout, STDOUT_FILENO);
  close(fdout);
}

*/

#define COMMAND_SEPARATOR "///"

#define READ_SIDE 0
#define WRITE_SIDE 1

// adds NULL at the end
void copy_ptrs_from_to(char** to, char** from, int from_ix, int to_ix)
{
    for (int dest_ix = 0; dest_ix < to_ix - from_ix + 1; dest_ix++)
        to[dest_ix] = from[from_ix + dest_ix];
    to[to_ix - from_ix + 1] = NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        fprintf(stderr,
            "Usage: %s <input> <cmd> <output> \n", argv[0]);
        return 1;
    }

    // int commands_separator_ix;

    // for (commands_separator_ix = 0;
    //     commands_separator_ix < argc
    //     && strcmp(argv[commands_separator_ix], COMMAND_SEPARATOR);
    //     commands_separator_ix++)
    //     ;

    // if (commands_separator_ix == argc) {
    //     fprintf(
    //         stderr, "Command separator '%s' not found\n", COMMAND_SEPARATOR);
    //     return 1;
    // }

    char* inputFile = argv[1];
    bool hasInputFile = false;

    if (!strcmp(inputFile, "-")) {
        hasInputFile = true;
    }

    char* cmd = argv[2];

    char* outputFile = argv[3];
    bool hasOutputFile = false;

    if (!strcmp(inpuhasOutputFiletFile, "-")) {
        hasOutputFile = true;
    }

    // +1 for NULL
    char** first_cmd = (char**)malloc(sizeof(char*) * argc + 1);
    char** second_cmd = (char**)malloc(sizeof(char*) * argc + 1);

    copy_ptrs_from_to(first_cmd,  argv, 1, commands_separator_ix - 1);
    copy_ptrs_from_to(second_cmd, argv, commands_separator_ix + 1, argc - 1);


    // creates a SINGLE pipe: write to pipe_from_read_to_write[1], read from pipe_from_read_to_write[0]
    int pipe_from_read_to_write[2];
    pipe(pipe_from_read_to_write);

    int command1_pid = fork();

    if (command1_pid == 0) {
        // first command process
        close(pipe_from_read_to_write[READ_SIDE]);
        dup2(pipe_from_read_to_write[WRITE_SIDE], STDOUT_FILENO);

        execve(first_cmd[0], first_cmd, NULL);

        fprintf(stderr, "Failed to execute first: %s\n", first_cmd[0]);
        // children should exit if exec fails
        _exit(1);
    }

    int command2_pid = fork();
    if (command2_pid == 0) {
        // second command process
        close(pipe_from_read_to_write[WRITE_SIDE]);
        dup2(pipe_from_read_to_write[READ_SIDE], STDIN_FILENO);

        execve(second_cmd[0], second_cmd, NULL);

        fprintf(stderr, "Failed to execute second: %s\n", second_cmd[0]);
        // children should exit if exec fails
        _exit(1);
    }

#ifdef WAIT_FOR_CHILDREN
    int wait_status;
    waitpid(command1_pid, &wait_status, 0);
    printf("command 1 exited with status %d\n", WEXITSTATUS(wait_status));
    waitpid(command2_pid, &wait_status, WNOHANG);
    printf("command 2 exited with status %d\n", WEXITSTATUS(wait_status));
#endif

    printf("parent %s pid is %d. forked %d and %d. Parent exiting\n",
        argv[0], getpid(), command1_pid, command2_pid);
    return 0;
}