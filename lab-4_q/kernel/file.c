#include "file.h"

int id;
int root = 0;
int currentDirectory = 1;
addressBlock b;

int returnCurrentDirectory(){
  return currentDirectory;
}

void rst(){
  id = 2;

  fileMetadata f;
  f.type = 2;
  f.fileName[0] = 'h';
  f.fileName[1] = 'o';
  f.fileName[2] = 'm'; 
  f.fileName[3] = 'e';
  f.dataAdress = 1;
 
  const uint8_t* x = (uint8_t*)&f;
  disk_wr( 0, x, 16);

  b.noAdresses = 0;
  const uint8_t* y = (uint8_t*)&b;
  disk_wr( 1, y, 16);
  
}

int alreadyExists(char name[12]){

  uint8_t* x;
  disk_rd(currentDirectory, x, 16);

  char nameFiles[12];
  int found = 1;
  for(int i=0; i<12; i++){
    if(name[i] != x[i+2]){
      found = 0;
    } 
  }

  return found;
}

int create(char name[12],int type){  
           
  b.address[ b.noAdresses ] = id;
  b.noAdresses = b.noAdresses + 1;
  const uint8_t* x = (uint8_t*)&b;
  disk_wr( 1, x, 16);

  fileMetadata f;
  f.type = type;
  for(int i=0; i<=11; i++){
    f.fileName[i] = name[i];
  }
  f.dataAdress = id + 1;
 
  const uint8_t* y = (uint8_t*)&f;
  disk_wr( id, y, 16);
  id = id + 2;

  return id-2;
}





