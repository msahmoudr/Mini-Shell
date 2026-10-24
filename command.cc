
/*
 *
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include "command.h"
#include "tokenizer.h"
#include <cctype>
#include <fcntl.h>
#include <glob.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <wordexp.h>

void parse(std::vector<Token> &tokens) {
  Command &currentCommand = Command::_currentCommand;
  SimpleCommand *currentSimpleCommand = new SimpleCommand();
  size_t i = 0;        // a7san men el unsigned_int fe el indexing
  auto hasFile = [&] { // access by refrence (bool)
    return (i + 1 < tokens.size() && tokens[i + 1].type == TOKEN_ARGUMENT);
  };
  while (i < tokens.size() && tokens[i].type != TOKEN_EOF) {
    Token &t = tokens[i];
    if (t.type == TOKEN_COMMAND || t.type == TOKEN_ARGUMENT) {
      currentSimpleCommand->insertArgument(
          strdup(t.value.c_str())); // fix the pointer issue
      i++;
    } else if (t.type== TOKEN_REDIRECT){
      if (!hasFile()){
        fprintf(stderr, "please enter a file name");
        return;
      }
      currentCommand._outFile=strdup(tokens[i + 1].value.c_str());
      i+=2; //filename included

    }
    else if (t.type ==TOKEN_PIPE){
      if( currentSimpleCommand->_numberOfArguments==0){
        fprintf(stderr, "enter a command before the pipe");
        return;
      }
      currentCommand.insertSimpleCommand(currentSimpleCommand);
      currentSimpleCommand = new SimpleCommand();
      i++;
    }else if(t.type== TOKEN_APPEND)
    {
      if (!hasFile()){
        fprintf(stderr, "please enter a file name");
        return;
      }
      currentCommand._outFile=strdup(tokens[i + 1].value.c_str());
      currentCommand._append=1;
      i+=2; //filename included
    }else if(t.type== TOKEN_INPUT)
    {
      if (!hasFile()){
        fprintf(stderr, "please enter a file name");
        return;
      }
      currentCommand._inputFile=strdup(tokens[i + 1].value.c_str());
      i+=2; //filename included

  }
  else if(t.type== TOKEN_ERROR)
    {
      if (!hasFile()){
        fprintf(stderr, "please enter a file name");
        return;
      }
      currentCommand._errFile=strdup(tokens[i + 1].value.c_str());
      i+=2; //filename included

  }
  else if(t.type== TOKEN_REDIRECT_AND_ERROR)
    {
      if (!hasFile()){
        fprintf(stderr, "please enter a file name");
        return;
      }
      currentCommand._outFile=strdup(tokens[i + 1].value.c_str());
      currentCommand._errFile=strdup(tokens[i + 1].value.c_str());
      currentCommand._out_error=1;
      i+=2; //filename included

  }
  else if(t.type== TOKEN_BACKGROUND)
    {
      currentCommand._background=1;
      i++;
    }
    else{
      fprintf(stderr, "invalid token");
      return;
    }
  }
  if( currentSimpleCommand->_numberOfArguments>0){
    currentCommand.insertSimpleCommand(currentSimpleCommand);
  }else{
    free(currentSimpleCommand->_arguments);
    free(currentSimpleCommand);
    if (currentCommand._numberOfSimpleCommands==0){
      currentCommand.prompt();
      return;
    }
  }

  currentCommand.execute();
  /*
    Implement this function
    you must parse for all possible tokens scenario
    right now command.execute only print the format , you will inject it in your
    logic.

  */
}

SimpleCommand::SimpleCommand() {
  _numberOfAvailableArguments = 5;
  _numberOfArguments = 0;
  _arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument) {
  if (_numberOfAvailableArguments == _numberOfArguments + 1) {
    _numberOfAvailableArguments *= 2;
    _arguments = (char **)realloc(_arguments,
                                  _numberOfAvailableArguments * sizeof(char *));
  }

  _arguments[_numberOfArguments] = argument;
  _arguments[_numberOfArguments + 1] = NULL;

  _numberOfArguments++;
}

Command::Command() {
  _numberOfAvailableSimpleCommands = 1;
  _simpleCommands = (SimpleCommand **)malloc(_numberOfSimpleCommands *
                                             sizeof(SimpleCommand *));

  _numberOfSimpleCommands = 0;
  _outFile = 0;
  _inputFile = 0;
  _errFile = 0;
  _background = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand) {
  if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands) {
    _numberOfAvailableSimpleCommands *= 2;
    _simpleCommands = (SimpleCommand **)realloc(
        _simpleCommands,
        _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
  }

  _simpleCommands[_numberOfSimpleCommands] = simpleCommand;
  _numberOfSimpleCommands++;
}

void Command::clear() {
  for (int i = 0; i < _numberOfSimpleCommands; i++) {
    for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++) {
      free(_simpleCommands[i]->_arguments[j]);
    }

    free(_simpleCommands[i]->_arguments);
    free(_simpleCommands[i]);
  }

  if (_outFile) {
    free(_outFile);
  }

  if (_inputFile) {
    free(_inputFile);
  }

  if (_errFile) {
    free(_errFile);
  }

  _numberOfSimpleCommands = 0;
  _outFile = 0;
  _inputFile = 0;
  _errFile = 0;
  _background = 0;
  _append = 0;
  _out_error = 0;
}

void Command::print() {
  printf("\n\n");
  printf("              COMMAND TABLE                \n");
  printf("\n");
  printf("  #   Simple Commands\n");
  printf("  --- ----------------------------------------------------------\n");

  for (int i = 0; i < _numberOfSimpleCommands; i++) {
    printf("  %-3d ", i);
    for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++) {
      printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
    }
  }

  printf("\n\n");
  printf("  Output       Input        Error        Err&Out       Background\n");
  printf(
      "  ------------ ------------ ------------ ------------ ------------\n");
  printf("  %-12s %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
         _inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
         _out_error == 1 ? _errFile : "default", _background ? "YES" : "NO");
  printf("\n\n");
}

void Command::execute() {
  print();
  /*
      Write your code here
  */

  clear();
  prompt();
}

void Command::prompt() {
  printf("myshell>");
  fflush(stdout);
  std::string input;
  while (true) {
    std::getline(std::cin, input);
    std::vector<Token> tokens = tokenize(input);
    parse(tokens);
  }
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int main() {
  Command::_currentCommand.prompt();
  return 0;
}
