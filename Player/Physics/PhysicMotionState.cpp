#include "PhysicMotionState.h"


PhysicMotionState::PhysicMotionState(const btTransform &initialpos, Ogre::SceneNode *node)
{
    mVisibleobj = node;
    mPos1 = initialpos;
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
        return;

    // Get the rotation
    btQuaternion rot = worldTrans.getRotation();
    mVisibleobj->setOrientation(rot.w(), 0, rot.y(), 0);

    // Get the position
    btVector3 pos = worldTrans.getOrigin();
    mVisibleobj->setPosition(pos.x(), pos.y()-0.9, pos.z());
}
