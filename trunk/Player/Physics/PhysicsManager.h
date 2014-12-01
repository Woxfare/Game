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
#ifndef PhysicsManager_H
#define PhysicsManager_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <OGRE/Ogre.h>
#include "PhysicsDebugDrawer.h"

class PhysicsManager
{
public:
    PhysicsManager( void );
    ~PhysicsManager( void );
    static PhysicsManager*          Instance( void );
    void                            addCollissionPlane(Ogre::SceneNode *nodePlane );
    btRigidBody*                    addDynamicBox( Ogre::SceneNode * node, float m = 1.0f );
    btRigidBody*                    addDynamicBot( Ogre::SceneNode * node, float m = 1.0f );
    btRigidBody*                    addRigidBody( btTransform *transform, btCollisionShape * shape, btScalar mass, Ogre::SceneNode * node = NULL);
    void                            addCollisionShape( btCollisionShape * colShape );
    btDiscreteDynamicsWorld*        getDynamicsWorld( void );
    btCollisionWorld*               getCollisionWorld( void );
    btBroadphaseInterface*          getBroadphase( void );
    void                            setRootSceneNode( Ogre::SceneNode * node );
    void                            shootBox(const Ogre::Vector3 & camPosition);
    void                            stepSimulation( btScalar step, int maxSubSteps );
    void                            addImpulse(btVector3 * aVec);
    bool                            hasCollision(btRigidBody * body);
    bool                            rayCastTest(const btVector3 &Start, btVector3 &End, btVector3 &Normal);
    btAlignedObjectArray<btCollisionShape*> getCollisionShapes();

protected:
    static PhysicsManager*                  mInstance;
    btAlignedObjectArray<btCollisionShape*> mCollisionShapes;
    btBroadphaseInterface *                 mBroadphase;
    btCollisionDispatcher *                 mDispatcher;
    btConstraintSolver *                    mSolver;
    btDefaultCollisionConfiguration *       mColConfig;
    btDiscreteDynamicsWorld *               mWorld;
    Ogre::SceneNode *                       mRootSceneNode;
    PhysicsDebugDrawer *                    mDebugDrawer;
};

#endif // PhysicsManager_H
