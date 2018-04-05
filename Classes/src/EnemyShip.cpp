#include "EnemyShip.h"


bool EnemyShip::isOnAction() const
{
    return onAction;
}

void EnemyShip::setOnAction(bool value)
{
    onAction = value;
}

float EnemyLaser::getDamage() const
{
    return damage;
}

void EnemyLaser::setDamage(float value)
{
    damage = value;
}

cocos2d::Sprite *EnemyLaser::getSprite() const
{
    return sprite;
}

void EnemyLaser::setSprite(cocos2d::Sprite *value)
{
    sprite = value;
}
