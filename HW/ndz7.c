#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


enum{
  BUFFER_SIZE = 1000,
  DASH = 1,
  IN = 2,
  OUT = 3,
  SPACE = 0
};

void garbage_collector(int (*fd)[2][2], char ***argv) {
  close((*fd)[0][0]);
  close((*fd)[0][1]);
  close((*fd)[1][0]);
  close((*fd)[1][1]);
  free(*argv);
}

char last_symb = 'a';

char* read_line(void) {
  char *line = (char*)calloc(BUFFER_SIZE, sizeof(char));
  size_t curr_size = BUFFER_SIZE;
  size_t ind = 0;
  char c;
  while ((c = getchar()) != '\n' && c != EOF) {
    if (ind >= curr_size) {
      curr_size += BUFFER_SIZE;
      line = (char *)realloc(line, curr_size*sizeof(char));
      if (!line) {
        perror("Not enough memory\n");
        _exit(1);
      }
    }
    line[ind] = c;
    ++ind;
  }
  if (ind == 0)
    last_symb = c;
  if (c == EOF && ind == 0) {
    _exit(0);
  }
  if (ind >= curr_size) {
    ++curr_size;
    line = (char *)realloc(line, curr_size);
    if (!line) {
      perror("Not enough memory\n");
      _exit(1);
    }
  }
  line[ind] = '\0';
  return line;
}

int is_separator(char c) {
  return (c == '|') || (c == '<') || (c == '>');
}

char** parsing_line(const char *line, size_t *argc, ssize_t *syntax_error, ssize_t **separators, size_t *separators_number) {
  char **argv = (char**) calloc(BUFFER_SIZE, sizeof(char*));
  if (!argv) {
    perror("Not enough memory\n");
    _exit(1);
  }
  char quote_flag = 0, apo_flag = 0, shell_separator = 0;
  size_t length = strlen(line);
  size_t argv_ind = 0, arg_ind = 0, separators_ind = 0;
  char last = 'a';
  int ind1 = 0;
  for (size_t line_ind = 0; line_ind < length; ++line_ind) {

      if ((line[line_ind] == ' ' || line[line_ind] == '\t' || line[line_ind] == '|' || line[line_ind] == '<' || line[line_ind] == '>') && !quote_flag && !apo_flag){//обработка пришедшего разделителя
        switch (line[line_ind]) {
          case '|':
            shell_separator = DASH;
            break;
          case '<':
            shell_separator = IN;
            break;
          case '>':
            shell_separator = OUT;
            break;
          case ' ':
            if (shell_separator) {
              argv[argv_ind][arg_ind]='\0';
              (*separators)[separators_ind] = argv_ind;
              separators_ind++;
            }
            shell_separator = SPACE;
            break;
        }
        if (shell_separator) {
          if (arg_ind) {
            if ((shell_separator == OUT && ((argv[argv_ind][arg_ind-1] == '2' && arg_ind < 2) || is_separator(argv[argv_ind][arg_ind-1]))) || (shell_separator && is_separator(argv[argv_ind][argv_ind-1]))
                || (is_separator(argv[argv_ind][arg_ind-1]))) {
              argv[argv_ind][arg_ind] = line[line_ind];
              last = line[line_ind];
            }
            else{
              arg_ind = 0;
              argv_ind++;
            }
          }
          if (!arg_ind) {
            if (is_separator(last)) {
              arg_ind= ind1;
              argv_ind--;
            }else {
              argv[argv_ind] = (char *) calloc(BUFFER_SIZE, sizeof(char));
            }
            argv[argv_ind][arg_ind] = line[line_ind];
            last = line[line_ind];
          }
          ++arg_ind;
          continue;
        }
        if (arg_ind) {
          argv[argv_ind][arg_ind] = '\0';
          argv_ind++;
          ind1 = arg_ind;
          arg_ind = 0;
        }
        continue;
    }
    last = line[line_ind];
    if (shell_separator){ // after separators processing
      argv[argv_ind][arg_ind]='\0';
      (*separators)[separators_ind] = argv_ind;
      arg_ind = 0;
      ++argv_ind;
      ++separators_ind;
      shell_separator = 0;
    }
    if (line[line_ind] == '\'') {
      if (quote_flag){
        quote_flag = 0;
       /* if (arg_ind && !apo_flag) {
          //argv[argv_ind][arg_ind] = '\0';
          argv_ind++;
          arg_ind = 0;
        }*/
        continue;
      }
      quote_flag = 1;
      continue;
    }

    if (line[line_ind] == '\"') {
      if (apo_flag){
        apo_flag = 0;
        /*if (arg_ind && !quote_flag) {
          argv[argv_ind][arg_ind] = '\0';
          argv_ind++;
          arg_ind = 0;
        }*/
        continue;
      }
      apo_flag = 1;
      continue;
    }

    if (!arg_ind) {
      argv[argv_ind] = (char*)calloc(BUFFER_SIZE, sizeof(char));
      if (!argv[argv_ind]){
        perror("Not enough memory\n");
        _exit(1);
      }
    }
    argv[argv_ind][arg_ind] = line[line_ind];
    ++arg_ind;
  }
  if (apo_flag || quote_flag){
    *syntax_error = argv_ind;
  }
  if (shell_separator) {
    (*separators)[separators_ind] = argv_ind;
    separators_ind++;
  } else {
    (*separators)[separators_ind] = argv_ind+1;
    separators_ind++;
  }

  *separators_number = separators_ind;
  *argc = argv_ind+1;
  return argv;
}

char** get_argv(size_t *argc, ssize_t *syntax_error, ssize_t **separators, size_t *separators_number) {
  char *line = read_line();
  char **argv = NULL;
  if (last_symb != '\n')
     argv = parsing_line(line, argc, syntax_error, separators, separators_number);
  free(line);
  return argv;
}

char* get_current_directory(void) {
  char *path = (char*) calloc(BUFFER_SIZE, sizeof(char));
  if (!path){
    perror("Not enough memory\n");
    return NULL;
  }
  getcwd(path, BUFFER_SIZE);
  return path;
}

void sig_handler(int signum) {
  _exit(0);
}

int cd(char **args) {
  int res;
  //printf("%s", args[1]);
  if (!args[1]) res = chdir(getenv("HOME"));
  else if (strlen(args[1]) == 0) return 1;
  else res = chdir(args[1]);

  if (res < 0) {
    fprintf(stderr, "Directory %s not found\n", args[1]);
    return 1;
  }
  return 0;
}

void conveyor(void){
  for(;;){
    size_t argc = 0;
    pid_t pid;
    int status;
    ssize_t syntax_error = -1;// index of the first syntax error in argv
    ssize_t *separators = (ssize_t *) calloc(BUFFER_SIZE, sizeof(ssize_t)); // indexes of all separators in argv
    size_t separators_number = 0;
    if (!separators) {
      perror("Not enough memory\n");
      _exit(1);
    }

    int fd[2][2];
    if (pipe(fd[0]) < 0) {
      perror("pipe");
      _exit(1);
    }
    if (pipe(fd[1]) < 0) {
      perror("pipe");
      _exit(1);
    }
    last_symb = 'a';
    char **argv = get_argv(&argc, &syntax_error, &separators, &separators_number);
    if (last_symb == '\n') continue;
    /*fprintf(stderr, "sep num = %ld\n", separators_number);
    if (separators_number == 1) {
      argv[separators[0]] = "|";
    }*/
    if (!argv[argc-1]) {
      separators[separators_number-1] --;
    }
   /* for (int i = 0; i < separators_number; i++){
      printf("%ld ", separators[i]);
    }
    printf("\n");*/
    if (argc == 1 && !strcmp(argv[0], "exit")) { // exit
      garbage_collector(&fd, &argv);
      printf("Shell is closing, bye\n");
      _exit(0);
    }
    if (argc == 1 && !strcmp(argv[0], "pwd")) { // pwd
      printf("%s\n", get_current_directory());
      garbage_collector(&fd, &argv);
      continue;
    }
   /* if (!strcmp(argv[0], "cd")) {
      cd(argv);
      continue;
    }*/
    /*if (argv[argc - 1] == NULL) {
      argc--;
    }*/
    /*printf("argc = %ld\n", argc);
    for (int i = 0; i < argc; i++) {
      if (argv[i])
        printf("%d %s\n", i, argv[i]);
    }*/
    if (!separators[0]) {
      fprintf(stderr, "Syntax error\n");
      continue;
    }
    int commands_number = separators_number - 1;
    ssize_t ind  = -1;
    int offset[2];
    pipe(offset);
    for (pid = fork();;ind++) {
      if (ind > commands_number-1) { // <-------------- ?
        if (!pid) _exit(0);
        else wait(NULL);
        break;
      }
     // fprintf(stderr, "----%ld\n", ind);
     // fflush(stderr);
      if (pid < 0) {
        perror("Fork error\n");
        garbage_collector(&fd, &argv);
        _exit(1);
      }
      if (!pid) {
        /*if (argc == 1) {
          ind = 0;
        }*/
        if ((ind == commands_number-1 || separators_number == 1) && argv[separators[ind+1]] != NULL) {
          fprintf(stderr, "Syntax error\n");
          _exit(1);
        }
        if (ind >= 0) {
          if (strlen(argv[separators[ind]]) > 2) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }
          if (!strcmp(argv[separators[ind]], "||") || !strcmp(argv[separators[ind]], "<<")
              || !strcmp(argv[separators[ind]], ">|") ||
              !strcmp(argv[separators[ind]], "|>") || !strcmp(argv[separators[ind]], "<|")
              || !strcmp(argv[separators[ind]], "|<") ||
              !strcmp(argv[separators[ind]], "><") || !strcmp(argv[separators[ind]], "<>")) {
                fprintf(stderr, "Syntax error\n");
                _exit(1);
          }
          if (syntax_error > separators[ind] && syntax_error < separators[ind+1]) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }
          if (separators[ind+1] - separators[ind] < 2) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }
        }else {
          if (syntax_error >= 0 && syntax_error < separators[0]) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }
        }
        if (argc == 1) {
          ind = -1;
        }
        //start processing if OK
        int read_flag = 0;
        int write_flag1 = 0;
        int write_flag2 = 0;
        int write_flag3 = 0;
        char **command;
        if (ind == - 1) {
          command = argv;
        } else {
          command = argv + separators[ind] + 1;
        }
        if (!strcmp(command[0], "exit")) {
          _exit(2);
        }
        int off = 0;
        while (argv[separators[ind + 1]] && strcmp(argv[separators[ind+1]], "|") ) {
          off++;
          //fprintf(stderr, "ind = %ld", ind);
          if (!strcmp(argv[separators[ind+1]], ">") || !strcmp(argv[separators[ind+1]], ">>") ||  !strcmp(argv[separators[ind+1]], "2>")) {
            int file_descriptor = -1;
            char redirection_type = -1;
            if (!strcmp(argv[separators[ind+1]], ">")) {
              redirection_type = 1;
              write_flag1++;
            }
            if (!strcmp(argv[separators[ind+1]], ">>")) {
              redirection_type = 2;
              write_flag1++;
            }
            if (!strcmp(argv[separators[ind+1]], "2>")) {
              redirection_type = 3;
              write_flag3++;
            }
            if (read_flag > 1 || write_flag1 > 1 || write_flag3 > 1) {
              //fprintf(stderr, "Syntax error\n");
              _exit(5);
            }
           // argv[separators[ind+1]] = NULL;
            //dup2(fd[0][0], 0);
            if (argv[separators[ind+1] + 1] != NULL) { // есть что-то после '>'
              if (separators[ind + 2] - separators[ind + 1] > 2) { // такая ситуация ' cmd > a.txt b.txt [cmd]'
                fprintf(stderr, "Syntax error\n");
                _exit(1);
              }
              if (redirection_type == 2) {
                file_descriptor = open(argv[separators[ind + 1] + 1], O_WRONLY | O_APPEND | O_CREAT, 0664);
              }else {
                file_descriptor = open(argv[separators[ind + 1] + 1], O_WRONLY | O_CREAT, 0664);
              }
              //fprintf(stderr, "fd = %d\n", file_descriptor);
              if (file_descriptor < 0) {
                if (!strcmp(argv[separators[ind + 1] + 1], "|") || !strcmp(argv[separators[ind + 1] + 1], ">>")
                    || !strcmp(argv[separators[ind + 1] + 1], "2>") || !strcmp(argv[separators[ind + 1] + 1], ">")
                    || !strcmp(argv[separators[ind + 1] + 1], "<")){
                  fprintf(stderr, "Syntax error\n");
                  _exit(1);
                }
                fprintf(stderr, "File %s not found\n", argv[separators[ind + 1] + 1]);
                _exit(1);
              }
              argv[separators[ind + 1] + 1] = NULL;
            } else {
              fprintf(stderr, "Syntax error\n");
              _exit(1);
            }
            if (redirection_type == 3) {
              dup2(file_descriptor, 2);
            }else {
              dup2(file_descriptor, 1);
            }
          }
          if (!strcmp(argv[separators[ind + 1]], "<")) {
            read_flag++;
            int file;
            if (read_flag > 1) {
              _exit(5);
            }
            //argv[separators[ind + 1]] = NULL;
            if (argv + separators[ind + 1] + 1 && argv[separators[ind + 1] + 1]) { // if we have "cmd < a.txt"
              file = open(argv[separators[ind + 1] + 1], O_RDONLY);
              if (file < 0) {
                if (!strcmp(argv[separators[ind + 1] + 1], "|") || !strcmp(argv[separators[ind + 1] + 1], ">>")
                    || !strcmp(argv[separators[ind + 1] + 1], "2>") || !strcmp(argv[separators[ind + 1] + 1], ">")
                    || !strcmp(argv[separators[ind + 1] + 1], "<")){
                  fprintf(stderr, "Syntax error\n");
                  _exit(1);
                }
                fprintf(stderr, "File %s not found\n", argv[separators[ind + 1] + 1]);
                _exit(1);
              }
              argv[separators[ind + 1] + 1] = NULL;
              if (separators[ind + 2] - separators[ind + 1] > 2) { // такая ситуация ' cmd < a.txt b.txt' more than 1 file
                fprintf(stderr, "Syntax error\n");
                _exit(1);
              }
            }else {
              fprintf(stderr, "Syntax error\n");
              _exit(1);
            }
            dup2(file, 0);
            /*if (ind + 1 < commands_number-1 && separators_number != 1) {
              dup2(fd[1][1], 1);
            }*/
          }

          //fprintf(stderr, "%s\n", argv[separators[ind + 1]]);
          if (argv[separators[ind + 1]]) {
            if (!strcmp(argv[separators[ind + 1]], "||") || !strcmp(argv[separators[ind + 1]], "<<")
                || !strcmp(argv[separators[ind + 1]], ">|") ||
                !strcmp(argv[separators[ind + 1]], "|>") || !strcmp(argv[separators[ind + 1]], "<|")
                || !strcmp(argv[separators[ind + 1]], "|<") ||
                !strcmp(argv[separators[ind + 1]], "><") || !strcmp(argv[separators[ind + 1]], "<>")) {
              fprintf(stderr, "Syntax error\n");
              _exit(1);
            }
            if (strlen(argv[separators[ind + 1]]) > 2) {
              fprintf(stderr, "Syntax error\n");
              _exit(1);
            }
          }
          //fprintf(stderr, "%ld %ld\n", separators[ind + 1], separators[ind]);
          if (separators[ind+1] && ind != - 1 && (separators[ind+1] - separators[ind] < 2)) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }
          argv[separators[ind +1]] = NULL;
          ind++;

          if (syntax_error > separators[ind] && syntax_error < separators[ind+1]) {
            fprintf(stderr, "Syntax error\n");
            _exit(1);
          }

        }
        /*int i = 0;
        for (i = 0; i < 4; i++) {
          fprintf(stderr, "%s\n", command[i]);
        }*/
        //fprintf(stderr, "%ld %ld\n", separators[ind + 2], separators[ind + 1]);
        if (argv[separators[ind + 1]] && !argv[separators[ind + 1] + 1]) {
          fprintf(stderr, "Syntax error\n");
          _exit(1);
        }
        //fprintf(stderr, "%d %d\n", write_flag3, write_flag1);
        write(offset[1], &off, sizeof(int));
        if (argv[separators[ind + 1]] == NULL || !strcmp(argv[separators[ind+1]], "|")) { // if we have '|'
          argv[separators[ind+1]] = NULL;
          if (!read_flag && ind >= 0)
            dup2(fd[0][0], 0);
          if (ind < commands_number-1 && separators_number != 1 && !write_flag1 && !write_flag2 && !write_flag3) {
            dup2(fd[1][1], 1);
          }
          if (ind >= 0) {
            close(fd[0][0]);
            close(fd[1][0]);
            close(fd[0][1]);
            close(fd[1][1]);
          }
          //fprintf(stderr, "%d\n", read_flag);
          execvp( command[0], command);
          if (!strcmp(command[0], "exit")) {
            _exit(2);
          }
          if (!strcmp(command[0], "pwd")) {
            printf("%s\n", get_current_directory());
            _exit(0);
          }
          if (!strcmp(command[0], "cd")) {
            _exit(3);
          }
          fprintf(stderr, "Command %s not found\n", command[0]);
          }
          _exit(1);
        // if we skip something

      } // father
        if (ind >= 0 ) {
          close(fd[0][0]);
          close(fd[0][1]);
        }
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) == 2) {
          garbage_collector(&fd, &argv);
          fprintf(stdout,  "Shell is closing, bye\n");
          _exit(0);
        }
        char **command;
        if (ind == - 1) {
          command = argv;
        } else {
          command = argv + separators[ind] + 1;
        }
        if (WEXITSTATUS(status) == 3) {
          int flag = 0;
          if (command[1] && !strcmp(command[1], "|")) {
            //fprintf(stderr, "dqwdqwdq\n");
            command[1] = NULL;
            flag = 1;
          }
          //fprintf(stderr, "dqwdqwdq\n");
          cd(command);
          if (flag) {
            command[1] = "|";
          }
        }
        if (!WIFEXITED(status) || (WEXITSTATUS(status) && WEXITSTATUS(status) != 3)) {
          if (WEXITSTATUS(status) == 5) {
            fprintf(stderr, "Syntax error\n");
          }
          /*if (WEXITSTATUS(status) == 6) {
            fprintf(stde)
          }*/
          break;
        }
        fd[0][0] = fd[1][0];
        fd[0][1] = fd[1][1];
        if (pipe(fd[1]) == -1) {
          perror("pipe");
          exit(-1);
        }
        int off;
        read(offset[0], &off, sizeof(int));
        if (argv[separators[ind+1]] != NULL && strcmp(argv[separators[ind+1]], "|") ) {
          ind+= off;
        }
        pid = fork();

    }

    garbage_collector(&fd, &argv);
  }
}

int main(void) {
  //sigaction(SIGTERM, &(struct sigaction) {.sa_handler = sig_handler, .sa_flags = SA_RESTART}, NULL);
  conveyor();
  return 0;
}
