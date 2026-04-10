/*The battery is passive: it has no threads, th other components do. It only has a mutex for the 
discharge method while the charge one is handled by the robot so (mutex included).
When the discharge method is called and there is not enouch battery threads are set to wait for the
full charge.*/
#include "Battery.h"

Battery::Battery(){
    this->batt_percent = 100.0f;
    this->battDischarged = false;    // Tells the robot when the battery is not enough
    this->stop = false;        // Used when the robot is shutdown
    this->batt_mutex = PTHREAD_MUTEX_INITIALIZER;
    this->cond_discharged = PTHREAD_COND_INITIALIZER;
    this->cond_charged = PTHREAD_COND_INITIALIZER;
}

// Possibility to set the battery level manually
Battery::Battery(float batt_percent){
    this->batt_percent = batt_percent;
    this->battDischarged = false;    // Tells the robot when the battery is not enough
    this->stop = false;      // Used when the robot is shutdown
    this->batt_mutex = PTHREAD_MUTEX_INITIALIZER;
    this->cond_discharged = PTHREAD_COND_INITIALIZER;
    this->cond_charged = PTHREAD_COND_INITIALIZER;
}

Battery::~Battery(){
    pthread_mutex_destroy(&batt_mutex);
    pthread_cond_destroy(&cond_discharged);
    pthread_cond_destroy(&cond_charged);
}

pthread_mutex_t& Battery::getBatt_mutex() { return batt_mutex; }
pthread_cond_t& Battery::getCond_discharged() { return cond_discharged; }

pthread_cond_t& Battery::getCond_charged() { return cond_charged; }

float Battery::charge(float q){
    // Here there is no mutex because the robot handles it, it is the only one to access this method
    // and it locks the mutex.
    if (this->batt_percent == 100.0f){
        cout << "Battery already full" << endl;
    }
    else if (this->batt_percent+q <= 100.0f){
        this->batt_percent = this->batt_percent + q;
    }
    else {
        this->batt_percent = 100.0f;
    }
    return this->batt_percent;
}

bool Battery::discharge(float q){
    // Here there is the mutex because all the parts can access this method.
    pthread_mutex_lock(&batt_mutex);
    while (this->batt_percent < q && !this->stop){
        cout << "There is not enough battery..." << endl;
        this->battDischarged = true;
        pthread_cond_signal(&cond_discharged);
        pthread_cond_wait(&cond_charged, &batt_mutex);
    }

    if (this->stop) {                             // When the stop is called I dont want to discharge
        pthread_mutex_unlock(&batt_mutex);
        return false; 
    }

    this->battDischarged = false;
    this->batt_percent -= q;
    pthread_mutex_unlock(&batt_mutex);
    return true;
}

bool Battery::getBatteryDischarged() {return this->battDischarged;}
float Battery::getLevel(){return this->batt_percent;}

void Battery::print(){
    int num_dec = this->batt_percent/10;

    cout << "[";
    for (int i = 0; i < num_dec; i++){
        cout << "=";
    }
    for (int i = num_dec; i < 10; i++){
        cout << " ";
    }
    cout << "]" << endl;
}

void Battery::setStop(bool stop){this->stop = stop; }
bool Battery::getStop(){return this->stop; }


