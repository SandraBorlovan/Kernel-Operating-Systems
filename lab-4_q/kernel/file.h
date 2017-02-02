#include <stdint.h>

typedef struct{
  uint16_t noAdresses;
  uint16_t address[6];
  uint16_t blockAdress;
}addressBlock;

typedef struct {
  uint16_t type;            // type =1 for file and type =2 for directory
  char fileName[12];
  uint16_t dataAdress;   
}fileMetadata ;

int returnCurrentDirectory();

void rst();

int create(char name[12],int type);

int alreadyExists(char name[12]);

