#include <iostream>
using namespace std;

class rainWater // 降雨量
{
private:
    int location; // 地点
    int rain; // 降雨量
public:
    rainWater(int location, int rain):location(location), rain(rain){}
    ~rainWater(){}
    int getLocation();
    int getRain();
    void addRain(int x);
};

class leftWater // 积水量
{
private:
    int location; // 地点
    int height; // 高度
    int water; // 积水量
public:
    leftWater(){}
    leftWater(int location, int height, int water):location(location), height(height), water(water){}
    ~leftWater(){}
    int getLocation();
    int getHeight();
    int getWater();
    void addWater(int x);
    void subWater(int x);
};