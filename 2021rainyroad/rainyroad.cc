#include "rainyroad.h"

int rainWater::getLocation()
{
    return this->location;
}

int rainWater::getRain()
{
    return this->rain;
}

void rainWater::addRain(int x)
{
    this->rain += x;
}




int leftWater::getLocation()
{
    return this->location;
}

int leftWater::getHeight()
{
    return this->height;
}

int leftWater::getWater()
{
    return this->water;
}

void leftWater::addWater(int x)
{
    this->water += x;
}

void leftWater::subWater(int x)
{
    this->water -= x;
}


