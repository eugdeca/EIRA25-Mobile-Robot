/*Gripper has a dedicated thread. It is present a queue to handle multiple command sent to the gripper
(FIFO) but this queue has room for maximum 10 consecutive commands (chosen in the Utilities).
The battery is discharged 3.7% every second the base moves. */
#include "Gripper.h"

void* executeGripper(void* arg){     // Help function just to run the thread
    Gripper* gripper = (Gripper*)arg; 
    
    // Call method
    gripper->execute();
        
    pthread_exit(nullptr);
}

Gripper::Gripper(Battery* battery){
    this->battery = battery;
    this->running = true;
    Utilities::inic(this->buffer);
    this->gripState = 0;
    this->dischargeAmount = 3.7f;

    this->buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
    this->not_empty = PTHREAD_COND_INITIALIZER;
    pthread_create(&gripperThread, nullptr, executeGripper, this);
}

Gripper::~Gripper(){
    pthread_mutex_lock(&buffer_mutex);
    running = false;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&buffer_mutex);

    pthread_join(gripperThread, nullptr);

    pthread_mutex_destroy(&buffer_mutex);
    pthread_cond_destroy(&not_empty);
}

void Gripper::execute(){
     while (running) {
        pthread_mutex_lock(&buffer_mutex);
        short int state = this->gripState;
        Utilities::queue& buffer = this->buffer;         

        while (Utilities::empty(buffer) && running){
            cout << "There are not commands for the gripper..." << endl;
            pthread_cond_wait(&not_empty, &buffer_mutex);
        }
        if (!running || this->battery->getStop()) {         // When the destroyer is called this is essential to finish the thread
            pthread_mutex_unlock(&buffer_mutex);
            break; 
        }
        char command;
        Utilities::esqueue(buffer, command);
        
        pthread_mutex_unlock(&buffer_mutex);

        switch (command)
        {
        case 'o':
        case 'O':                                    // Command O to open gripper
            if (state == 0){
                this->openGripper();                                             
                // pthread_cond_signal(&not_full); 
            }
            break;
        case 'c':
        case 'C':                                    // Command C to close the gripper
            if (state == 1){
                this->closeGripper();                                             
                // pthread_cond_signal(&not_full);
                
            }
            break;
        default:
            break;
        }
    }
}

void Gripper::openGripper(){
    battery->discharge(this->dischargeAmount);
    Utilities::sleepMs(3000);
    this->gripState = 1;
    cout << "Gripper opened..." << endl;
}

void Gripper::closeGripper(){
    battery->discharge(this->dischargeAmount);
    Utilities::sleepMs(1000);
    this->gripState = 0;
    cout << "Gripper closed..." << endl;
}

bool Gripper::addCommand(char cmd){  // Method used to add a command to the queue
    pthread_mutex_lock(&buffer_mutex);
    Utilities::queue& buffer = this->buffer;

    if (Utilities::full(buffer)){
        pthread_mutex_unlock(&buffer_mutex);
        cout << "Robot can process maximum 10 commands for " <<
        "the gripper at a time, wait a bit and try again..." << endl;
        return false;
    }
    else {
        Utilities::insqueue(buffer, cmd);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&buffer_mutex);
        return true;
    }
}

short int Gripper::getGripState(){return this->gripState;}

pthread_mutex_t& Gripper::getBuffer_mutex(){return buffer_mutex; }                            
pthread_cond_t& Gripper::getCond_notEmpty() {return not_empty; }

