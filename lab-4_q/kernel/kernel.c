#include "kernel.h"

pcb_t pcb[50], *current = NULL;
int typeScheduler = 2;         //1 for priority scheduler and 2 for roundRobinScheduler
int nopcb = 3;                 //The maximum id of a process so far = no of processes
int currentProcess = 0;        //The index of the current pcb
int priorityOfCurrentProcess;  //Stores the priority of the current runnig process
int memoryEmpty = 1;           //Set to 1 if the disk has been emptied

int buffer[3][3] = {{-1,-1,-1},{-1,-1,-1},{-1,-1,-1}};  //Retains information interchanged by processes

/*********************************************
 Returns an pid for the next initiated pcb
that will also be its position in ths pcb array
**********************************************/
pid_t newId(){
  int id = -1;
  for(int i = 0; i < nopcb; i++){
    if(pcb[i].pid == -1){
      id = i;
      break;	
    }
  }
  if(id == -1){
    id = nopcb;
    nopcb++;
  }	
	
  return id;
}

/*********************************************
 A schedule that takes account of the priority
of each process choosing to run next the process
with the highest priority
**********************************************/
void priorityScheduler( ctx_t* ctx){
  int id = 0;
  int arriveTime = pcb[0].arriveTime;
  int priority = pcb[0].priority; 


  //Goes through existant processes and selects the one with higher priority
  for(int i = 1; i < nopcb; i++){
    if(pcb[i].priority > priority || ( pcb[i].priority == priority && pcb[i].arriveTime < arriveTime) && pcb[i].pid !=-1){
        id = i;
        arriveTime = pcb[i].arriveTime;
        priority = pcb[i].priority;
    }
  } 
  //Solves starvation by introducing aging
  for(int i = 0; i < nopcb; i++){
    if(i != id){
      pcb[i].priority = pcb[i].priority + 1;
    }
  }

  priorityOfCurrentProcess = priority;
  memcpy( ctx, &pcb[ id ].ctx, sizeof( ctx_t ) );
  current = &pcb[ id ];
  currentProcess = id;
}

void roundRobinScheduler( ctx_t* ctx) {
  
    if(currentProcess == nopcb-1){
      memcpy( ctx, &pcb[ 0 ].ctx, sizeof( ctx_t ) );
      current = &pcb[ 0 ];
      currentProcess = 0;
    }else{
      memcpy( &pcb[currentProcess].ctx, ctx, sizeof( ctx_t ) );
      memcpy( ctx, &pcb[ currentProcess + 1 ].ctx, sizeof( ctx_t ) );
      current = &pcb[ currentProcess + 1 ];
      currentProcess = currentProcess + 1;
    }
    //In case the current selected process has been exited the scheduler will be recalled           
    if(pcb[currentProcess].pid == -1){
      roundRobinScheduler(ctx);
    }
  
}


void kernel_handler_rst( ctx_t* ctx ) {
 /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *    
   * - the CPSR value of 0x50 means the processor is switched into USR 
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack. 
   */
 
  
  write(0,"Entered rst\n",13);
  for(int i= 0; i < nopcb;i++){
    memset( &pcb[ i ], 0, sizeof( pcb_t ) );
    pcb[ i ].pid      = i;
    pcb[ i ].ctx.cpsr = 0x50;
  }

  pcb[ 0 ].ctx.pc     = ( uint32_t )( entry_P0 );
  pcb[ 0 ].ctx.sp     = ( uint32_t )(  &tos_P0 );
  pcb[ 0 ].ctx.tos    = ( uint32_t )(  &tos_P0 );
  pcb[ 0 ].arriveTime = 0;
  pcb[ 0 ].priority   = 8;
  pcb[ 0 ].noFiles   = 0;

  pcb[ 1 ].ctx.pc        = ( uint32_t )( entry_P1 );
  pcb[ 1 ].ctx.sp        = ( uint32_t )(  &tos_P1 );
  pcb[ 1 ].ctx.tos       = ( uint32_t )(  &tos_P1 );
  pcb[ 1 ].arriveTime    = 0;
  pcb[ 1 ].priority      = 3;
  pcb[ 1 ].noFiles   = 0;
  
  pcb[ 2 ].ctx.pc        = ( uint32_t )( entry_P2 );
  pcb[ 2 ].ctx.sp        = ( uint32_t )(  &tos_P2 );
  pcb[ 2 ].ctx.tos       = ( uint32_t )(  &tos_P2 );
  pcb[ 2 ].arriveTime    = 0;
  pcb[ 2 ].priority      = 5;
  pcb[ 2 ].noFiles   = 0;
  
  
  /*
  memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
  pcb[ 0 ].pid      = 0;
  pcb[ 0 ].ctx.cpsr = 0x50;
  pcb[ 0 ].ctx.pc     = ( uint32_t )( entry_shell );
  pcb[ 0 ].ctx.sp     = ( uint32_t )(  &tos_shell );
  pcb[ 0 ].ctx.tos    = ( uint32_t )(  &tos_shell );
  pcb[ 0 ].arriveTime = 0;
  pcb[ 0 ].priority      = 10;
  pcb[ 0 ].noFiles   = 0;
  */

  if(memoryEmpty){
    rst();
  }
  
  /*  Once the PCBs are initialised, in case we are using the 
   *  roundRobinScheduler we (arbitrarily) select one to be
   *  restored and executed as our current process. Otherwise
   *  we call the priorityScheduler that will choose the current 
   *  process to be the one with the highest priority.
   */

  if(typeScheduler == 1){
    priorityScheduler(ctx);
  }else if(typeScheduler == 2){
    current = &pcb[ 0 ]; 
    memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
  }
  

  /* Configure the mechanism for interrupt handling by
   *
   * - configuring timer st. it raises a (periodic) interrupt for each 
   *   timer tick,
   * - configuring GIC st. the selected interrupts are forwarded to the 
   *   processor via the IRQ interrupt signal, then
   * - enabling IRQ interrupts.
   */
   

  TIMER0->Timer1Load     = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl    |= 0x00000080; // enable          timer

  GICC0->PMR             = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER[ 1 ] |= 0x00001010; // enable timer          interrupt
  GICC0->CTLR            = 0x00000001; // enable GIC interface
  GICD0->CTLR            = 0x00000001; // enable GIC distributor

  irq_enable();

  return;
}

/***********************************************************
  Trigers interrupts that determine context switches in the
case of roundRobinScheduler or verificatons whether the 
current process has the highest priority in case of a 
priority based scheduler
************************************************************/

void kernel_handler_irq(ctx_t* ctx) {

  uint32_t id = GICC0->IAR;

  
  if( id == GIC_SOURCE_TIMER0) {
    if(nopcb != 1){
      if(typeScheduler == 2){
        roundRobinScheduler( ctx );
      }else if(typeScheduler == 1){
        priorityScheduler( ctx );
      }
    }
    TIMER0->Timer1IntClr = 0x01;
  }


  GICC0->EOIR = id;
}

void kernel_handler_svc( ctx_t* ctx, uint32_t id ) { 
  /* Based on the identifier encoded as an immediate operand in the
   * instruction:
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */


  switch( id ) {
    case 0x01 : {   // write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );  
      char*  x = ( char* )( ctx->gpr[ 1 ] );  
      int    n = ( int   )( ctx->gpr[ 2 ] ); 

      for( int i = 0; i < n; i++ ) {
        PL011_putc( UART0, *x++ );
      }
      
      ctx->gpr[ 0 ] = n;
      break;
    }
    case 0xC : {  //writeno(uint32_t no)
      uint32_t   no = (uint32_t)( ctx->gpr[ 0 ] ); 
      uint32_t x = no;
      uint32_t divisor= 10;
  
      if(no == 0){
        PL011_putc( UART0, ('0') );
      }else{
        while( no != 0){
          no = no/10;
          divisor = divisor * 10;
        }
        divisor = divisor/100;
        no = x;
        while(divisor >= 1) {
          PL011_putc( UART0, ('0'+ (no/divisor)%10) );
          divisor = divisor/10;
       }
     }

     ctx->gpr[ 0 ] = 1;
     break;
    }
    case 0x02 : {   // read(fd, buf, nbytes)
      int   fd = ( int   )( ctx->gpr[ 0 ] );  
      char* buf = ( char* )( ctx->gpr[ 1 ] );  
      int nbytes = ( int   )( ctx->gpr[ 2 ] ); 

      for( int i = 0; i < nbytes; i++ ) {
         buf[i] = PL011_getc(UART0);
      }

      ctx->gpr[ 0 ] = nbytes;
      break;
    }
    case 0x03 : {   //fork()
      
      pid_t id = newId();
      int offset = ctx->sp - ctx->tos; 
      pcb[id].pid  = id;
      memcpy( &pcb[id].ctx, ctx, sizeof( ctx_t ) );
      pcb[id].ctx.tos  = ( uint32_t )(  pcb[0].ctx.tos + id* 0x00001000);
      pcb[id].ctx.sp   = ( uint32_t )(  pcb[id].ctx.tos + offset);
      //pcb[id].priority = ;
     
      ctx->gpr[ 0 ] = pcb[id].pid;
      pcb[id].ctx.gpr[0] = 0;
 	     
      memcpy( &(pcb[id].ctx.sp), &(ctx->sp), offset );
       
      //roundRobinScheduler( ctx );
      //Calling the priorityScheduler(ctx) when the new created process has higher priority 
      //than the current process
      
      /*if(pcb[id].priority > priorityOfCurrentProcess){
        priorityScheduler(ctx);
      }*/
	
      break;
      
    }
    case 0x04:{ //_exit()
      
      pcb[currentProcess].pid = -1;
      if(typeScheduler == 2){
      	roundRobinScheduler( ctx );
      }else{
        priorityScheduler(ctx);
      }
      
      ctx->gpr[ 0 ] = 1;
      break;
    }
    case 0x05 : {  //exit(int pid)
      int   pid = ( int )( ctx->gpr[ 0 ] );
     
      int ok = -1;
      uint32_t tos;
      if(pid == 0){
        tos = ( uint32_t )(  &tos_P0 );
      }else if(pid == 1){
        tos = ( uint32_t )(  &tos_P1 );
      }else if(pid == 2){
        tos = ( uint32_t )(  &tos_P2 );
      }

      for(int i = 0; i < nopcb; i++){
        if(pcb[i].ctx.tos == tos){
          pcb[i].pid = -1;
          ok = 1;
        }
      }
      ctx->gpr[ 0 ] = ok;
      break;
    }
    case 0x06:{//execute(idProcess)
      int   idProcess = ( int )( ctx->gpr[ 0 ] );

      if(idProcess == 0){
        pcb[ currentProcess ].ctx.pc     = ( uint32_t )( entry_P0 );
        pcb[ currentProcess ].ctx.tos    = ( uint32_t )(  &tos_P0 );
        pcb[ currentProcess ].ctx.sp     = ( uint32_t )(  &tos_P0 );
        //pcb[ currentProcess ].arriveTime = ;
        pcb[ currentProcess ].priority   = 8;
        pcb[ currentProcess ].noFiles   = 0;
      }else if(idProcess == 1){
        pcb[ currentProcess ].ctx.pc     = ( uint32_t )( entry_P1 );
        pcb[ currentProcess ].ctx.tos    = ( uint32_t )(  &tos_P1 );
        pcb[ currentProcess ].ctx.sp     = ( uint32_t )(  &tos_P1 );
        //pcb[ currentProcess ].arriveTime = ;
        pcb[ currentProcess ].priority   = 3;
        pcb[ currentProcess ].noFiles   = 0;
      }else if(idProcess == 2){
        pcb[ currentProcess ].ctx.pc     = ( uint32_t )( entry_P2 );
        pcb[ currentProcess ].ctx.tos    = ( uint32_t )(  &tos_P2 );
        pcb[ currentProcess ].ctx.sp     = ( uint32_t )(  &tos_P2 );
        //pcb[ currentProcess ].arriveTime = ;
        pcb[ currentProcess ].priority   = 5;
        pcb[ currentProcess ].noFiles   = 0;
      }
      
      ctx->gpr[ 0 ] = 1;
      break;
    }
    case 0x07 : {  //require(pid)
       int   idProcess = ( int   )( ctx->gpr[ 0 ] );  
      
       int info = buffer[idProcess][currentProcess];
       buffer[idProcess][currentProcess] = -1;
      
       ctx->gpr[ 0 ] = info;  
       break;
    }
    
    case 0x08 : {   //send(pid,x)
       int  idProcess = ( int   )( ctx->gpr[ 0 ] );
       uint32_t info = (  uint32_t  )( ctx->gpr[ 1 ] );

       buffer[currentProcess][idProcess] = info; 
          
       ctx->gpr[ 0 ] = 1;
       break;
    }
    case 0x09 : {  //creat(char name[12],int flag, int type)
      
      char* name = ( char*)( ctx->gpr[ 0 ] );  
      int flag = ( int )( ctx->gpr[ 1 ] );  
      int type = ( int )( ctx->gpr[ 2 ] ); 

      int block;
      //if(alreadyExists(name) == 0){
        block = create(name,type);
      /*}else{
        PL011_putc( UART0, 'F');
      }*/

      ctx->gpr[ 0 ] = block;
      break;
    }
    case 0xA : {  //  open(char name[12],int flag){
      
      char* name = ( char*)( ctx->gpr[ 0 ] );  
      int flag = ( int )( ctx->gpr[ 1 ] );  
 
      pcb[currentProcess].fd[ pcb[currentProcess].noFiles ].nameFile[12] = name[12];
      pcb[currentProcess].fd[ pcb[currentProcess].noFiles ].blockAdress = returnCurrentDirectory();
      pcb[currentProcess].fd[ pcb[currentProcess].noFiles ].rwPointer = 0;
      pcb[currentProcess].fd[ pcb[currentProcess].noFiles ].flag = flag;

      ctx->gpr[ 0 ] = 1;
      break;
    }
    case 0xB : {  //  close(char name[12]){
      
      char* name = ( char*)( ctx->gpr[ 0 ] );
    
      int found = 1;
      int position = -1;
      for(int i=0; i<pcb[currentProcess].noFiles; i++){
        for(int j=0; j<=11 ; j++){
          if( name[i] != pcb[currentProcess].fd[i].nameFile[j] ){
            found = 0;
            break;
          }
        }
        if(found == 1){
          position = i;
          break;
        }
      } 
      
      int resp;
      if(position != -1){
        for(int i=position ; i<pcb[currentProcess].noFiles -1; i++){
           pcb[currentProcess].fd[i] = pcb[currentProcess].fd[i+1];
        }
        resp = 1;
      }else{
        resp = 0;
      }

      ctx->gpr[ 0 ] = resp; //Returns 1 for a succesfull closure and 0 if the file was not found
      break;
    }
    
    default   : { // unknown
      break;
    }
  }
  
 
  return;
}
