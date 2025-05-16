#include "os.h"

int main(){
    uint64_t cur_time = 0;
    while(1){
        cur_time = sys_gettime();
        printf("current_time:%x\n",cur_time);
    }
    return 0;
}