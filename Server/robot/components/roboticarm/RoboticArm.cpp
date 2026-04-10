/*The arm has a dedicated thread that waits for a target tilt to be set. If it recives a goel while
a previous one is being processed it ignores it: you can controll tilt only when the arm is still.
The battery is discharged 8% every second the arm moves.*/
#include "RoboticArm.h"

void* executeArm(void* arg){           // Help function just to run the thread
    RoboticArm* arm = (RoboticArm*)arg; 
    
    // Call method
    arm->execute();
        
    pthread_exit(nullptr);
}

RoboticArm::RoboticArm(Battery* battery){
    this->battery = battery;
    this->running = true;
    this->tilt = 0.0f;
    this->targetTilt = -1;         // At the beginning no target tilt is selected
    this->dischargeAmount = 8.0f;
    this->isMoving = false;

    this->arm_mutex = PTHREAD_MUTEX_INITIALIZER;
    this->new_target = PTHREAD_COND_INITIALIZER;
    pthread_create(&armThread, nullptr, executeArm, this);
}

RoboticArm::~RoboticArm(){
    pthread_mutex_lock(&arm_mutex);
    running = false;                    // This is used to stop the thread
    pthread_cond_broadcast(&new_target); // Used to wake up the thread waiting for a target 
    pthread_mutex_unlock(&arm_mutex);

    pthread_join(armThread, nullptr);

    pthread_mutex_destroy(&arm_mutex);
    pthread_cond_destroy(&new_target);
}

bool RoboticArm::moveToTilt(float targetTilt){       //Function to add a target tilt
    if (targetTilt < 0.0f || targetTilt > 90.0f){
        cout << "Target tilt desired is out of bounds (0°, 90°)..." << endl;
        return false;
    }
    pthread_mutex_lock(&arm_mutex);
    if (!this->isMoving){         // Arm has to be still to change its target
        this->targetTilt = targetTilt;
        this->isMoving = true;
        pthread_cond_signal(&new_target);
        pthread_mutex_unlock(&arm_mutex);
        return true;
    }
    // If a new target tilt is set while a previous one is being handled it is ignored
    else {
        float diff = this->tilt - this->targetTilt;
        if (diff < 0) diff = -diff;                           // To be sure the difference is positive 
        int remainingTime = (int)(diff / 10) + 1;

        cout << "The arm is already working on reaching the previus target tilt, please" <<
        " wait untill it finishes: " <<  remainingTime << "s more or less" << endl;
        pthread_mutex_unlock(&arm_mutex);
        return false;
    }

}

void RoboticArm::execute(){
    while(running && !this->battery->getStop()){
        pthread_mutex_lock(&arm_mutex);

        while ((this->tilt == this->targetTilt || this->targetTilt == -1) && running){   // Check if the target has changed
            pthread_cond_wait(&new_target, &arm_mutex);                                  // otherwise wait for the change
        }
        if (!running) {                                // When the arm is destroyed
            pthread_mutex_unlock(&arm_mutex);
            break;
        }

        // Tilt update is handled secon per second
        while (this->tilt != this->targetTilt && this->targetTilt != -1 && running){      // Core loop
            battery->discharge(this->dischargeAmount);
            
            if (!running || this->battery->getStop()) {    // When the arm is destroyed
                break;
            }

            pthread_mutex_unlock(&arm_mutex);
            Utilities::sleepMs(1000);
            pthread_mutex_lock(&arm_mutex);

            if (!running) {                                // When the arm is destroyed
                break;
            }

            if (this->tilt > this->targetTilt && (this->tilt - this->targetTilt) >= 10){
                this->tilt = this->tilt - 10;
            }
            
            else if(this->tilt < this->targetTilt && (this->targetTilt - this->tilt) >= 10){
                this->tilt = this->tilt + 10;
            }
            else if ((this->tilt - this->targetTilt) < 10){
                this->tilt = this->targetTilt;
            }
        }
        if (running && !this->battery->getStop()){  // When the robot is destroyed the string should not be shown 
            cout << "Desired tilt reached..." << endl;
        }
        this->isMoving = false; 
        pthread_mutex_unlock(&arm_mutex);          
    }
}

float RoboticArm::getTilt(){return this->tilt;}
float RoboticArm::getTargetTilt(){return this->targetTilt;}
void RoboticArm::setTilt(float tilt){
    if (tilt < 0.0f || tilt > 90.0f){
        cout << "Target tilt desired is out of bounds (0°, 90°)..." << endl;
    }
    else {
        pthread_mutex_lock(&arm_mutex);
        this->tilt = tilt;
        pthread_mutex_unlock(&arm_mutex);
    }
}