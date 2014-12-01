#include "PhysicsManager.h"
#include "PhysicMotionState.h"

#include "../Display/OgreManager.h"

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
    mWorld->getCollisionWorld()->setDebugDrawer(mDebugDrawer);
}

PhysicsManager*
PhysicsManager::Instance( void )
{
    if(!mInstance)
        mInstance = new PhysicsManager();

    return mInstance;
}

PhysicsManager::~PhysicsManager( void )
{
    ///Cleanup bulletdyanmics
    ///Cleanup in the reverse order of creation/initialization
    ///Remove the rigidbodies from the dynamics world and delete them
    for (int i=mWorld->getNumCollisionObjects()-1; i>=0; i--)
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

    //delete collision shapes
    for (int j=0; j < mCollisionShapes.size(); j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        mCollisionShapes[j] = 0;
        delete shape;
    }

    delete mWorld;
    delete mSolver;
    delete mBroadphase;
    delete mDispatcher;
    delete mColConfig;
}

void
PhysicsManager::addCollissionPlane(Ogre::SceneNode *nodePlane)
{
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(1500.),btScalar(.0f),btScalar(1500.)));

    mCollisionShapes.push_back(groundShape);

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

btRigidBody*
PhysicsManager::addDynamicBot( Ogre::SceneNode * node, float m )
{
    btCollisionShape * colShape = new btBoxShape(btVector3(0.5, 0.9, 0.3));
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

btRigidBody*
PhysicsManager::addRigidBody(btTransform *transform, btCollisionShape * shape, btScalar mass, Ogre::SceneNode * node)
{
    mCollisionShapes.push_back(shape);
    btVector3 localInertia(0, 0, 0);

    shape->calculateLocalInertia(mass, localInertia);
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

bool
PhysicsManager::rayCastTest(const btVector3 &Start, btVector3 &End, btVector3 &Normal)
{
    btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);

    // Perform raycast
    mWorld->rayTest(Start, End, RayCallback);

    btVector3 direction(End.x()-Start.x(), 0.5, End.z()-Start.z());
    direction = direction.normalize();

    if(RayCallback.hasHit()) {

            End = RayCallback.m_hitPointWorld;
            Normal = RayCallback.m_hitNormalWorld;
            btRigidBody* body = (btRigidBody*)RayCallback.m_collisionObject;
            body->activate(true);
            body->applyCentralImpulse(direction*6);

            return true;
    }
    return false;
}
