#include "KeyboardController.h"
#include "../Player/Player.h"
#include <qDebug>

KeyboardController * KeyboardController::_pInstance = 0;

KeyboardController::KeyboardController( )
{
    _vKeysPresed = std::vector<OIS::KeyCode>();
    _vActionKey = std::map<std::string,OIS::KeyCode>();
    _rotation180 = false;

    //ESTO TENDRA QUE CARGAR DE FORMA EXTERNA DESDE CONFIG
    _vActionKey["moveUp"] = OIS::KC_W;
    _vActionKey["moveDown"] = OIS::KC_S;
    _vActionKey["moveRight"] = OIS::KC_D;
    _vActionKey["moveLeft"] = OIS::KC_A;
    _vActionKey["rotateLeft"] = OIS::KC_LEFT;
    _vActionKey["rotateRight"] = OIS::KC_RIGHT;
    _vActionKey["rotate180"] = OIS::KC_DOWN;
    _vActionKey["shot"] = OIS::KC_UP;
    _vActionKey["jump"] = OIS::KC_SPACE;
    _vActionKey["weaponUp"] = OIS::KC_LCONTROL;
    _vActionKey["weaponDown"] = OIS::KC_LSHIFT;
    _timers.push_back(0);
    _timers.push_back(0);
}

KeyboardController::~KeyboardController()
{

    _vActionKey.clear();
    _vKeysPresed.clear();
}

KeyboardController * KeyboardController::Instance()
{
    if(!_pInstance)
        _pInstance = new KeyboardController();

    return _pInstance;
}

void
KeyboardController::update(double elapsedTime)
{
    std::vector<OIS::KeyCode>::iterator keyIt;
    Ogre::Vector3 playerDirection(0,0,0);

    bool downPresed = false;

    for(keyIt = _vKeysPresed.begin(); keyIt != _vKeysPresed.end(); ++keyIt)
    {
        if((*keyIt) == _vActionKey["moveUp"])
        {
            playerDirection.z = 1;
        }
        else if((*keyIt) == _vActionKey["moveDown"])
        {
            playerDirection.z = -1;
        }
        else if((*keyIt) == _vActionKey["moveLeft"])
        {
            playerDirection.x = 1;
        }
        else if((*keyIt) == _vActionKey["moveRight"])
        {
            playerDirection.x = -1;
        }
        else if((*keyIt) == _vActionKey["jump"])
        {
            Player::Instance()->jump();
        }
        else if((*keyIt) == _vActionKey["rotateLeft"])
        {
            Player::Instance()->addRotation(5*elapsedTime);
        }
        else if((*keyIt) == _vActionKey["rotateRight"])
        {
            Player::Instance()->addRotation(-5*elapsedTime);
        }
        else if((*keyIt) == _vActionKey["shot"])
        {
            Player::Instance()->shot();
        }
        else if((*keyIt) == _vActionKey["rotate180"])
        {
            downPresed = true;
        }
        else if(((*keyIt) == _vActionKey["weaponUp"]) && (_timers[0] > 0.5))
        {
            _timers[0] = 0;
            Player::Instance()->changeWeaponUp();
        }
        else if(((*keyIt) == _vActionKey["weaponDown"]) && (_timers[1] > 0.5))
        {
            _timers[1] = 0;
            Player::Instance()->changeWeaponDown();
        }
    }

    for(int i = 0; i < _timers.size(); i++)
    {
        _timers[i] += elapsedTime;
    }

    if(downPresed)
    {
        if(_rotation180)
        {
            Player::Instance()->addRotation(Ogre::Math::PI);
            _rotation180 = false;
        }
    }
    else
        _rotation180 = true;

    playerDirection.normalise();

    Player::Instance()->setDirectionToMove(playerDirection.x,
                                           playerDirection.y,
                                           playerDirection.z);
}

void
KeyboardController::pressKey(OIS::KeyCode key)
{
   _vKeysPresed.push_back(key);
}

void
KeyboardController::releaseKey(OIS::KeyCode key)
{
    std::vector<OIS::KeyCode>::iterator keyIt;
    for(keyIt = _vKeysPresed.begin(); keyIt != _vKeysPresed.end(); ++keyIt)
    {
        if((*keyIt) == key)
        {
            _vKeysPresed.erase(keyIt);
            break;
        }
    }

}
