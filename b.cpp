#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>


int fdAB, fdCB;

int main(){

    fdAB = open("tempAB", O_RDWR);
    fdCB = open("tempCB", O_RDWR);

    if (fdAB < 0 || fdCB < 0){
		printf("Error while opening a file\n");
		return 2;
	}

    ftruncate(fdAB, 2 * sizeof(int));
    ftruncate(fdCB, 2 * sizeof(int));

    int *ptrAB;
    int *ptrCB;
    ptrAB = (int*)mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fdAB, 0);
    ptrCB = (int*)mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fdCB, 0);

    if (ptrCB == MAP_FAILED || ptrAB == MAP_FAILED){
        printf("Error while memory map created\n");
        return 3;
	}

    printf("Programm B started\n");

    while (true){
        while (ptrAB[0] == 0){
        }
        if (ptrAB[0] == 1){
            printf("B: from A: %d\n", ptrAB[1]);
            ptrAB[0] = 0;
        }
        while (ptrCB[0] == 0){

        }
        if (ptrCB[0] == 1){
            printf("B: from C: %d\n", ptrCB[1]);
            ptrCB[0] = 0;
        }
        if (ptrAB[0] == 2 && ptrCB[0] == 2){
            break;
        }
    }
    
    if (close(fdCB) == -1 || close(fdAB) == -1){
		printf("Error while closing a files\n");
		return 17;
    }

    munmap(ptrCB, 2 * sizeof(int));
    munmap(ptrAB, 2 * sizeof(int));

    return 0;
}
