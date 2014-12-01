#include "PhysicsManager.h"
#include "PhysicMotionState.h"

#include "../Bots/BotAbs.h"

#include "../Display/OgreManager.h"

#include "qDebug"

PhysicsManager * PhysicsManager::mInstance = 0;

PhysicsManager::PhysicsManager( void )
{
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    mColConfig = new btDefaultCollisionConfiguration;
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    mDispatcher = new btCollisionDispatcher(mColConfig);
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    mBroadphase = new btDbvtBroadphase;
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    mSolver = new btSequentialImpulseConstraintSolver;
    mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mColConfig);
    mWorld->setGravity(btVector3(0, -10, 0));
    mRootSceneNode = 0;
    mDebugDrawer = new PhysicsDebugDrawer(OgreManager::Instance()->getSceneManager());
    mDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    //mWorld->getCollisionWorld()->setDebugDrawer(mDebugDrawer);
}

PhysicsManager* PhysicsManager::Instance( void ) { return ( mInstance ) ? mInstance : mInstance = new PhysicsManager; }

PhysicsManager::~PhysicsManager( void )
{
    ///Cleanup bulletdyanmics
    ///Cleanup in the reverse order of creation/initialization
    ///Remove the rigidbodies from the dynamics world and delete them
    for (int i = mWorld->getNumCollisionObjects() - 1; i>=0; i--)
    {
        btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        mWorld->removeCollisionObject( obj );
        delete obj;
    }

	removeAllCollisionShapes();
    delete mWorld;
    delete mSolver;
    delete mBroadphase;
    delete mDispatcher;
    delete mColConfig;
}

void
PhysicsManager::removeAllCollisionShapes()
{
	//delete collision shapes
	for (int j=0; j < mCollisionShapes.size(); j++)
	{
		btCollisionShape* shape = mCollisionShapes[j];
		mCollisionShapes[j] = 0;
		delete shape;
	}
}

void
PhysicsManager::removeAllRigidBodies()
{
	///Cleanup in the reverse order of creation/initialization
	///Remove the rigidbodies from the dynamics world and delete them
	for (int i = mWorld->getNumCollisionObjects() - 1; i>=0; i--)
	{
		btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
			delete body->getMotionState();
		mWorld->removeCollisionObject( obj );
		delete obj;
	}
}

btRigidBody *
PhysicsManager::addCollissionPlane(Ogre::SceneNode *nodePlane)
{
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(100.),btScalar(.0f),btScalar(100.)));

    btTransform groundTransform;
    groundTransform.setIdentity();

    //This plane isnt going to be moving so i dont care about setting the motion state
    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    groundTransform.setOrigin(btVector3(nodePlane->getPosition().x, nodePlane->getPosition().y, nodePlane->getPosition().z));
    PhysicMotionState *phyMotionState = new PhysicMotionState( groundTransform, nodePlane );
    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, phyMotionState, groundShape, btVector3(0,0,0));
    btRigidBody* body = new btRigidBody(rbInfo);

    body->setFriction(5.0);

    // Add the plane to our base world
    mWorld->addRigidBody(body);

    return body;
}

btRigidBody*
PhysicsManager::addDynamicBox( Ogre::SceneNode * node, float m )
{

    btCollisionShape * colShape = new btBoxShape(btVector3(0.8, 0.8, 0.8));
    mCollisionShapes.push_back(colShape);
    btTransform boxTransform;
    boxTransform.setIdentity();

    btScalar mass(m);
    btVector3 localInertia(0, 0, 0);


    colShape->calculateLocalInertia(mass, localInertia);

    boxTransform.setOrigin(btVector3(node->getPosition().x, node->getPosition().y, node->getPosition().z));

    PhysicMotionState * motionState = new PhysicMotionState(boxTransform, node);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

    btRigidBody * body = new btRigidBody(rbInfo);


    mWorld->addRigidBody(body);

    return body;
}

void
PhysicsManager::remove(btRigidBody * rigidBody)
{
    if(rigidBody->getMotionState())
        delete rigidBody->getMotionState();


    mWorld->removeCollisionObject( dynamic_cast<btCollisionObject*>(rigidBody) );
    delete rigidBody;
    rigidBody = 0;
}

void
PhysicsManager::addEntity(entity * ent)
{
    mWorld->addRigidBody(ent);

}


void
PhysicsManager::removeEntity( entity * pEntity )
{

    if(pEntity->getType() != entity::Player)
    {
        if ( pEntity->getMotionState())
            delete pEntity->getMotionState();
        mWorld->removeCollisionObject( dynamic_cast<btCollisionObject*>(pEntity) );
        delete pEntity;
        pEntity = 0;
    }
}

void
PhysicsManager::removeAllBots()
{
	for (int i = mWorld->getNumCollisionObjects() - 1; i>=0; i--)
	{
		btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);

        BotAbs* pBot = dynamic_cast< BotAbs* >( body );

        if(!pBot)
            continue;

        if (body && body->getMotionState())
            delete body->getMotionState();

        mWorld->removeCollisionObject( obj );

        delete obj;
	}
}

btRigidBody*
PhysicsManager::addDynamicBot( Ogre::SceneNode * node, float m )
{
    //TODO:BTRIGIDBODYSBOT
    btCollisionShape * colShape = new btBoxShape(btVector3(0.5, 0.9, 0.3));
    mCollisionShapes.push_back(colShape);
    btTransform boxTransform;
    boxTransform.setIdentity();

    btScalar mass(m);
    btVector3 localInertia(0, 0, 0);

    colShape->calculateLocalInertia(mass, localInertia);

    boxTransform.setOrigin(btVector3(node->getPosition().x, node->getPosition().y, node->getPosition().z));

    PhysicMotionState * motionState = new PhysicMotionState(boxTransform, node, 0.9);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
    btRigidBody * body = new btRigidBody(rbInfo);


    mWorld->addRigidBody(body);
    return body;
}

btRigidBody*
PhysicsManager::addRigidBody(btTransform *transform, btCollisionShape * shape, btScalar mass, Ogre::SceneNode * node)
{
    mCollisionShapes.push_back(shape);
    btVector3 localInertia(0, 0, 0);

    //shape->calculateLocalInertia(mass, localInertia);
    PhysicMotionState * motionState = new PhysicMotionState(*transform, node);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody * body = new btRigidBody(rbInfo);

    mWorld->addRigidBody(body);

    return body;
}

void
PhysicsManager::addCollisionShape( btCollisionShape * colShape )
{
    mCollisionShapes.push_back(colShape);
}

btDiscreteDynamicsWorld*
PhysicsManager::getDynamicsWorld( void )
{
    return mWorld;
}

btCollisionWorld*
PhysicsManager::getCollisionWorld( void )
{
    return mWorld->getCollisionWorld();
}

btBroadphaseInterface*
PhysicsManager::getBroadphase( void )
{
    return mBroadphase;
}

void
PhysicsManager::setRootSceneNode( Ogre::SceneNode * node )
{
    mRootSceneNode = node;
}

void
PhysicsManager::shootBox(const Ogre::Vector3 & camPosition)
{
    if (mRootSceneNode)
    {
        Ogre::SceneNode * node = mRootSceneNode->createChildSceneNode(camPosition);
        btRigidBody * box = addDynamicBox(node);
        box->applyCentralImpulse(btVector3(50, 0, 0));
    }
}

void
PhysicsManager::stepSimulation(btScalar step, int maxSubSteps)
{
    mWorld->stepSimulation( step, maxSubSteps );
}

struct btDrawingResult : public btCollisionWorld::ContactResultCallback
{
    virtual	btScalar	addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
    {
        btVector3 ptA = cp.getPositionWorldOnA();
        btVector3 ptB = cp.getPositionWorldOnB();

        return 0;
    }
};

btAlignedObjectArray<btCollisionShape*>
PhysicsManager::getCollisionShapes()
{
    return mCollisionShapes;
}

bool
PhysicsManager::hasCollision(btRigidBody * body)
{
    btDrawingResult renderCallback;
    mWorld->contactTest(body,renderCallback);

    if(mWorld->getDispatcher()->getNumManifolds() != 0)
        return true;
    return false;
}

void
PhysicsManager::addImpulse(btVector3 * aVec)
{
    for (int i=mWorld->getNumCollisionObjects()-1; i>=0; i--)
    {
        btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            body->applyCentralImpulse(*aVec);
        }
    }
}

entity *
PhysicsManager::rayCastTest(const btVector3 &Start, btVector3 &End, btVector3 &Normal)
{
    btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);

    // Perform raycast
    mWorld->rayTest(Start, End, RayCallback);

    if(RayCallback.hasHit()) {

            End = RayCallback.m_hitPointWorld;
            Normal = RayCallback.m_hitNormalWorld;

            try{
                entity * pEntity = (entity*)( RayCallback.m_collisionObject );
                return pEntity;
            }
            catch (...)
            {
                qDebug() << "CATCH FATAL ERROR";
                return 0;
            }
            return 0;
    }
    return 0;
}
