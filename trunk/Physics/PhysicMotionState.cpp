#include "PhysicMotionState.h"

#include "qDebug"

PhysicMotionState::PhysicMotionState(const btTransform &initialpos, Ogre::SceneNode *node, float yDesviation)
{
    mVisibleobj = node;
    mPos1 = initialpos;
    _yDesviation = yDesviation;
}

PhysicMotionState::~PhysicMotionState()
{

}

void
PhysicMotionState::setNode(Ogre::SceneNode *node)
{
    mVisibleobj = node;
}

void
PhysicMotionState::getWorldTransform(btTransform &worldTrans) const
{
    worldTrans = mPos1;
}

void
PhysicMotionState::setWorldTransform(const btTransform &worldTrans)
{
    // If we do not have visible object return
    if(!mVisibleobj)
    {
        qDebug() << "HAHAHAHAHAHAHAHAHAHAHAHAHAHAHAHHAHAHAHA";
        return;
    }
    // Get the rotation
    btQuaternion rot = worldTrans.getRotation();
    mVisibleobj->setOrientation(rot.w(), 0, rot.y(), 0);

    // Get the position
    btVector3 pos = worldTrans.getOrigin();

    mVisibleobj->setPosition(pos.x(), pos.y()-_yDesviation, pos.z());
    
}
