#include "BehaviourAbs.h"
#include "../../Scenary/SceneObjects/Item/ItemManager.h"

BehaviourAbs::BehaviourAbs(int life, int points, std::vector<std::string> itemsNames)
{
    _vPosition = Ogre::Vector3(0,0,0);
    _vDirection = Ogre::Vector3(0,0,0);
    _pAnimationStateBottom = NULL;
    _pAnimationStateTop = NULL;
    _points = points;
    _maxLife = life;
    _life = life;
    _itemProb = 0.7;
    _removeFlag = false;
    _vItemsNames = itemsNames;
}

BehaviourAbs::~BehaviourAbs()
{

}

void
BehaviourAbs::setPosition(float x, float y, float z)
{
    _vPosition.x = x;
    _vPosition.y = y;
    _vPosition.z = z;
}

Ogre::Vector3
BehaviourAbs::getPosition()
{
    return _vPosition;
}

void
BehaviourAbs::setDirection(float x, float y, float z)
{
    _vDirection.x = x;
    _vDirection.y = y;
    _vDirection.z = z;
}

Ogre::Vector3
BehaviourAbs::getDirection()
{
    return _vDirection;
}

/*void
BehaviourAbs::setAnimation(std::string name, Ogre::Entity * botEntity, bool loop)
{
    if(_pAnimationState)
    {
       _pAnimationState->setEnabled(false);
    }
    _pAnimationState = botEntity->getAnimationState(name);
    _pAnimationState->setEnabled(true);
    _pAnimationState->setLoop(loop);
}*/

void BehaviourAbs::stopAllAnimations()
{
    if(_pAnimationStateBottom != NULL)
    {
        _pAnimationStateBottom->setEnabled(false);
    }
    if(_pAnimationStateTop != NULL)
    {
        _pAnimationStateTop->setEnabled(false);
    }
}

void BehaviourAbs::createRandomItem()
{
    int itemIndex = Ogre::Math::IFloor(Ogre::Math::RangeRandom(0, _vItemsNames.size()));
    if(itemIndex == _vItemsNames.size())
        itemIndex--;

    ItemManager::Instance()->addItem(_vItemsNames[itemIndex], _vPosition);
}

bool BehaviourAbs::getRemoveFlag()
{
    return _removeFlag;
}

void BehaviourAbs::addLife(int life)
{
    _life += life;
}

void BehaviourAbs::setLife(int life)
{
    _life = life;
}

int  BehaviourAbs::getLife()
{
    return _life;
}

