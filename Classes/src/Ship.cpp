#include "Ship.h"

int Ship::getHP() const
{
    return HP;
}

void Ship::setHP(int value)
{
    HP = value;
}

float Ship::getFireRate() const
{
    return fireRate;
}

void Ship::setFireRate(float value)
{
    fireRate = value;
}

float Ship::getDamage() const
{
    return damage;
}

void Ship::setDamage(float value)
{
    damage = value;
}

char Ship::getModel() const
{
    return model;
}

void Ship::setModel(const char* value)
{
    model = *const_cast<char*>(value);
}

std::string Ship::getColor() const
{
    return color;
}

void Ship::setColor(const std::string &value)
{
    color = value;
}

cocos2d::Sprite *Ship::getSprite()
{
    return sprite;
}

void Ship::setSprite(cocos2d::Sprite *value)
{
    sprite = value;
}

float Ship::getFromLastShotTime() const
{
    return fromLastShotTime;
}

void Ship::setFromLastShotTime(float value)
{
    fromLastShotTime = value;
}
