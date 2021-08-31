#include <stdio.h>   
#include <stdlib.h>   
#include <signal.h>

static int g_exit = 0;
void func(int sig) {  
    printf("Signal deal!\n");  
    printf("Signal was stop by ctrl+c!\n"); 
    g_exit = 1;
    //exit(0);
}  
  
int main() {  
    signal(SIGINT,func);  
  
    while(1){
        printf("signal run\n");
        sleep(1);  
        if (g_exit == 1)
           break;
    }  
      
    printf("signal exit\n");
    return 0;
}  