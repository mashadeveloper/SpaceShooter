#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "MyBodyParser.h"

#include <math.h>

USING_NS_CC;

Scene* MainScene::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    scene->getPhysicsWorld()->setGravity(Vec2(0.0, 0.0));
    auto layer = MainScene::create();
    layer->SetPhysicsWorld(scene->getPhysicsWorld());
    scene->addChild(layer);
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() ) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 winSize = Director::getInstance()->getWinSizeInPixels();

    auto keyboardEventListener = EventListenerKeyboard::create();
    keyboardEventListener->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
    keyboardEventListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardEventListener, this);
    this->schedule(schedule_selector(MainScene::onKeyHold));

    auto mouseEventListener = EventListenerMouse::create();
    mouseEventListener->onMouseMove = CC_CALLBACK_1(MainScene::onMouseMove, this);
    mouseEventListener->onMouseDown = CC_CALLBACK_1(MainScene::onMouseDown, this);
    mouseEventListener->onMouseUp = CC_CALLBACK_1(MainScene::onMouseUp, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(mouseEventListener, 1);
    mousePosition = Vec2(visibleSize.height / 2, visibleSize.width / 2);
    this->schedule(schedule_selector(MainScene::followMouse));
    this->schedule(schedule_selector(MainScene::onMouseHold));

    back = Node::create();
    back->setAnchorPoint(Vec2(0, 0));
    this->addChild(back, 1);
    int verticalSpritesCount = visibleSize.height / 256 + 1;
    int horizontalSpritesCount = visibleSize.width / 256 + 1;
    for(int i = 0; i < horizontalSpritesCount; i++)
    {
        for(int j = 0; j < verticalSpritesCount; j++)
        {
            Sprite* sprite = Sprite::create("Backgrounds/purple.png");
            sprite->setAnchorPoint(Vec2(0, 0));
            sprite->setPosition(256 * i, 256 * j);
            back->addChild(sprite, 1);
        }
    }
    this->schedule( schedule_selector(MainScene::makeMoveBackground) );

    player->setModel("1");
    player->setColor("red");

    const std::string path = "PNG/";
    const std::string name = std::string("playerShip") + player->getModel() + "_" + player->getColor();

    auto pinfo = AutoPolygon::generatePolygon(path + name + std::string(".png"));
    player->setSprite(Sprite::create(pinfo));

    if( MyBodyParser::getInstance()->parseJsonFile( "physics_shapes.json" ) )
    {
        // Создаём физическое тело. Второй параметр - имя тела(не путать с именем файла), третий параметр - материал, с которым вы можете поиграться устанавливая различные значения.
        auto spriteBody = MyBodyParser::getInstance()->bodyFormJson( player->getSprite(), name, PhysicsMaterial( 1.0f, 0.0f, 1.0f ) );
        spriteBody->setCollisionBitmask(1);
        spriteBody->setContactTestBitmask(true);
        if( spriteBody != nullptr )
        {
            // Устанавливаем тело для спрайта
            player->getSprite()->setPhysicsBody( spriteBody );
        }
        else
        {
            CCLOG( "Object.cpp spriteBody is nullptr" );
        }
    }
    else
    {
        CCLOG( "JSON file not found" );
    }

    player->getSprite()->setAnchorPoint(Vec2(0.5, 0.5));
    player->getSprite()->setPosition(visibleSize.width / 2, player->getSprite()->getContentSize().height);
    player->getSprite()->setTag(1);
    this->addChild(player->getSprite(), 2);

    player->setHP(1000);
    player->setDamage(40);
    player->setFireRate(60);
    player->setFromLastShotTime(10.0);
    this->schedule(schedule_selector(MainScene::checkShipExitFromScreen));

    playerHP = Label::createWithTTF(std::to_string(player->getHP()), "NESCyrillic.ttf", 48, Size::ZERO, TextHAlignment::CENTER);
    //playerHP->setTextColor(Color4B::YELLOW);
    playerHP->setPosition(Vec2(visibleSize.width / 2, 50));
    this->addChild(playerHP, 1);

    this->schedule(schedule_selector(MainScene::addEnemy), 5.0);
    this->schedule(schedule_selector(MainScene::processEnemyActions));
    this->schedule(schedule_selector(MainScene::checkEnemiesExitFromScreen));
    this->schedule(schedule_selector(MainScene::checkEnemySpacing));
    this->schedule(schedule_selector(MainScene::calculateEnemyRotation));
    this->schedule(schedule_selector(MainScene::enemyShooting));

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(MainScene::onContactBegin, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    srand(time(0));

    return true;
}

void MainScene::makeMoveBackground(float dt)
{
    back->setPosition(back->getPosition().x, back->getPosition().y - 1);
    if(back->getPosition().y <= -256)
    {
        back->setPosition(0, 0);
    }
}

void MainScene::followMouse(float dt)
{
    Vec2 spritePosition = player->getSprite()->getPosition();

    const float PI = 3.14159265;

    float dx = mousePosition.x - spritePosition.x;
    float dy = mousePosition.y - spritePosition.y;

    float rotation = (std::atan2(dy, dx)) * (180 / PI);

    player->getSprite()->setRotation(-rotation + 90);
}

void MainScene::checkShipExitFromScreen(float dt)
{
    if(player->getSprite()->getPosition().x > visibleSize.width - 50)
    {
        player->getSprite()->setPosition(visibleSize.width - 50, player->getSprite()->getPosition().y);
    }

    if(player->getSprite()->getPosition().x < 50)
    {
        player->getSprite()->setPosition(50, player->getSprite()->getPosition().y);
    }

    if(player->getSprite()->getPosition().y > visibleSize.height - 50)
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x, visibleSize.height - 50);
    }

    if(player->getSprite()->getPosition().y < 50)
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x, 50);
    }
}

void MainScene::laserMoveFinished(Node *sender)
{
    Sprite *sprite = (Sprite *)sender;
    this->removeChild(sprite, true);
}

void MainScene::addEnemy(float dt)
{
    if(enemies.size() < 3)
    {
        EnemyShip* enemy = new EnemyShip;
        enemy->setColor("Black");
        enemy->setModel("1");
        enemy->setSprite(Sprite::create(std::string("PNG/Enemies/enemy") + enemy->getColor() + enemy->getModel() + ".png"));
        if( MyBodyParser::getInstance()->parseJsonFile( "physics_shapes.json" ) )
        {
            // Создаём физическое тело. Второй параметр - имя тела(не путать с именем файла), третий параметр - материал, с которым вы можете поиграться устанавливая различные значения.
            auto spriteBody = MyBodyParser::getInstance()->bodyFormJson( enemy->getSprite(),
                                                                         std::string("enemy") + enemy->getColor() + enemy->getModel(),
                                                                         PhysicsMaterial( 1.0f, 0.0f, 1.0f ) );
            spriteBody->setCollisionBitmask(2);
            spriteBody->setContactTestBitmask(true);
            if( spriteBody != nullptr )
            {
                // Устанавливаем тело для спрайта
                enemy->getSprite()->setPhysicsBody( spriteBody );
            }
            else
            {
                CCLOG( "Object.cpp spriteBody is nullptr" );
            }
        }
        else
        {
            CCLOG( "JSON file not found" );
        }
        enemy->getSprite()->setAnchorPoint(Vec2(0.5, 0.5));
        int x = rand() % (int)visibleSize.width;
        enemy->setOnAction(false);
        enemy->getSprite()->setPosition(x, visibleSize.height + enemy->getSprite()->getContentSize().height);
        enemy->getSprite()->runAction(MoveTo::create(1, Vec2(enemy->getSprite()->getPosition().x, enemy->getSprite()->getPosition().y - 200)));
        enemy->setHP(200);
        enemy->setDamage(20);
        enemy->setFireRate(30);
        enemy->setFromLastShotTime(10.0);
        enemy->getSprite()->setTag(2);
        this->addChild(enemy->getSprite(), 1);
        enemies.push_back(enemy);
    }
}

void MainScene::processEnemyActions(float dt)
{
    for(size_t i = 0; i < enemies.size(); i++)
    {
        if(!enemies[i]->isOnAction())
        {
            int x = 100 + rand() % (int)visibleSize.width - 100;
            int y = 100 + rand() % (int)visibleSize.height - 100;
            int dx = x - enemies[i]->getSprite()->getPosition().x;
            int dy = y - enemies[i]->getSprite()->getPosition().y;
            float length = sqrt(dx*dx + dy*dy);
            float speed = 240;
            float velocity = length/speed;
            auto callbackSetNoAction = CallFunc::create([this, i](){
                enemies[i]->setOnAction(false);
            });
            Action* action = Sequence::create(MoveTo::create(velocity, Vec2(x, y)), callbackSetNoAction, NULL);
            action->setTag(1);
            enemies[i]->getSprite()->runAction(action);
            enemies[i]->setOnAction(true);
            continue;
        }
    }
}

void MainScene::checkEnemiesExitFromScreen(float dt)
{
    for(size_t i = 0; i < enemies.size(); i++)
    {
        if(enemies[i]->isOnAction())
        {
            if(enemies[i]->getSprite()->getPosition().x > visibleSize.width - 50)
            {
                enemies[i]->getSprite()->setPosition(visibleSize.width - 51, enemies[i]->getSprite()->getPosition().y);
                enemies[i]->getSprite()->stopAllActions();
                enemies[i]->setOnAction(false);
            }

            if(enemies[i]->getSprite()->getPosition().x < 50)
            {
                enemies[i]->getSprite()->setPosition(51, enemies[i]->getSprite()->getPosition().y);
                enemies[i]->getSprite()->stopAllActions();
                enemies[i]->setOnAction(false);
            }

            if(enemies[i]->getSprite()->getPosition().y > visibleSize.height - 50)
            {
                enemies[i]->getSprite()->setPosition(enemies[i]->getSprite()->getPosition().x, visibleSize.height - 51);
                enemies[i]->getSprite()->stopAllActions();
                enemies[i]->setOnAction(false);
            }

            if(enemies[i]->getSprite()->getPosition().y < 50)
            {
                enemies[i]->getSprite()->setPosition(enemies[i]->getSprite()->getPosition().x, 51);
                enemies[i]->getSprite()->stopAllActions();
                enemies[i]->setOnAction(false);
            }
        }
    }
}

void MainScene::checkEnemySpacing(float dt)
{
    if(enemies.size() < 2)
    {
        return;
    }
    for(size_t i = 0; i < enemies.size() - 1; i++)
    {
        for(size_t j = i + 1; j < enemies.size(); j++)
        {
            int dx = enemies[i]->getSprite()->getPosition().x - enemies[j]->getSprite()->getPosition().x;
            int dy = enemies[i]->getSprite()->getPosition().y - enemies[j]->getSprite()->getPosition().y;
            float length = sqrt(dx*dx + dy*dy);
            if(length < 130)
            {
                enemies[i]->getSprite()->setPosition(enemies[i]->getSprite()->getPosition().x + (dx > 0 ? 1 : -1),
                                                     enemies[i]->getSprite()->getPosition().y + (dy > 0 ? 1 : -1));
                enemies[j]->getSprite()->setPosition(enemies[j]->getSprite()->getPosition().x + (dx > 0 ? -1 : 1),
                                                     enemies[j]->getSprite()->getPosition().y + (dy > 0 ? -1 : 1));

                int x1 = enemies[i]->getSprite()->getPosition().x + (dx > 0 ? rand() % 250 : -(rand() % 250));
                int x2 = enemies[i]->getSprite()->getPosition().x + (dx > 0 ? -(rand() % 250) : rand() % 250);

                int y1 = enemies[i]->getSprite()->getPosition().y + (dy > 0 ? rand() % 250 : -(rand() % 250));
                int y2 = enemies[i]->getSprite()->getPosition().y + (dy > 0 ? -(rand() % 250) : rand() % 250);

                auto moveShip = [](EnemyShip* ship, int x, int y)
                {
                    int dx = ship->getSprite()->getPosition().x - x;
                    int dy = ship->getSprite()->getPosition().y - y;
                    float length = sqrt(dx*dx + dy*dy);
                    float speed = 240;
                    float velocity = length/speed;
                    auto callbackSetNoAction = CallFunc::create([ship](){
                        ship->setOnAction(false);
                    });
                    ship->getSprite()->stopActionByTag(1);
                    Action* action = Sequence::create(MoveTo::create(velocity, Vec2(x, y)), callbackSetNoAction, NULL);
                    action->setTag(1);
                    ship->getSprite()->runAction(action);
                    ship->setOnAction(true);
                };

                moveShip(enemies[i], x1, y1);
                moveShip(enemies[j], x2, y2);

                return;
            }
        }
    }
}

void MainScene::calculateEnemyRotation(float dt)
{
    for(size_t i = 0; i < enemies.size(); i++)
    {
        Vec2 playerPosition = player->getSprite()->getPosition();
        Vec2 enemyPosition = enemies[i]->getSprite()->getPosition();

        const float PI = 3.14159265;

        float dx = enemyPosition.x - playerPosition.x;
        float dy = enemyPosition.y - playerPosition.y;

        float rotation = (std::atan2(dy, dx)) * (180 / PI);

        enemies[i]->getSprite()->setRotation(-rotation + 90);
    }
}

void MainScene::enemyShooting(float dt)
{
    for(size_t i = 0; i < enemies.size(); i++)
    {
        enemies[i]->setFromLastShotTime(enemies[i]->getFromLastShotTime() + dt);
        if(enemies[i]->getFromLastShotTime() < 60.0 / enemies[i]->getFireRate())
        {
            continue;
        }

        enemies[i]->setFromLastShotTime(0);

        Vec2 enemyPosition = enemies[i]->getSprite()->getPosition();
        Vec2 playerPosition = player->getSprite()->getPosition();

        EnemyLaser* el = new EnemyLaser;

        el->setSprite(Sprite::create("PNG/Lasers/laserRed03.png"));
        el->setDamage(enemies[i]->getDamage());

        auto laserBody = PhysicsBody::createBox(el->getSprite()->getContentSize());
        laserBody->setCollisionBitmask(4);
        laserBody->setContactTestBitmask(true);
        el->getSprite()->setPhysicsBody(laserBody);

        float x0 = playerPosition.x - enemyPosition.x;
        float y0 = playerPosition.y - enemyPosition.y;

        float k = 90 / (sqrt(x0*x0 + y0*y0));

        float x1 = x0 * k;
        float y1 = y0 * k;

        el->getSprite()->setPosition(enemyPosition.x + x1, enemyPosition.y + y1);
        el->getSprite()->setRotation(enemies[i]->getSprite()->getRotation() + 180);

        float length = sqrt(visibleSize.width * visibleSize.width + visibleSize.height * visibleSize.height);

        float kdest = length / sqrt(x0*x0 + y0*y0);

        float xdest = x0 * kdest;
        float ydest = y0 * kdest;

        Vec2 dest = Vec2(enemyPosition.x + xdest, enemyPosition.y + ydest);

        float velocity = 320;
        float duration = length / velocity;

        el->getSprite()->setTag(4);

        el->getSprite()->runAction( Sequence::create(
                              MoveTo::create(duration, dest),
                              CallFuncN::create(this, callfuncN_selector(MainScene::laserMoveFinished)),
                              NULL) );

        enemyLasers.push_back(el);

        this->addChild(el->getSprite(), 1);
    }
}

bool MainScene::onContactBegin(PhysicsContact &contact)
{
    PhysicsBody* A = contact.getShapeA()->getBody();
    PhysicsBody* B = contact.getShapeB()->getBody();

    if ((1 == A->getCollisionBitmask() && 4 == B->getCollisionBitmask()) || (1 == B->getCollisionBitmask() && 4 == A->getCollisionBitmask()))
    {
        auto enemyLaser = getEnemyLaserByNode(1 == A->getCollisionBitmask() ? B->getNode() : A->getNode());

        if(enemyLaser)
        {
            player->setHP(player->getHP() - enemyLaser->getDamage());
            playerHP->setString(std::to_string(player->getHP()));
            if(player->getHP() <= 0)
            {
                //lose
            }
        }

        this->removeChild(1 == A->getCollisionBitmask() ? B->getNode() : A->getNode());
        enemyLasers.erase(std::remove( enemyLasers.begin(), enemyLasers.end(), enemyLaser ), enemyLasers.end());
    }
    else if ((2 == A->getCollisionBitmask() && 4 == B->getCollisionBitmask()) || (2 == B->getCollisionBitmask() && 4 == A->getCollisionBitmask()))
    {
        if(2 == A->getCollisionBitmask())
        {
            this->removeChild(B->getNode());
        }
        else if(2 == B->getCollisionBitmask())
        {
            this->removeChild(A->getNode());
        }
    }
    else if ((2 == A->getCollisionBitmask() && 3 == B->getCollisionBitmask()) || (2 == B->getCollisionBitmask() && 3 == A->getCollisionBitmask()))
    {
        this->removeChild(2 == A->getCollisionBitmask() ? B->getNode() : A->getNode());

        auto enemy = getEnemyShipByNode(2 == A->getCollisionBitmask() ? A->getNode() : B->getNode());

        if(enemy)
        {
            enemy->setHP(enemy->getHP() - player->getDamage());

            if(enemy->getHP() <= 0)
            {
                this->removeChild(enemy->getSprite());
                enemies.erase(std::remove( enemies.begin(), enemies.end(), enemy ), enemies.end());
            }
        }
    }

    return true;
}

EnemyShip *MainScene::getEnemyShipByNode(Node *node)
{
    for(size_t i = 0; i < enemies.size(); i++)
    {
        if(enemies[i]->getSprite() == node && enemies[i] != NULL)
        {
            return enemies[i];
        }
    }
    return NULL;
}

EnemyLaser *MainScene::getEnemyLaserByNode(Node *node)
{
    for(size_t i = 0; i < enemyLasers.size(); i++)
    {
        if(enemyLasers[i]->getSprite() == node && enemyLasers[i] != NULL)
        {
            return enemyLasers[i];
        }
    }
    return NULL;
}


void MainScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    if(std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end())
        heldKeys.push_back(keyCode);
}

void MainScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event)
{
    heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
}

void MainScene::onKeyHold(float dt)
{
    // up pressed
    if(std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_UP_ARROW) != heldKeys.end()
            || std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_W) != heldKeys.end())
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x, player->getSprite()->getPosition().y + 4);
    }

    // down pressed
    if(std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_DOWN_ARROW) != heldKeys.end()
            || std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_S) != heldKeys.end())
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x, player->getSprite()->getPosition().y - 4);
    }

    // right pressed
    if(std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_RIGHT_ARROW) != heldKeys.end()
            || std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_D) != heldKeys.end())
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x + 4, player->getSprite()->getPosition().y);
    }

    // left pressed
    if(std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_LEFT_ARROW) != heldKeys.end()
            || std::find(heldKeys.begin(), heldKeys.end(), EventKeyboard::KeyCode::KEY_A) != heldKeys.end())
    {
        player->getSprite()->setPosition(player->getSprite()->getPosition().x - 4, player->getSprite()->getPosition().y);
    }
}

void MainScene::onMouseMove(Event *event)
{
    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;

    Vec2 spritePosition = player->getSprite()->getPosition();
    mousePosition = Vec2(e->getCursorX(), e->getCursorY());

    // now we have both the sprite position and the cursor
    // position lets do the calculation so our sprite will
    // face the position of the mouse
    const float PI = 3.14159265;

    float dx = mousePosition.x - spritePosition.x;
    float dy = mousePosition.y - spritePosition.y;

    float rotation = (std::atan2(dy, dx)) * (180 / PI);

    player->getSprite()->setRotation(-rotation + 90);
}

void MainScene::onMouseDown(Event *event)
{
    mouseDown = true;
}

void MainScene::onMouseUp(Event *event)
{
    mouseDown = false;
}

void MainScene::onMouseHold(float dt)
{
    player->setFromLastShotTime(player->getFromLastShotTime() + dt);
    if(player->getFromLastShotTime() < 60.0 / player->getFireRate())
    {
        return;
    }
    if(mouseDown)
    {
        player->setFromLastShotTime(0);

        Vec2 spritePosition = player->getSprite()->getPosition();

        Sprite *laser = Sprite::create("PNG/Lasers/laserRed01.png");

        auto laserBody = PhysicsBody::createBox(laser->getContentSize());
        laserBody->setCollisionBitmask(3);
        laserBody->setContactTestBitmask(true);
        laser->setPhysicsBody(laserBody);

        float x0 = mousePosition.x - spritePosition.x;
        float y0 = mousePosition.y - spritePosition.y;

        float k = 90 / (sqrt(x0*x0 + y0*y0));

        float x1 = x0 * k;
        float y1 = y0 * k;

        laser->setPosition(spritePosition.x + x1, spritePosition.y + y1);
        laser->setRotation(player->getSprite()->getRotation());

        float length = sqrt(visibleSize.width * visibleSize.width + visibleSize.height * visibleSize.height);

        float kdest = length / sqrt(x0*x0 + y0*y0);

        float xdest = x0 * kdest;
        float ydest = y0 * kdest;

        Vec2 dest = Vec2(spritePosition.x + xdest, spritePosition.y + ydest);

        float velocity = 720;
        float duration = length / velocity;

        laser->setTag(3);

        laser->runAction( Sequence::create(
                              MoveTo::create(duration, dest),
                              CallFuncN::create(this, callfuncN_selector(MainScene::laserMoveFinished)),
                              NULL) );

        this->addChild(laser, 1);
    }
}


