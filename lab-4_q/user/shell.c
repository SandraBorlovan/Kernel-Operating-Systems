#define _GNU_SOURCE
#include "shell.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int pid;
int id_child;

void execute_command(char command[5], char arg[12],int word){

  bool commandFound = false;

  //Reading the command mkdir directoryName
  if( strlen(command)==5 && command[0]=='m' && command[1]=='k' && command[2]=='d'&& command[3]=='i'&& command[4]=='r'&& word==2){
    commandFound = true;
    int b = creat(arg,3,2);
  }

  //Reading the command mk fileName
  else if( strlen(command)==2 && command[0]=='m' && command[1]=='k'&& word==2){
    commandFound = true;
    int b = creat(arg,3,1);
  }

  //Reading the command run arg
  else if( strlen(command)==3 && command[0]=='r' && command[1]=='u' && command[2]=='n'&& word==2){
    commandFound = true;
    int running = 0;
    if((arg[0] == 'p' || arg[0] == 'P') && strlen(arg) == 2){
      if(arg[1] == '0'){
        id_child = 0;
        pid = fork();
      }else if(arg[1] == '1'){
        id_child = 1;
        pid = fork();
      }else if(arg[1] == '2'){
        id_child = 2;
        pid = fork();
      }else{
        write( 0, "Process not defined\n> ", 23);
        running = 1;
      }
    }else{
      write( 0, "Process not defined\n> ", 23);
      running = 1;
    }
    if(running == 0){
      if(pid != 0){
          write( 0, "Running ", 9);
          write( 0, arg, 5);
          write( 0, "\n> ", 4);
       }else{
         execute(id_child);
       }
     }
  }

  //Reading the command exit
  else if( strlen(command)==4 && command[0]=='q' && command[1]=='u' && command[2]=='i' && command[3]=='t' && word==1){
    commandFound = true;
    write( 0, "Terminal quited ", 16);
    _exit();
  }

  //Reading the command exit arg
  
  else if( strlen(command)==4 && command[0]=='q' && command[1]=='u' && command[2]=='i' && command[3]=='t' && word==2){
    int processRunning;
    commandFound = true;
    if((arg[0] == 'p' || arg[0] == 'P') && strlen(arg) == 2){
     
      if(arg[1] == '0'){
        processRunning = exit(0);
      }else if(arg[1] == '1'){
        processRunning = exit(1);
      }else if(arg[1] == '2'){
        processRunning = exit(2);
      }else{
        write( 0, "Process not defined\n> ", 23);
        processRunning = -2;
      }

      if(processRunning == -1){
        write( 0, "The prosess specified is not currently running\n> ", 50);
      }else if(processRunning != -2){
        write( 0, arg, 5);
        write( 0, " quited\n> ", 11);
      }
      
    }else{
      write( 0, "Process not defined\n> ", 23);
    }
    
  }

  //Reading the command help
  else if(strlen(command)==4 && command[0]=='h' && command[1]=='e' && command[2]=='l' && command[3]=='p' && word==1){
    commandFound = true;
    write( 0, "Available commands:\n  ", 23);
    write( 0, "run arg  - to run a process arg\n  ", 35);
    write( 0, "quit     - to terminate the shell\n  ", 37);
    write( 0, "quit arg - to terminate the process arg\n  ", 43);
    write( 0, "mkdir arg - to creat a directory named arg\n  ", 47);
    write( 0, "mk arg - to creat a file named arg\n> ", 38);
  }

  //In case the command is not defined
  if(commandFound == false && pid!=0){
    write( 0, "No such command\n", 17);
    write( 0, "> ", 2 );
  }


}


void shell(int argc, char **argv){

  char input[1];
  char command[5];
  char arg[12] ;
  int index = 0;
  int word = 1;
  
 
  pid = -1;
  write( 0, "> Welcome to qemu command-line shell\n", 38 );
  write( 0, "> ", 3 );

  while(1){
      read(0, input, 1);
      
      //Continuingly reading until enter is pressed
      while(input[0] != '\r'){
        write( 0, input, 1);
      	if(input[0] == ' '){
          word++;
          index = 0;
        }else if(word == 1){	
          command[index] = input[0];
          index++; 	
        }else if(word == 2){
          arg[index] = input[0];
          index++;
        }
        read( 0, input, 1);
      }

      //Allowing commands that hava a maximum of 2 words
      if(word > 2){
        write( 0, "\n" , 1);
        write( 0, "> No such command", 17);
      }
      write( 0, "\n" , 2);
      write( 0, "> ", 2 );
      if(word <=2 && !(word==1 && index==0)){
        execute_command(command,arg,word);
      }
     
      //Reinitialisation of used variables
      index = 0;
      word = 1;
      for(int i=0; i<5; i++){
        command[i]='\0';
      }
      for(int i=0; i<12; i++){
        arg[i]='\0';
      }
     
    }	
  
  
}


void (*entry_shell)() = &shell;

