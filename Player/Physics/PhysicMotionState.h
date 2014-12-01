 /*! ==================== Copyright (c) 2014 Woxfare Team =================
//  __    __
// / / /\ \ \ ___ _   _   ____ __ _ _ __ ___
// \ \/  \/ /| _ |\ \/ / |  __/ _` | '__/ _ \
//  \  /\  / ||_|| >  <  | |_  (_| | | |  __/
//   \/  \/  |___|/_/\_\|_  _|\__,_|_|  \___|
//                       | |
//                       |_|
// Members:
// Fernando Perera ( datiner )
// Alejandro Vazquez ( Vazquinhos )
// ==================== Copyright (c) 2014 Woxfare Team ==================
// Description: Motion states of physics entities
// ====================================================================== */
#ifndef PhysicMotionState_H
#define PhysicMotionState_H

// Includes
#include "Ogre.h"
#include <btBulletDynamicsCommon.h>

class PhysicMotionState : public btMotionState
{
public:
    /*!
     * \brief The motion state constructor
     * \param initialpos, The intial position
     * \param node, The node of the scene
     */
    PhysicMotionState( const btTransform &initialpos, Ogre::SceneNode *node );

    /*!
     * \brief The motion state destructor
     */
    ~PhysicMotionState();

    /*!
     * \brief Sets the node to the motion state
     * \param node
     */
    void setNode(Ogre::SceneNode *node);

    /*!
     * \brief Gets the world transformation to be seted to the scene node
     * \param worldTrans
     */
    virtual void getWorldTransform(btTransform &worldTrans) const;

    /*!
     * \brief setWorldTransform
     * \param worldTrans
     */
    virtual void setWorldTransform(const btTransform &worldTrans);

protected:
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
};

#endif // PhysicMotionState_H
