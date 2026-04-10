#include "Environment.h"

Environment::Environment():WIDTH(100.0f),HEIGHT(100.0f){
    this->WEATHER_PERCENT = 100.0f;
    this->WIND_SPEED_PERCENT = 100.0f;
}

Environment::Environment(float width, float height, float weather, float wind_speed):WIDTH(width > 0 ? width : 100.0f), 
HEIGHT(height > 0 ? height : 100.0f){
    if (weather < 0) this->WEATHER_PERCENT = 0;
    else if (weather > 100) this->WEATHER_PERCENT = 100;
    else this->WEATHER_PERCENT = weather;

    if (wind_speed < 0) this->WIND_SPEED_PERCENT = 0;
    else if (wind_speed > 100) this->WIND_SPEED_PERCENT = 100;
    else this->WIND_SPEED_PERCENT = wind_speed;
}

float Environment::getWidth(){return WIDTH;}
float Environment::getHeight(){return HEIGHT;}
float Environment::getWeather(){return WEATHER_PERCENT;}
float Environment::getWind_Speed(){return WIND_SPEED_PERCENT;}

void Environment::setWeather(float weather){this->WEATHER_PERCENT = weather;}
void Environment::setWind_Speed(float wind_speed){this->WIND_SPEED_PERCENT = wind_speed;}