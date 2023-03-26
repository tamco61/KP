#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>


int fdAB, fdAC;


int main(){

    std::cout << "Programm A started\n";


    // work with file
    fdAB = open("tempAB", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fdAB == -1)
    {
		std::cout << "Error while opening a file\n";
		return 2;
	}
    ftruncate(fdAB, 2 * sizeof(int));

    fdAC = open("tempAC", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fdAC == -1)
    {
        std::cout << "Error while opening a file\n";
        return 2;
    }
    ftruncate(fdAC, 1025 * sizeof(char));


    // display in RAM
    int *ptrAB;
    ptrAB = (int *)mmap(NULL, 2 * sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fdAB, 0);
    if (ptrAB == MAP_FAILED)
    {
        std::cout << "Error while memory map created\n";
        return 3;
    }
    ptrAB[0] = 0;

    char *ptrAC;
    ptrAC = (char *)mmap(NULL, 1025 * sizeof(char), PROT_WRITE | PROT_READ, MAP_SHARED, fdAC, 0);
    if (ptrAC == MAP_FAILED)
    {
        std::cout << "Error while memory map created\n";
        return 3;
	}
    ptrAC[0] = 0;


    // create B process
    int idB = fork();
    
    if (idB == -1)
    {
        std::cout << "Fork raise error\n";
        return 4;
    } 
    else if (idB == 0)
    {
        // B process
        if (execl("b.out", "b.out", NULL, (char *)NULL) == -1)
        {
            std::cout << "Error in executing b.cpp\n";
            return 5;
        }

    } 
    else 
    {
        // create C process
        int idC = fork();

        if (idC < 0)
        {
            std::cout << "Fork raise error\n";
            return 4;
        } 
        else if (idC == 0)
        {
            // C process    
            if (execl("c.out", "c.out", NULL, (char *)NULL) == -1)
            {
                std::cout << "Error in executing c.cpp\n";
                return 6;
            }

        } 
        else 
        {
    
            char c;
            int count = 0, reading;

            while (true){
                while (ptrAC[0] == 1){
                
                }
                while (ptrAB[0] == 1){
                
                }
                reading = scanf("%c", &c);
                if (reading > 0){
                    if (c != '\n'){
                        ptrAC[count + 1] = c;
                        count++;
                    } else {
                        ptrAC[count + 1] = c;
                        ptrAC[0] = 1;

                        ptrAB[1] = count;
                        ptrAB[0] = 1;

                        count = 0;
                    }
                } 
                else {
                    ptrAB[0] = 2;
                    ptrAC[0] = 2;
                    break;
                }
            }

            if (close(fdAB) == -1 || close(fdAC) == -1){
		        std::cout << "Error while closing a files\n";
		        return 17;
            }

            munmap(ptrAB, 2 * sizeof(int));
            munmap(ptrAC, 1025 * sizeof(char));

            std::cout << "Programm stopped\n";

        }

    }

    return 0;

}
