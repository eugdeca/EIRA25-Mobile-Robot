#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <iostream>
using namespace std;

class Environment{
    protected:
      const float WIDTH, HEIGHT;          // meters
      float WEATHER_PERCENT;              // 0=cloudy, 100=sunny
      float WIND_SPEED_PERCENT;           // 0=No wind, 100="storm"

    public:
      Environment();
      Environment(float width, float height, float weather, float wind_speed);

      float getWidth();
      float getHeight();
      float getWeather();
      float getWind_Speed();

      void setWeather(float weather);
      void setWind_Speed(float wind_speed);
};

#endif //ENVIRONMENT_H