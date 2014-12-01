#include "Follower.h"
#include "../BotManager.h"
#include "../../Player/Player.h"
#include "../../Display/OgreManager.h"
#include "../../Scenary/SceneObjects/Item/ItemManager.h"
#include "QDebug"


Follower::Follower(int life, int points, std::vector<std::string> itemsNames) : BehaviourAbs(life, points, itemsNames)
{
    _state = Respawn;
    _maxAttackVel = 0.7;
    _attackVel = _maxAttackVel;
}


Follower::~Follower()
{

}

void Follower::initialize(Ogre::SceneNode * owner)
{
    _sceneNode = owner;
}

void
Follower::update(double elapsedTime, btRigidBody * rigidBody)
{
    qDebug() << "Update Follower";
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();
    Ogre::Entity * botEntity = sceneManager->getEntity(_sceneNode->getName());

    Ogre::Vector3 pPosition = Player::Instance()->getPosition();

    btTransform transRigidBody = rigidBody->getCenterOfMassTransform();

    _vPosition.x = transRigidBody.getOrigin().x();
    _vPosition.y = transRigidBody.getOrigin().y();
    _vPosition.z = transRigidBody.getOrigin().z();

    _vDirection.x = pPosition.x - _vPosition.x;
    _vDirection.y = 0;
    _vDirection.z = pPosition.z - _vPosition.z;
    _vDirection.normalise();

    //qDebug() << "UPDATE " << _life;
    switch(_state)
    {
        //-------------------- RESPAWN --------------------
        case Respawn:

            //Respawn;

            if(_pAnimationStateTop == NULL)
            {
                _pAnimationStateTop = botEntity->getAnimationState("Respawn");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(false);
            }
            else if(_pAnimationStateTop->getAnimationName() != "Respawn")
            {
                _pAnimationStateTop->setEnabled(false);
                _pAnimationStateTop = botEntity->getAnimationState("Respawn");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(false);
            }

            if(_pAnimationStateTop->getTimePosition() >= _pAnimationStateTop->getLength())
            {
                _state = Idle;
            }
            break;

        //-------------------- IDLE --------------------
        case Idle:
            if(_pAnimationStateTop->getAnimationName() != "Idle")
            {
                _pAnimationStateTop->setEnabled(false);
                _pAnimationStateTop = botEntity->getAnimationState("Idle");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(true);
            }
            if(_life <= 0)
            {
               _state = Dead;
            }
            else if( _vPosition.distance( pPosition) < 20 )
            {
                qDebug() << "Idle To Follow";
                _state = Follow;
            }
            break;

        //-------------------- FOLLOW --------------------
        case Follow:
            //qDebug() << "FOLLOW";
            //qDebug() << "BOT LINEAR VEL " << rigidBody->getLinearVelocity().x() << rigidBody->getLinearVelocity().y() << rigidBody->getLinearVelocity().z();
            if(abs(rigidBody->getLinearVelocity().x()) < 2 && abs(rigidBody->getLinearVelocity().z()) < 2)
            {
                rigidBody->applyCentralForce(btVector3(_vDirection.x*7,
                                                         0.5,
                                                         _vDirection.z*7));
            }
            rigidBody->activate(true);
            if(_pAnimationStateTop->getAnimationName() != "WalkTop")
            {
                _pAnimationStateTop->setEnabled(false);
                _pAnimationStateTop = botEntity->getAnimationState("WalkTop");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(true);
            }
            if(_pAnimationStateBottom != NULL)
            {
                if(_pAnimationStateBottom->getAnimationName() != "StopBottom")
                {
                    _pAnimationStateBottom->setEnabled(false);
                    _pAnimationStateBottom = botEntity->getAnimationState("StopBottom");
                    _pAnimationStateBottom->setEnabled(true);
                    _pAnimationStateBottom->setLoop(true);
                }
            }
            else
            {
                _pAnimationStateBottom = botEntity->getAnimationState("StopBottom");
                _pAnimationStateBottom->setEnabled(true);
                _pAnimationStateBottom->setLoop(true);
            }
            if(_life <= 0)
            {
               _state = Dead;
            }
            else if( _vPosition.distance( pPosition) < 1.5 )
            {
                qDebug() << "Follow To Attack";
                _state = Attack;
            }
            break;

        //-------------------- ATTACK --------------------
        case Attack:
            if(_attackVel < 0)
            {
                _attackVel = _maxAttackVel;;
                Player::Instance()->addLifePoints(-10);
            }
            else
                _attackVel -= elapsedTime;

            if(_pAnimationStateTop->getAnimationName() != "Attack")
            {
                _pAnimationStateTop->setEnabled(false);
                _pAnimationStateTop = botEntity->getAnimationState("Attack");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(true);
            }
            if(_pAnimationStateBottom != NULL)
            {
                if(_pAnimationStateBottom->getAnimationName() != "StopBottom")
                {
                    _pAnimationStateBottom->setEnabled(false);
                    _pAnimationStateBottom = botEntity->getAnimationState("StopBottom");
                    _pAnimationStateBottom->setEnabled(true);
                    _pAnimationStateBottom->setLoop(true);
                }
            }
            else
            {
                _pAnimationStateBottom = botEntity->getAnimationState("StopBottom");
                _pAnimationStateBottom->setEnabled(true);
                _pAnimationStateBottom->setLoop(true);
            }
            if( _vPosition.distance( pPosition) > 1.5)
            {
                qDebug() << "Attack To Follow";
                _attackVel = _maxAttackVel;
                _state = Follow;
            }
            break;

        //-------------------- DEAD --------------------
        case Dead:
            //qDebug() << "dead";
            if(_pAnimationStateBottom != NULL)
            {
                _pAnimationStateBottom->setEnabled(false);
                _pAnimationStateBottom = NULL;
            }
            if(_pAnimationStateTop->getAnimationName() != "Death.001")
            {
                _pAnimationStateTop->setEnabled(false);
                _pAnimationStateTop = botEntity->getAnimationState("Death.001");
                _pAnimationStateTop->setEnabled(true);
                _pAnimationStateTop->setLoop(false);
            }

            if(_pAnimationStateTop->getTimePosition() >= _pAnimationStateTop->getLength())
                    _state = Out;
            break;

        //-------------------- OUT --------------------
        case Out:
            //qDebug() << "out";
            Player::Instance()->addGold(_points);
            _sceneNode->setVisible(false);
            _removeFlag = true;
            if(Ogre::Math::RangeRandom(0, 1) < _itemProb)
            {
                createRandomItem();
            }
            break;
    }

    Ogre::Vector3 originalPosition(0,0,-1);

    Ogre::Quaternion rot = originalPosition.getRotationTo(_vDirection);

    btQuaternion rotation = btQuaternion(0,rot.y,0, rot.w);

    transRigidBody.setRotation(rotation) ;
    rigidBody->setCenterOfMassTransform(transRigidBody);

    if(_pAnimationStateTop != NULL)
        _pAnimationStateTop->addTime(elapsedTime);

    if(_pAnimationStateBottom != NULL)
        _pAnimationStateBottom->addTime(elapsedTime);

}


