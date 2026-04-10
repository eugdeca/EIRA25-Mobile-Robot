/*The moving base has a dedicated thread. In this part there are not mutex because the reading and 
the writing of the position and other attributes is considered atomic and this part is the only one 
to have acces to the position angle and speed data. 
The battery is discharged 5.3% every second the base moves.*/
#include "MovingRobot.h"

void* Move(void* arg){                    // Help function just to run the thread
    MovingRobot* base = (MovingRobot*)arg; 
    
    // Call method
    base->MoveBase();
        
    pthread_exit(nullptr);
}

MovingRobot::MovingRobot(Battery* battery, Environment* env){
    this->dischargeAmount = 5.3f;
    this->battery = battery;
    this->env = env;
    this->running = true;         // Used when the robot has to shut down its parts
    this->data.pos[0] = env->getWidth()/2;
    this->data.pos[1] = env->getHeight()/2;
    this->data.angle = 0.0f;
    this->data.speed = 0.0f;
    this->data.maxSpeed = 5.0f; 

    pthread_create(&movingThread, nullptr, Move, this);
}

MovingRobot::MovingRobot(float x, float y, Battery* battery, Environment* env){
    this->dischargeAmount = 5.3f;
    this->battery = battery;
    this->env = env;
    this->running = true;        // Used when the robot has to shut down its parts
    this->data.pos[0] = x;
    this->data.pos[1] = y;
    this->data.angle = 0.0f;
    this->data.speed = 0.0f;
    this->data.maxSpeed = 5.0f; 

    pthread_create(&movingThread, nullptr, Move, this);
}

MovingRobot::~MovingRobot(){
    running = false;           // Used to shut down the base
    pthread_join(movingThread, nullptr);
}

void MovingRobot::MoveBase(){
    bool stringFlag = true;   // To print and tell to the user that the robot is still just once
    while (running && !this->battery->getStop()) {
        float angleRad = Utilities::degToRad(this->data.angle);          // Deg to rad for sin and cos functions
        float speed = this->data.speed;
        if (speed < 0.0001f){
            if (stringFlag){
                cout << "Speed of the robot is 0.0, so it is not moving... " << endl;
                stringFlag = false;
            }
            Utilities::sleepMs(1000);
        }
        else {
            bool collision = false;
            float xMax = (this->env->getWidth());
            float yMax = (this->env->getHeight());

            float currX = this->data.pos[0] - sin(angleRad)*speed;
            float currY = this->data.pos[1] + cos(angleRad)*speed;

            if (currX > xMax || currX < 0 || currY < 0 ||
                currY > yMax){
                    collision = true;
            }
            if (!collision){
                //In case the robot has been shut down
                if (!battery->discharge(this->dischargeAmount)) break;

                Utilities::sleepMs(1000);
                this->data.pos[0] = currX;
                this->data.pos[1] = currY;
            }
            else {
                this->data.speed = 0.0f;
                cout << "The robot will hit the wall in a second. Its position has not been updated and " <<
                "speed has been set to 0. Current pos: (" << this->data.pos[0] << "," << 
                this->data.pos[1]<< ")" << endl;
            } 
            stringFlag = true;           
        }
    }
}

float MovingRobot::getX(){return this->data.pos[0];}
float MovingRobot::getY(){return this->data.pos[1];}
float MovingRobot::getAngle(){return this->data.angle;}
float MovingRobot::getSpeed(){return this->data.speed;}

void MovingRobot::setSpeedPercentage(int SpeedPercentage){
    if (SpeedPercentage < 0 || SpeedPercentage > 100) {
        cout << "Inserted speed percentage value is not valid [0% - 100%]." << endl;
        return;
    }
    this->data.speed = (float)SpeedPercentage * this->data.maxSpeed / 100; // Real speed
}

void MovingRobot::setAngle(float angle){
    if (angle < 0 || angle > 360) cout << "Inserted angle value is not valid [0°-360°]." << endl;
    else this->data.angle = angle;
}
      