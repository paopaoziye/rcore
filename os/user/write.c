#include "os.h"
int main(){
    const char *message = "task write is running!\n";
    while (1){
        printf(message);
    }
    return 0;
}