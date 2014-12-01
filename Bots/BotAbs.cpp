    #include "BotAbs.h"
#include "../Physics/PhysicsManager.h"
#include "../Physics/PhysicMotionState.h"

#include "qDebug"

//TODO:BTRIGIDBODYSBOT
//Convertir a rigid body, usando esto al crear los obejetos en TODO:BTRIGIDBODYS
//BotAbs::BotAbs(std::string name) - btRigidBody(rbInfo)
BotAbs::BotAbs(std::string name, btRigidBodyConstructionInfo * btInfo) : entity(entity::Bot,btInfo)
{
    _name = name;
    _removeFlag = false;
}

BotAbs::~BotAbs()
{
    qDebug() << "BOTABS_DESTROY";
}

void
BotAbs::setBehaviour( BehaviourAbs * behaviour )
{
    _pBehaviour = behaviour;
}

BehaviourAbs *
BotAbs::getBehaviour()
{
    return _pBehaviour;
}

void
BotAbs::setSceneNode( Ogre::SceneNode * botNode )
{
    _pBehaviour->setPosition(botNode->getPosition().x,
                             botNode->getPosition().y,
                             botNode->getPosition().z);
    dynamic_cast<PhysicMotionState*>(getMotionState())->setNode(botNode);
    _pBotNode = botNode;
}

void
BotAbs::updateBot( double elapsedTime )
{
    _pBehaviour->update(elapsedTime, dynamic_cast<btRigidBody*>(this));
    _removeFlag = _pBehaviour->getRemoveFlag();
}

bool
BotAbs::getRemoveFlag()
{
    return _removeFlag;
}

