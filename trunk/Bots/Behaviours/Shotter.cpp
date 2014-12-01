#include "Shotter.h"
#include "../BotManager.h"
#include "../../Player/Player.h"
#include "../../Display/OgreManager.h"
#include "../../Scenary/SceneObjects/Item/ItemManager.h"
#include "../../Weapons/WeaponManager.h"
#include "../../Physics/PhysicsManager.h"
#include "QDebug"


Shotter::Shotter(int life, int points, std::vector<std::string> itemsNames) : BehaviourAbs(life, points, itemsNames)
{
    _state = Respawn;
    _maxAttackVel = 1.5;
    _attackVel = _maxAttackVel;
}


Shotter::~Shotter()
{
    qDebug() << "Destroy Shotter";
    delete weapon;
}

void Shotter::initialize(Ogre::SceneNode * owner)
{
    _sceneNode = owner;
    weapon = WeaponManager::Instance()->getNewWeaponInstance("Arma1", _sceneNode);
}

void
Shotter::update(double elapsedTime, btRigidBody * rigidBody)
{
    qDebug() << "Update Shotter";
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
                if(_pAnimationStateBottom->getAnimationName() != "WalkBottom")
                {
                    _pAnimationStateBottom->setEnabled(false);
                    _pAnimationStateBottom = botEntity->getAnimationState("WalkBottom");
                    _pAnimationStateBottom->setEnabled(true);
                    _pAnimationStateBottom->setLoop(true);
                }
            }
            else
            {
                _pAnimationStateBottom = botEntity->getAnimationState("WalkBottom");
                _pAnimationStateBottom->setEnabled(true);
                _pAnimationStateBottom->setLoop(true);
            }
            if(_life <= 0)
            {
               _state = Dead;
            }
            else if( _vPosition.distance( pPosition) < 8 )
            {
                btVector3 origin(_vPosition.x, _vPosition.y, _vPosition.z);
                btVector3 final( pPosition.x, pPosition.y+0.5, pPosition.z );

                btVector3 normal;

                entity * ent = PhysicsManager::Instance()->rayCastTest(origin, final, normal);
                if(ent != 0 && (ent->getType() != entity::Scenery && ent->getType() != entity::Bot))
                {
                    qDebug() << "Follow To Attack";
                    _state = Attack;
                }
            }
            break;

        //-------------------- ATTACK --------------------
        case Attack:
            {
                btVector3 origin(_vPosition.x, _vPosition.y + 0.5, _vPosition.z);
                btVector3 final( pPosition.x, pPosition.y+0.5, pPosition.z );
                btVector3 normal;
                entity * ent = PhysicsManager::Instance()->rayCastTest(origin, final, normal);

                if( _vPosition.distance( pPosition ) > 8 || (ent->getType() == entity::Scenery))
                {
                    qDebug() << "Attack To Follow";
                    _attackVel = _maxAttackVel;
                    _state = Follow;
                    weapon->setVisible(false);
                }
                else
                {
                    weapon->setVisible(true);
                    if(weapon->BotShot(_vDirection))
                    {
                        if( Ogre::Math::RangeRandom(0,100) < 40 )
                            Player::Instance()->addLifePoints(-10);
                    }

                    if(_life <= 0)
                    {
                       _state = Dead;
                    }
                    if(_pAnimationStateTop->getAnimationName() != "Shotting")
                    {
                        _pAnimationStateTop->setEnabled(false);
                        _pAnimationStateTop = botEntity->getAnimationState("Shotting");
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
                }
            }
            break;

        //-------------------- DEAD --------------------
        case Dead:
            //qDebug() << "dead";
            weapon->setVisible(false);
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

    weapon->updateShot(elapsedTime);
    if(_pAnimationStateBottom != NULL)
        _pAnimationStateBottom->addTime(elapsedTime);
    if(_pAnimationStateTop != NULL)
        _pAnimationStateTop->addTime(elapsedTime);

}


