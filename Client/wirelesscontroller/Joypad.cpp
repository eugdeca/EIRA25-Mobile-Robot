/* Joypad connects to the robot as a client. It does not contain threads because the 
multi controller problem is handled inside the robot. It has a flag "connected" just to 
know if the connection to socket worked or not. It presents all the possible actions that 
can be taken to controll the robot.*/
#include "Joypad.h"

Joypad::Joypad(){
    this->connected = false;             // Initialized as disconnected from the socket
    this->sock = -1;
    this->initializeConnection();        // Connection to the socket
}

Joypad::~Joypad(){
    if (this->sock > 0) {
        close(this->sock);
        cout << "Disconnected and closed socket." << endl;
    }
}

void Joypad::initializeConnection(){
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    while (!connected){             // To check if the connection worked otherwise try again in a few seconds
        if (this->sock == -1) {
            this->sock = socket(AF_INET, SOCK_STREAM, 0);
        }

        int ret = connect(sock, (struct sockaddr*)&address, sizeof(address));
        if (ret < 0){
            cerr << "Connection failed. Try again in few seconds..." << endl;
            this->connected = false; 
            close(this->sock);       
            this->sock = -1;
            Utilities::sleepMs(1000);
        }
        else {
            cout << "Joypad connection initialized." << endl;
            this->connected = true;
        }
    }
}

void Joypad::sendActivate(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'A';
    cout << "Sending ACTIVATE." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void Joypad::sendDeactivate(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'D';
    cout << "Sending DEACTIVATE." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void Joypad::sendStop(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'S';
    cout << "Sending Stop." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void Joypad::sendAngleAndSpeed(float angle, uint32_t speedPerc){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }
    // Check if the data are correct
    if (angle < 0 || angle > 360) {
        cout << "Inserted angle value is not valid [0° - 360°]." << endl;
        if (speedPerc > 100){ // Unsigned int can be only positive
            cout << "Inserted speed percentage value is not valid [0% - 100%]." << endl;
        }
        return;
    }
    else if (speedPerc > 100){ // Unsigned int can be only positive
        cout << "Inserted speed percentage value is not valid [0% - 100%]." << endl;
        return;
    }
    // Preparing the buffer to send to robot
    char buffer[sizeof(char) + sizeof(float) + sizeof(uint32_t)];
    int pos = 0;

    char opcode = 'M';
    memcpy(buffer + pos, &opcode, sizeof(opcode));
    pos += sizeof(opcode);

    uint32_t angleN = Utilities::htonf(angle);
    memcpy(buffer + pos, &angleN, sizeof(angleN));
    pos += sizeof(angleN);

    uint32_t speedPercN = htonl(speedPerc);
    memcpy(buffer + pos, &speedPercN, sizeof(speedPercN));
    pos += sizeof(speedPercN);

    cout << "Sending Movement: angle " << angle << ", speed percentage " << speedPerc << "%" << endl;
    send(sock, buffer, pos, 0);
}

void Joypad::sendTilt(float tilt){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }
    if (tilt<0 || tilt>90){
        cerr << "Inserted tilt value is not valid [0° - 90°]." << endl;
        return;
    }
    // Preparing the buffet to send to robot
    char buffer[sizeof(char) + sizeof(float)];
    int pos = 0;

    char opcode = 'T';
    memcpy(buffer + pos, &opcode, sizeof(opcode));
    pos += sizeof(opcode);

    uint32_t tiltN = Utilities::htonf(tilt);
    memcpy(buffer + pos, &tiltN, sizeof(tiltN));
    pos += sizeof(tiltN);

    cout << "Sending target tilt: " << tilt << endl;
    send(sock, buffer, pos, 0);
}

void Joypad::sendClose(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'C';
    cout << "Sending close gripper." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void Joypad::sendOpen(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'O';
    cout << "Sending open gripper." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void Joypad::sendGetState(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }
    // Send just a char
    char opcode = 'G';

    cout << "Requesting system state..." << endl;
    send(sock, &opcode, sizeof(opcode), 0);

    // Preparing Buffer to recive the whole state
    char recvBuffer[sizeof(float)*5 + sizeof(uint16_t) + sizeof(float)];

    int bytesRead = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
    if (bytesRead == 26) {    // 26 is the total amount of bytes for the full state
        int pos = 0;

        uint32_t XN, YN, speedN, angleN, tiltN, battLevelN;   // Network variables (uint32_t)
        float X, Y, speed, angle, tilt, battLevel;            // Hpst variables (float or uint16_t)
        uint16_t gripperState;

        // Extract X position
        memcpy(&XN, recvBuffer + pos, sizeof(XN));
        pos += sizeof(XN);
        X = Utilities::ntohf(XN);
        // Extract Y position
        memcpy(&YN, recvBuffer + pos, sizeof(YN));
        pos += sizeof(YN);
        Y = Utilities::ntohf(YN);
        // Extract speed
        memcpy(&speedN, recvBuffer + pos, sizeof(speedN));
        pos += sizeof(speedN);
        speed = Utilities::ntohf(speedN);
        // Extract angle w.r.t. North
        memcpy(&angleN, recvBuffer + pos, sizeof(angleN));
        pos += sizeof(angleN);
        angle = Utilities::ntohf(angleN);
        // Extract tilt angle
        memcpy(&tiltN, recvBuffer + pos, sizeof(tiltN));
        pos += sizeof(tiltN);
        tilt = Utilities::ntohf(tiltN);
        // Extract position state
        memcpy(&gripperState, recvBuffer + pos, sizeof(gripperState));
        pos += sizeof(gripperState);
        gripperState = ntohs(gripperState);
        // Extract battery level
        memcpy(&battLevelN, recvBuffer + pos, sizeof(battLevelN));
        pos += sizeof(battLevelN);
        battLevel = Utilities::ntohf(battLevelN);

        cout << "State of the robot: " << endl;
        cout << "Pos: (" << X << "," << Y << ")" << endl;
        cout << "Speed: " << speed << "[m/s]" << endl;
        cout << "Angle w.r.t. North: " << angle << "°" << endl;
        cout << "Arm tilt: " << tilt << "°" << endl;
        cout << "Gripper state (0=closed, 1=opened): " << gripperState << endl;
        cout << "Battery level: " << battLevel << "%"  << endl;
    } 
    else {
        cerr << "Error receiving system state." << endl;
    }    
}