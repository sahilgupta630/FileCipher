#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement() : childrenCount(0) {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
    int pid = fork();
    if(pid < 0){
        return false;
    }else if(pid > 0){
        std::cout<<"Entering the parent process"<<std::endl;
        childrenCount++;
    }else{
        std::cout<<"Entering the child process"<<std::endl; 
        executeTask();
        std::cout<<"Exiting the child process"<<std::endl;
    }
    return true;
}

void ProcessManagement::executeTask() {
    
    if(!taskQueue.empty()){
        std::unique_ptr<Task> task = std::move(taskQueue.front());
        taskQueue.pop();
        std::string taskStr = task->toString();
        
        // Execute the dedicated cryption binary using execl
        if (execl("./cryption", "./cryption", taskStr.c_str(), (char*)NULL) == -1) {
            std::cerr << "Failed to execute crypt binary for task" << std::endl;
            exit(1);
        }
    }

}

void ProcessManagement::waitForAllProcesses() {
    int status;
    for (int i = 0; i < childrenCount; ++i) {
        wait(&status);
    }
    std::cout << "All background processes finished." << std::endl;
}