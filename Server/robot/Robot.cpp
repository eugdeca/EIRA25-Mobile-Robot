/*The robot has a thread to charge the battery and as many threads as the maximum number of controllers
allowed to controll it (this number is chosen by the user in the constructor). The first thread
starts with the robot, the other ones start when startService is called.*/
#include "Robot.h"

void* startChargeThread(void* arg){
    Robot* robot = (Robot*)arg; 
    
    // Call method
    robot->Charge();
        
    pthread_exit(nullptr);
}

// Following struct used to pass arguments to the thread
struct SockThreadData {
    Robot* robotInstance;
    int threadID;
};

void* startSockThreads(void* arg){
    SockThreadData* data = (SockThreadData*)arg;
    
    // Extract data
    Robot* robot = data->robotInstance;
    int id = data->threadID;

    delete data; 

    // 4. Chiamiamo il metodo passando l'ID
    robot->runController(id);
        
    pthread_exit(nullptr);
}

Robot::Robot(Environment* env){
    this->battery = new Battery;                               // Initialize the components in the heap
    this->gripper = new Gripper(battery);
    this->movingBase = new MovingRobot(battery, env);
    this->arm = new RoboticArm(battery);
    this->env = env;                                           // Env is not owned by the robot
    this->kill = false;                                        // Kill in the destroyer
    this->shutDown = false; 
    this->remoteEnabled = true; 
    this->MAX = 5;
    this->sockThreads = new pthread_t[MAX];                                           

    this->robotMutex = PTHREAD_MUTEX_INITIALIZER;
    this->acceptMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_create(&chargerThread, nullptr, startChargeThread, this);     // Create harvest thread
}

Robot::Robot(Environment* env, int MAX){
    this->battery = new Battery;                               // Initialize the components in the heap
    this->gripper = new Gripper(battery);
    this->movingBase = new MovingRobot(battery, env);
    this->arm = new RoboticArm(battery);
    this->env = env;                                           // Env is not owned by the robot
    this->kill = false;                                        // Kill in the destroyer
    this->shutDown = false; 
    this->remoteEnabled = true; 
    this->MAX = MAX;
    this->sockThreads = new pthread_t[this->MAX];                                           

    this->robotMutex = PTHREAD_MUTEX_INITIALIZER;
    this->acceptMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_create(&chargerThread, nullptr, startChargeThread, this);     // Create harvest thread
}

Robot::~Robot(){
    this->shutDown = true;                             // Destroy the thread immediatelly, I am quitting the program, 
    this->battery->setStop(true);                      // Unlocks threads stuck in discharge
    pthread_cond_signal(&battery->getCond_discharged());  // Wakes up Robot charger thread
    pthread_cond_broadcast(&battery->getCond_charged());  // Wakes up threads stuck in discharge
    shutdown(this->RobotSock, SHUT_RDWR);              // Found this command online to wake up the accept   
    close(this->RobotSock);                            
    
    // Main waits for all the threads to stop          
    for (int i = 0; i < MAX; ++i) {
        pthread_join(sockThreads[i], nullptr);
    } 
    
    delete gripper;                                    // Delete from heap
    delete movingBase;
    delete arm;

    pthread_join(chargerThread, nullptr);                      
    
    pthread_mutex_destroy(&robotMutex);
    pthread_mutex_destroy(&acceptMutex);

    delete[] this->sockThreads;
    delete battery;  
    cout << "--- END ROBOT DESCTRUCTION ---" << endl;                                          
}

/*============= CHARGING HANDLING METHODS=======================*/
/* The following thread waits the battery to be discharged before starting to charge it and
it goes back to waiting when the battery is full*/
void Robot::Charge(){
    enum chargingSource { NONE, SUN, WIND };

    while (true && !this->shutDown){
        pthread_mutex_lock(&battery->getBatt_mutex());

        // Wait for the signal from the battery to charge
        while (!battery->getBatteryDischarged() && !this->shutDown){
            pthread_cond_wait(&battery->getCond_discharged(), &battery->getBatt_mutex());
        }

        float solarHarvest[2];         // {Power, total time}  
        float windHarvest[2];          // {Power, total time}

        bool charging = true;
        chargingSource currentMethod = NONE;
        // This loop allows the charger to be sensible to environment changes
        while (charging && !this->shutDown){ 
            this->solarPanel(battery->getLevel(), solarHarvest);
            this->windTurbine(battery->getLevel(), windHarvest);
            if (battery->getLevel()==100){
                break;
            }
            // Chose which method to use
            while (solarHarvest[0] == -1 && windHarvest[0] == -1 && !this->shutDown){
                // There is no sun and no wind, it is impossible to charge
                cout << "it is impossible to charge the battery, there is no sun and no wind. Waiting 5s for changes..."
                << endl;
                currentMethod = NONE;
                // Wait new environment settings, unlock the mutex because it is a slow process
                
                for(int i=0; i<50; i++){
                    if(this->shutDown) break; // If there is a shut down I break before the 5 secs 
                    Utilities::sleepMs(100);
                }

                // Lock the mutex again
                // Try again hoping the environment changed
                this->solarPanel(battery->getLevel(), solarHarvest);
                this->windTurbine(battery->getLevel(), windHarvest);
            }
            // Choose the fastest, method to recharge 
            if (solarHarvest[1]<=windHarvest[1] && !this->shutDown){
                if (currentMethod != SUN) {
                    cout << "Prepering solar pannels" << endl;
                    Utilities::sleepMs(1000);
                    currentMethod = SUN;
                }
                // The charging method is second per second
                if((100-battery->getLevel())>=solarHarvest[0] && !this->shutDown){     // Charging loop
                    battery->charge(solarHarvest[0]);
                    Utilities::sleepMs(1000);
                    cout << "Charging with sun. " << battery->getLevel() << "%" << endl;
                    battery->print();
                }
                // When the remaining battery to charge is less than the energy per second
                else if (battery->getLevel()<100.0f && !this->shutDown){                      // Remaining energy to charge
                    Utilities::sleepMs((100-battery->getLevel())/solarHarvest[0]*1000);  // time to charge less than power
                    battery->charge(solarHarvest[0]); // The energy in excess is handeld by the battery
                    cout << "Charging with sun. " << battery->getLevel() << "%" << endl;
                    battery->print();
                }
            }
            else if (windHarvest[1]<solarHarvest[1] && !this->shutDown){
                if (currentMethod != WIND) {
                    cout << "Prepering wind turbine" << endl;
                    Utilities::sleepMs(5000);
                    currentMethod = WIND;
                }
                // The charging method is second per second
                if((100-battery->getLevel())>=windHarvest[0] && !this->shutDown){     // Carging loop
                    battery->charge(windHarvest[0]);
                    Utilities::sleepMs(1000);
                    cout << "Charging with wind. " << battery->getLevel() << "%" << endl;
                    battery->print();
                }
                // When the remaining battery to charge is less than the energy per second
                else if (battery->getLevel()<100 && !this->shutDown){                                         // Remaining energy to charge
                    Utilities::sleepMs((100-battery->getLevel())/windHarvest[0]*1000);  // time to charge less than power
                    battery->charge(windHarvest[0]); // The energy in excess is handeld by the battery
                    cout << "Charging with wind. " << battery->getLevel() << "%" << endl;
                    battery->print();
                }
            }
        }
        pthread_cond_signal(&battery->getCond_charged());      // Wake up the battery discharge
        pthread_mutex_unlock(&battery->getBatt_mutex());
    }
}

// Saves in an array power and time needed for full recharge
void Robot::solarPanel(float q, float harvest[2]){   
    float power;
    float K = 2;
    float p = 1.65f;
    float x = this->env->getWeather();
    // Check if there is no sun
    if (x < 0.0001f){
        harvest[0] = -1;
        harvest[1] = 50000000;            // High number to simulate infinite
    }
    else {
        power = K*(pow(x,p)/(pow(x,p) + pow(100-x,p)));
        harvest[0] = power;
        harvest[1] = (100-q)/power + 1;
    }
}

// Saves in an array power and time needed for full recharge
void Robot::windTurbine(float q, float harvest[2]){
    float power;
    float K = 1.5f;
    float p = 0.5f;
    float x = this->env->getWind_Speed();
    // Check if there is no wind
    if (x < 0.0001f){
        harvest[0] = -1;
        harvest[1] = 50000000;            // High number to simulate infinite
    }
    else {
        power = K*pow(x/100,p)*(1-pow(1-x/100,p));
        harvest[0] = power;
        harvest[1] = (100-q)/power + 5;
    }
}

/*============= SOCKET HANDLING METHODS=======================*/

void Robot::startService(){               // Opens the Server socket
    this->RobotSock = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    //inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);  
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(this->RobotSock, (struct sockaddr*)&address, sizeof(address));
    listen(this->RobotSock, 20);

    cout << "Robot service started. Waiting for connections..." << endl;

    cout << "Server started. launching " << MAX << " threads for multicontroller..." << endl;

    // Create as many threads as the maximum number of controller that can connect (chosen by the user)
    for (int i = 0; i < MAX; ++i) {
        // Construct data
        SockThreadData* data = new SockThreadData;
        data->robotInstance = this;
        data->threadID = i + 1;     // Just an incremental number to recognise what controller connected

        if (pthread_create(&sockThreads[i], nullptr, &startSockThreads, data) != 0) {
            cerr << "Error creating thread " << i << endl;
            delete data;
        }
    }
 
}

void Robot::runController(int id){
    while (true && !this->kill) {
        // We are inside one of the 5 threads
        pthread_mutex_lock(&acceptMutex);
        int clientSock = accept(this->RobotSock, nullptr, nullptr);     // Waiting for someone to connect
        pthread_mutex_unlock(&acceptMutex);

        if (clientSock < 0) {
            // If accept return -1 the socket has been closed
            if (this->kill || this->shutDown) {
                // the robot is shutting down
                break; 
            }
            // Otherwise it is an error
            continue; 
        }

        cout << "Client n." << id << " connected." << endl;       // To know who connected
        while (true && !this->kill) {
            // Creating buffer as big as the biggest command to send
            char buffer[sizeof(char) + sizeof(float) + sizeof(uint32_t)];
            int bytesRead = recv(clientSock, buffer, sizeof(buffer), 0);

            if (bytesRead > 0) {
                int pos = 0;

                char opcode;
                memcpy(&opcode, buffer + pos, sizeof(opcode));
                pos += sizeof(opcode);

                // Robot is disabled untill it recives A
                if (!this->remoteEnabled && opcode != 'A') {
                    cout << "Robot is disactivated, your request is ignored..." << endl;
                    continue; 
                }
                pthread_mutex_lock(&this->robotMutex);
                switch (opcode){
                    case 'A':
                    case 'a':
                        this->remoteEnabled = true;
                        cout << "Robot Active..." << endl;
                        break;
                    case 'D':
                    case 'd':
                        this->remoteEnabled = false;
                        this->movingBase->setSpeedPercentage(0);
                        cout << "Robot Disactivated..." << endl;
                        break;
                    case 'S':
                    case 's':
                        this->movingBase->setSpeedPercentage(0);
                        cout << "Robot Stopped..." << endl;
                        break;
                    case 'O':
                    case 'o':
                        this->getGripper()->addCommand('o');
                        break;
                    case 'C':
                    case 'c':
                        this->getGripper()->addCommand('c');                        
                        break;
                    case 'T':
                    case 't': {
                        if (bytesRead < 5) {
                            cout << "Bytes missing in tilt request..." << endl;
                            break;
                        }
                        uint32_t tiltN;       // Network tilt
                        float tilt;           // Host tilt
                        // Extract the tilt 
                        memcpy(&tiltN, buffer + pos, sizeof(tiltN));
                        pos += sizeof(tiltN);
                        tilt = Utilities::ntohf(tiltN);
                        if (this->getArm()->moveToTilt(tilt)){
                            cout << "Target tilt set..." << endl;
                        }
                        else {cout << "Target tilt NOT set..." << endl;}
                        break;
                        }
                    case 'M':
                    case 'm':{
                        if (bytesRead < 9) {
                            cout << "Bytes missing in moving request" << endl;
                            break;
                        }
                        uint32_t angleN;          // Network angle
                        float angle;              // Host angle
                        uint32_t speedPerc;       // Here type is the same for network and host
                        memcpy(&angleN, buffer + pos, sizeof(angleN));
                        pos += sizeof(angleN);
                        angle = Utilities::ntohf(angleN);

                        memcpy(&speedPerc, buffer + pos, sizeof(speedPerc));
                        pos += sizeof(speedPerc);
                        speedPerc = ntohl(speedPerc);

                        this->getBase()->setAngle(angle);
                        this->getBase()->setSpeedPercentage(speedPerc);
                        cout << "Angle and speed set. Speed: " << this->getBase()->getSpeed() <<
                                "[m/s]" << endl;
                        break;
                        }
                    case 'G':
                    case 'g':{
                        cout << "Received state request." << endl;
                        // Preparing the buffer to send back to the controller
                        char sendBuffer[sizeof(float)*5 + sizeof(uint16_t) + sizeof(float)];
                        int sendPos = 0;

                        // Network and Host have different types to send floats
                        float responseX = this->getBase()->getX();
                        uint32_t responseXN = Utilities::htonf(responseX);
                        memcpy(sendBuffer + sendPos, &responseXN, sizeof(responseXN));
                        sendPos += sizeof(responseXN);

                        float responseY = this->getBase()->getY();
                        uint32_t responseYN = Utilities::htonf(responseY);
                        memcpy(sendBuffer + sendPos, &responseYN, sizeof(responseYN));
                        sendPos += sizeof(responseYN);

                        float responseSpeed = this->getBase()->getSpeed();
                        uint32_t responseSpeedN = Utilities::htonf(responseSpeed);
                        memcpy(sendBuffer + sendPos, &responseSpeedN, sizeof(responseSpeedN));
                        sendPos += sizeof(responseSpeedN);

                        float responseAngle = this->getBase()->getAngle();
                        uint32_t responseAngleN = Utilities::htonf(responseAngle);
                        memcpy(sendBuffer + sendPos, &responseAngleN, sizeof(responseAngleN));
                        sendPos += sizeof(responseAngleN);

                        float responseTilt = this->getArm()->getTilt();
                        uint32_t responseTiltN = Utilities::htonf(responseTilt);
                        memcpy(sendBuffer + sendPos, &responseTiltN, sizeof(responseTiltN));
                        sendPos += sizeof(responseTiltN);

                        uint16_t stateGripper = this->getGripper()->getGripState();
                        stateGripper = htons(stateGripper);
                        memcpy(sendBuffer + sendPos, &stateGripper, sizeof(stateGripper));
                        sendPos += sizeof(stateGripper);

                        float stateBattery = this->getBattery()->getLevel();
                        uint32_t stateBatteryN = Utilities::htonf(stateBattery);
                        memcpy(sendBuffer + sendPos, &stateBatteryN, sizeof(stateBatteryN));
                        sendPos += sizeof(stateBatteryN);

                        send(clientSock, sendBuffer, sendPos, 0);
                        cout << "Sent system state." << endl;
                        break;
                    }
                    default:
                        cerr << "Unknown opcode received: " << opcode << endl;
                        break;
                }
                pthread_mutex_unlock(&this->robotMutex);
            }
            else if (bytesRead == 0) {
                cout << "Client " << id << " closed the connection." << endl;
                close(clientSock);
                break;
            } 
            else {
                cerr << "Error receiving data." << endl;
                close(clientSock);
                break;
            }
        }
    }
}

