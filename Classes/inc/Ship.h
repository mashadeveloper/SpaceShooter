#ifndef __SHIP_H__
#define __SHIP_H__

#include "cocos2d.h"
#include <string>

class Ship
{
public:

    int getHP() const;
    void setHP(int value);

    float getFireRate() const;
    void setFireRate(float value);

    float getDamage() const;
    void setDamage(float value);

    char getModel() const;
    void setModel(const char *value);

    std::string getColor() const;
    void setColor(const std::string &value);

    cocos2d::Sprite *getSprite();
    void setSprite(cocos2d::Sprite *value);

    float getFromLastShotTime() const;
    void setFromLastShotTime(float value);

protected:
    int HP;
    float fireRate;
    float fromLastShotTime;
    float damage;
    char model;
    std::string color;
    cocos2d::Sprite *sprite;
};

#endif // __SHIP_H__
