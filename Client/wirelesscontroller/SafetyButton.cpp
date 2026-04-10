/* Safety Button connects to the robot as a client. It does not contain threads because the 
multi controller problem is handled inside the robot. It has a flag "connected" just to 
know if the connection to socket worked or not. It presents only activation, disactivation
and stop.*/
#include "SafetyButton.h"

SafetyButton::SafetyButton(){
    this->connected = false;            // Initialized as disconnected
    this->sock = -1;
    this->initializeConnection();       // Tries to connect to the socket
}

SafetyButton::~SafetyButton(){
    if (this->sock > 0) {
        close(this->sock);              // Close the soket because the controller is deleted
        cout << "Disconnected and closed socket." << endl;
    }
}

void SafetyButton::initializeConnection(){
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    while (!connected){             // To check if the connection worked otherwise try again in a few seconds
        if (this->sock == -1) {
            this->sock = socket(AF_INET, SOCK_STREAM, 0);
        }

        int ret = connect(sock, (struct sockaddr*)&address, sizeof(address));
        if (ret < 0){                // If connection fails it tries again in 1 second
            cerr << "Connection failed. Try again in few seconds..." << endl;
            this->connected = false; 
            close(this->sock);       
            this->sock = -1;
            Utilities::sleepMs(1000);
        }
        else {
            cout << "Safety button connection initialized." << endl;
            this->connected = true;
        }
    }
}

void SafetyButton::sendActivate(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'A';
    cout << "Sending ACTIVATE." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void SafetyButton::sendDeactivate(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'D';
    cout << "Sending DEACTIVATE." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}

void SafetyButton::sendStop(){
    if (!this->connected) {
        cerr << "Error: Not connected to robot." << endl;
        return;
    }

    char opcode = 'S';
    cout << "Sending Stop." << endl;
    send(sock, &opcode, sizeof(opcode), 0);
}