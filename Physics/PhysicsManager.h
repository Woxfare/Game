/*

     __    __
    / / /\ \ \ ___ _   _   ____ __ _ _ __ ___
    \ \/  \/ /| _ |\ \/ / |  __/ _` | '__/ _ \
     \  /\  / ||_|| >  <  | |_  (_| | | |  __/
      \/  \/  |___|/_/\_\|_  _|\__,_|_|  \___|
                          | |
                          |_|
Copyright (c) 2014 Fernando Perera & Alejandro Vázquez

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef PhysicsManager_H
#define PhysicsManager_H

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <OGRE/Ogre.h>
#include "PhysicsDebugDrawer.h"
#include "../entity.h"

class BotAbs;

class PhysicsManager
{
public:
    PhysicsManager( void );
    ~PhysicsManager( void );
    static PhysicsManager*          Instance( void );
    btRigidBody*                    addCollissionPlane(Ogre::SceneNode *nodePlane );
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
    entity *                        rayCastTest(const btVector3 &Start, btVector3 &End, btVector3 &Normal);
    btAlignedObjectArray<btCollisionShape*> getCollisionShapes();
    void                            addEntity(entity * ent);
    void                            removeEntity( entity * pEntity );
    void                            remove(btRigidBody * rigidBody);
	void							removeAllBots( void );
	void							removeAllRigidBodies( void );
	void							removeAllCollisionShapes( void );

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
	std::vector< BotAbs* >					mBots;
};

#endif // PhysicsManager_H
