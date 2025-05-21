#include "os.h"

int main(){
    int pid = sys_fork();
    while(1){
        if(pid > 0){
            //printf("father\n");
        }else if(pid == 0){
            //printf("child\n");
        }else{

        }
    }
    return 0;
}
