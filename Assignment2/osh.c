#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

/**
* Hard definitions
*/
#define MAX_LINE 80 // The maximum length command
#define HISTORY_SIZE 10 // history

/**
* Function prototypes
*/
void interactive();
int execute_command(char *bin, char **args);
int save_history(char **history, char *cmd_input, int commands_run);
int print_history(char **history, int commands_run);
int run_history(char **history, int commands_run, int num_back);

/**
* int main(int argc, char *argv[])
*
* @param argc {int} number of commandline terms eg {'./ass' => argc=1}
* @param argv {char **} binary eg {'./ass ls -al' as ['./ass', 'ls','-al']}
*
* execute command if given arguments, else start interactive mode
*/
int main(int argc, char *argv[]) {
  if(argc > 1) {
    char *args[MAX_LINE/2 + 1];
    int real_index,
        i,
        offset = 1;
    // iterate over all arguments, without the meta
    for (i=1, real_index = i-offset; i < (argc); real_index = ++i-offset) { // printf("string = %s\n", argv[i]);
      args[real_index] = argv[i]; // printf("args[%d] = %s\n", real_index, args[real_index]);
    }
    args[real_index] = NULL; // printf("args[%d] = %s\n", real_index, args[real_index]);
    execvp(args[0], args);
  } else interactive();
  return 0;
}

/**
* void interactive()
*
* start interactive prompt for user to input Linux commands
*/
void interactive(){
  /**
  * Initialize char double arrays to get input args and
  * to hold our history
  */
  char last[80];
  char **args, // command line arguments
       **history; // hold our history
  args = malloc(MAX_LINE * sizeof(char *));
  history = malloc(HISTORY_SIZE * sizeof(char *));
  int i;
  for (i = 0; i < MAX_LINE; i++){
    args[i] = malloc(MAX_LINE * sizeof(char));
    args[i][0] = '\0';
  }
  for (i = 0; i < HISTORY_SIZE; i++){
    history[i] = malloc(HISTORY_SIZE * sizeof(char));
    history[i][0] = '\0';
  }

  /**
  * Interactive while loop controled by input
  */
  int run = 1, // flag to determine when to exit program */
      counter = 0,
      commands_run = 0; // position on each string */
  while (run) {
    printf("osh> ");
    fflush(stdout);
    /**
    * Get input from stdin and save to args list
    */
    size_t argc = 0;
    char c;
    int line_length = 0;
    do {
      c = getchar();
      for(counter = 0; !isspace(c); counter++,c = getchar()) {
        args[argc][counter] = c; //putchar (c);  //printf("%c", c);
        last[line_length++] = c;
      }
      args[argc++][counter] = '\0';
      last[line_length] = '\0';
    } while (c == ' ');
    /**
    * Kill switch and end loop or
    * exec input command and setup for next round
    */
    if(strcmp (args[0],"exit") == 0 || strcmp (args[0],"quit") == 0){
      run = 0;
    } else if(strcmp (args[0],"history") == 0){
      print_history(history, commands_run);
    } else if(strcmp (args[0],"!!") == 0){
      run_history(history, commands_run, 0);
    } else if(args[0][0] == '!' && isdigit (args[0][1])){
      int history_num = 0;
      if(sscanf(args[0], "!%d", &history_num) == 1){
        //printf("!%d", history_num);
        if(history_num > 0 && history_num <= HISTORY_SIZE && history_num <= commands_run)
          run_history(history, commands_run, history_num-1);
        else
          printf("Error: Invalid history request\n");
      } else printf("!shitwentwrong\n");
    }else {
      free(args[argc]);
      args[argc] = NULL;
      execute_command(args[0], args);
      if(!(argc > 0 && args[argc-1][0] == '&')) wait(0); // Wait if & is appended to command
      args[argc] = malloc(MAX_LINE * sizeof(char));
      args[argc][0] = '\0';
      commands_run = save_history(history, last, commands_run);
    }
  }
  return;
}

/**
* int execute_command(char *bin, char **args)
*
* @param bin {char *} binary eg {'ls' in 'ls -al'}
* @param args {char **} binary eg {'ls -al' as ['ls','-al']}
*
* 1. fork a child process using fork()
* 2. the child process will invoke execvp()
* 3. if command included &, parent will invoke wait()
*/
int execute_command(char *bin, char **args){
  pid_t pid = fork();
  if (pid == 0) { // Code only executed by child process //printf("Child PID: %i", pid); //printf("user input: `%s`", bin);
    execvp(args[0], args);
  }
  else if (pid < 0) { // failed to fork //printf("Failed to fork"); // Throw exception
    return -1;
  }
  else { // parent
    pid = getpid(); //printf("parent_pid = %d\n", pid);  //printf("Process Complete.\n");
  }
  return 0;
}

/**
* int save_history(char **history, char *cmd_input, int commands_run)
*
* @param history {char **} history string list
* @param cmd_input {char *} cmd str eg {'ls -al'}
* @param commands_run {int} number of commands that have been run
*
* update history to include cmd_input
*/
int save_history(char **history, char *cmd_input, int commands_run){
  int index = commands_run++ % HISTORY_SIZE;
  strcpy (history[index], cmd_input);
  return commands_run;
}

/**
* int print_history(char **history, int commands_run)
*
* @param history {char **} history string list
* @param commands_run {int} number of commands that have been run
*
* print history
*/
int print_history(char **history, int commands_run){
  int hist_num = HISTORY_SIZE+1,
      last_index = (commands_run-1)%HISTORY_SIZE,
      i;
  for(i = last_index+1; i < HISTORY_SIZE; i++){
    if(--hist_num <= commands_run)
      printf("%d %s\n", hist_num, history[i%HISTORY_SIZE]);
  }
  for(i = 0; i <= last_index; i++){
    if(--hist_num <= commands_run)
      printf("%d %s\n", hist_num, history[i%HISTORY_SIZE]);
  }
  return commands_run;
}

/**
* int run_history(char **history, int commands_run, int num_back)
*
* @param history {char **} binary eg {'ls' in 'ls -al'}
* @param commands_run {int} number of commands that have been run
* @param num_back {int} place in history to run
*
* run command at num_back in history
*/
int run_history(char **history, int commands_run, int num_back){
  int index = (commands_run-1-num_back)%HISTORY_SIZE,
      i;
  char **args; // command line arguments
  args = malloc(MAX_LINE * sizeof(char *));
  for (i = 0; i < MAX_LINE; i++){
    args[i] = malloc(MAX_LINE * sizeof(char));
    args[i][0] = '\0';
  }
  printf("%d: %s\n", num_back+1, history[index]);
  size_t argc = 0;
  int counter = 0;
  i = 0;
  do {
    for(counter = 0; isalpha(history[index][i]); counter++,i++) {
      args[argc][counter] = history[index][i]; // putchar (history[index][i]);  //printf("%c", c);
    }
    args[argc++][counter] = '\0';
  } while (history[index][i++] == ' ');
  args[argc++] = NULL;
  execute_command(args[0], args); // printf("%d: %s\n", argc, args[0]);
  return commands_run;
}