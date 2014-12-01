#include "Shotgun.h"
#include "../Physics/PhysicsManager.h"
#include "../Display/OgreManager.h"
#include "../Bots/BotAbs.h"

#include "qDebug"

Shotgun::Shotgun(std::string name, Ogre::SceneNode * owner, float maxDistance, float maxAmmo, float timeBetweenShots, int damange) : WeaponAbs(name, maxDistance, maxAmmo, timeBetweenShots, damange)
{
    _soundIndex = OgreManager::Instance()->getSoundManager()->CreateSound(Ogre::String("Shotgun.wav"));
    _soundChannel = INVALID_SOUND_CHANNEL;

    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();
    Ogre::Entity * weaponEntity = sceneManager->createEntity(name, Ogre::SceneManager::PT_CUBE);
    _pWeaponNode = owner->createChildSceneNode(name);
    _pWeaponNode->attachObject(weaponEntity);
    _pWeaponNode->scale(0.006,0.006,0.012);
    _pWeaponNode->setPosition(1, 2, -1.5);

    //Creating Billboard for shot
    set = sceneManager->createBillboardSet(_name, 1);
    set->setMaterialName("shotting");

    set->createBillboard(0,0,-70, Ogre::ColourValue(1,1,1,1));

    Ogre::String shotNodeName = _name;

    Ogre::SceneNode * shot = _pWeaponNode->createChildSceneNode(shotNodeName+"shot");
    shot->attachObject(set);
    shot->scale(3,1,2);
    _pWeaponNode->setVisible(false);
}

Shotgun::~Shotgun()
{
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();
    _pWeaponNode->detachAllObjects();
    sceneManager->destroyEntity(sceneManager->getEntity(_name));
    sceneManager->destroySceneNode(_pWeaponNode);

    sceneManager->getSceneNode(_name+"shot")->detachAllObjects();
    sceneManager->destroySceneNode(_name+"shot");
    set->removeBillboard((unsigned int)(0));
    sceneManager->destroyBillboardSet(_name);
}

int Shotgun::getType()
{
    return WeaponAbs::Shotgun;
}

bool
Shotgun::PlayerShot(Ogre::Vector3 position, Ogre::Vector3 orientation)
{
    bool res = false;
    if(_timeNextShot <= 0 && _currentAmmo > 0)
    {
        //EMIT SHOT SOUND
        if(_soundChannel != INVALID_SOUND_CHANNEL)
        {
            OgreManager::Instance()->getSoundManager()->StopSound(&_soundChannel);
        }
        _soundChannel = INVALID_SOUND_CHANNEL;
        OgreManager::Instance()->getSoundManager()->PlaySound( _soundIndex, _pWeaponNode, &_soundChannel );

        btVector3 start(position.x,
                        position.y+0.5,
                        position.z);

        btVector3 end(position.x + orientation.x*_maxDistance,
                      position.y+0.5,
                      position.z + orientation.z*_maxDistance);

        btVector3 normal;

        entity * ent = PhysicsManager::Instance()->rayCastTest(start, end, normal);

        if(ent != 0 && ent->getType() == entity::Bot)
        {
            qDebug() << "LE HAS DADO A UN ENEMIGO!";
            btVector3 direction(end.x()-start.x(), 0, end.z()-start.z());
            ent->applyCentralImpulse(direction.normalize()*5);
            ((BotAbs*)(ent))->getBehaviour()->addLife(-_damange);
            res = true;
        }
        end = btVector3(orientation.x, 0, orientation.z);
        end = end.rotate(btVector3(0,1,0), 10);
        end = end + start;
        ent = 0;
        ent = PhysicsManager::Instance()->rayCastTest(start, end, normal);

        if(ent != 0 && ent->getType() == entity::Bot)
        {
            qDebug() << "LE HAS DADO A UN ENEMIGO!";
            btVector3 direction(end.x()-start.x(), 0, end.z()-start.z());
            ent->applyCentralImpulse(direction.normalize()*5);
            ((BotAbs*)(ent))->getBehaviour()->addLife(-_damange);
            res = true;
        }

        end = btVector3(orientation.x, 0, orientation.z);
        end = end.rotate(btVector3(0,1,0), -20);
        end = end + start;

        ent = 0;
        ent = PhysicsManager::Instance()->rayCastTest(start, end, normal);

        if(ent != 0 && ent->getType() == entity::Bot)
        {
            btVector3 direction(end.x()-start.x(), 0, end.z()-start.z());
            ent->applyCentralImpulse(direction.normalize()*5);
            ((BotAbs*)(ent))->getBehaviour()->addLife(-_damange);
            res = true;
        }
        //Build the "bullet"
        Ogre::Vector2 origin(0,1);

        Ogre::Billboard * billboard = set->getBillboard(0);
        billboard->setRotation(-origin.angleTo(Ogre::Vector2(orientation.x, orientation.z)));
        set->setVisible(true);
        set->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(0);

        _timeNextShot = _timeBetweenShots;
        _currentAmmo -=1;

        return res;
    }

    return res;
}

bool
Shotgun::BotShot(Ogre::Vector3 orientation)
{
    if(_timeNextShot <= 0)
    {
        //EMIT SHOT SOUND
        if(_soundChannel != INVALID_SOUND_CHANNEL)
        {
            OgreManager::Instance()->getSoundManager()->StopSound(&_soundChannel);
        }
        _soundChannel = INVALID_SOUND_CHANNEL;
        OgreManager::Instance()->getSoundManager()->PlaySound( _soundIndex, _pWeaponNode, &_soundChannel );

        //Build the "bullet"
        Ogre::Vector2 origin(0,1);

        Ogre::Billboard * billboard = set->getBillboard(0);
        billboard->setRotation(-origin.angleTo(Ogre::Vector2(orientation.x, orientation.z)));
        set->setVisible(true);
        set->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(0);
        OgreManager::Instance()->getSceneManager()->getSceneNode(_name+"shot")->setVisible(true);
        _timeNextShot = _timeBetweenShots;
        return true;
    }
    return false;
}

void
Shotgun::updateShot(float elapsedTime)
{
    if(_timeNextShot >= 0)
    {
        _timeNextShot -= elapsedTime;
        if (_timeNextShot <= _timeBetweenShots - _timeAnimationShot)
        {
            Ogre::String shotNodeName = _name;
            OgreManager::Instance()->getSceneManager()->getSceneNode(_name+"shot")->setVisible(false);
            set->setVisible(false);
        }
    }
}
