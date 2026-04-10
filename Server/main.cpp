#include "robot/Robot.h"

int main() {
    Environment* env = new Environment(200, 100, 10, 100);
    // Environment* env = new Environment(100, 100, 0, 0);
    cout << "The environment has the following parameters:" << endl;
    cout << "X axis: (" << 0 << "," <<  env->getWidth() << ")" << endl;
    cout << "Y axis: (" << 0 << "," <<  env->getHeight() << ")" << endl;
    cout << "Sun: " << env->getWeather() << "%" << endl; 
    cout << "Wind speed: " << env->getWind_Speed() << "%" << endl;
    
    int max; 
    cout << "Chose max number of controllers that can be connected to the robot:" << endl;
    cin >> max;

    Robot* robot = new Robot(env, max);
    robot->startService();
    
    // while (true){
    //     Utilities::sleepMs(20000);
    //     env->setWeather(0);
    //     env->setWind_Speed(0);

    //     Utilities::sleepMs(20000);
    //     env->setWeather(10);
    //     env->setWind_Speed(100);

    //     Utilities::sleepMs(20000);
    //     env->setWeather(100);
    //     env->setWind_Speed(10);
    // }
    // Trivial while to let the user Kill the robot when he wants
    while (true){
        char kill;
        cout << "Press K to KILL the robot" << endl;
        cin >> kill;

        if (kill == 'k' || kill == 'K'){
            robot->kill = true;
            break;
        }
        else {
            cout << "Char unknown" << endl;
            continue;
        }
    }
    delete robot;
    delete env;
    return 0;
}