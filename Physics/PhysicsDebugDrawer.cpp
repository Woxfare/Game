#include "PhysicsDebugDrawer.h"
#include "qDebug"
#include "PhysicsManager.h"

inline btVector3 cvt(const Ogre::Vector3 &V){
    return btVector3(V.x, V.y, V.z);
}

inline Ogre::Vector3 cvt(const btVector3&V){
    return Ogre::Vector3(V.x(), V.y(), V.z());
}

inline btQuaternion cvt(const Ogre::Quaternion &Q)
{
    return btQuaternion(Q.x, Q.y, Q.z, Q.w);
}

inline Ogre::Quaternion cvt(const btQuaternion &Q)
{
    return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
}

PhysicsDebugDrawer::PhysicsDebugDrawer( Ogre::SceneManager *scm )
{
    mContactPoints = &mContactPoints1;
    mLines = new Ogre::ManualObject("physics lines");
    assert( mLines );
    mTriangles = new Ogre::ManualObject("physics triangles");
    assert( mTriangles );
    mLines->setDynamic(true);
    mTriangles->setDynamic(true);
    //mLines->estimateVertexCount( 100000 );
    //mLines->estimateIndexCount( 0 );

    scm->getRootSceneNode()->attachObject( mLines );
    scm->getRootSceneNode()->attachObject( mTriangles );

    static const char * matName = "DebugLines";
    Ogre::MaterialPtr mtl = Ogre::MaterialManager::getSingleton().getDefaultSettings()->clone(matName);
    mtl->setReceiveShadows(false);
    mtl->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
    mtl->setDepthBias( 0.1, 0 );
    Ogre::TextureUnitState * tu = mtl->getTechnique(0)->getPass(0)->createTextureUnitState();
    assert( tu );
    tu->setColourOperationEx( Ogre::LBX_SOURCE1, Ogre::LBS_DIFFUSE );
    mtl->getTechnique(0)->setLightingEnabled(false);
    //mtl->getTechnique(0)->setSelfIllumination( ColourValue::White );

    qDebug() << "Debug Drawer Lines";
    mLines->begin( matName, Ogre::RenderOperation::OT_LINE_LIST );
    mLines->position( Ogre::Vector3::ZERO );
    mLines->colour( Ogre::ColourValue::Blue );
    mLines->position( Ogre::Vector3::ZERO );
    mLines->colour( Ogre::ColourValue::Blue );

    qDebug() << "Debug Drawer Triangles";
    mTriangles->begin( matName, Ogre::RenderOperation::OT_TRIANGLE_LIST );
    mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour( Ogre::ColourValue::Blue );
    mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour( Ogre::ColourValue::Blue );
    mTriangles->position( Ogre::Vector3::ZERO );
    mTriangles->colour( Ogre::ColourValue::Blue );

    mDebugModes = (DebugDrawModes) DBG_DrawWireframe;
    Ogre::Root::getSingleton().addFrameListener(this);

}

PhysicsDebugDrawer::~PhysicsDebugDrawer()
{
    Ogre::Root::getSingleton().removeFrameListener(this);
    delete mLines;
    delete mTriangles;
}

void PhysicsDebugDrawer::drawLine( const btVector3 &from, const btVector3 &to, const btVector3 &color )
{
    qDebug() << "DrawLine";
    Ogre::ColourValue c( color.getX(), color.getY(), color.getZ() );
    c.saturate();
    mLines->position( cvt(from) );
    mLines->colour( c );
    mLines->position( cvt(to) );
    mLines->colour( c );
}

void PhysicsDebugDrawer::drawTriangle( const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar alpha )
{
    Ogre::ColourValue c( color.getX(), color.getY(), color.getZ(), alpha );
    c.saturate();
    mTriangles->position( cvt(v0) );
    mTriangles->colour( c );
    mTriangles->position( cvt(v1) );
    mTriangles->colour( c );
    mTriangles->position( cvt(v2) );
    mTriangles->colour( c );
}

void PhysicsDebugDrawer::drawContactPoint( const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color )
{
    mContactPoints->resize( mContactPoints->size() + 1 );
    ContactPoint p = mContactPoints->back();
    p.from = cvt( PointOnB );
    p.to = p.from + cvt( normalOnB ) * distance;
    p.dieTime = Ogre::Root::getSingleton().getTimer()->getMilliseconds() + lifeTime;
    p.color.r = color.x();
    p.color.g = color.y();
    p.color.b = color.z();
}

bool PhysicsDebugDrawer::frameStarted( const Ogre::FrameEvent& evt )
{
    size_t now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();
    std::vector< ContactPoint > *newCP = mContactPoints == &mContactPoints1 ? &mContactPoints2 : &mContactPoints1;
    for ( std::vector< ContactPoint >::iterator i = mContactPoints->begin(); i < mContactPoints->end(); i++ ){
        ContactPoint &cp = *i;
        mLines->position( cp.from );
        mLines->colour( cp.color );
        mLines->position( cp.to );
        if ( now <= cp.dieTime  )
            newCP->push_back( cp );
    }
    mContactPoints->clear();
    mContactPoints = newCP;

    /*btAlignedObjectArray<btCollisionObject*> objects =
            PhysicsManager::Instance()->getDynamicsWorld()->getCollisionObjectArray();
    btRigidBody * obj;
    for(int i = 0; i < objects.size(); i++)
    {
        btVector3 min, max;
        //objects[i]->getBoundingSphere(c, r);
        obj = (btRigidBody*)objects[i];
        obj->getAabb(min, max);
        qDebug() << "MAX" << max.x() << max.y() << max.z();
        drawBox(max, min, btVector3(1,1,1));
    }*/

    //qDebug() << "Debug STARTFRAME";
    mLines->end();
    mTriangles->end();

    return true;
}

bool PhysicsDebugDrawer::frameEnded( const Ogre::FrameEvent& evt )
{
    //qDebug() << "Debug beginUpdate";
    mLines->beginUpdate(0);
    mTriangles->beginUpdate(0);
    return true;
}

void PhysicsDebugDrawer::reportErrorWarning( const char *warningString )
{
    Ogre::LogManager::getSingleton().getDefaultLog()->logMessage( warningString );
}


void PhysicsDebugDrawer::draw3dText( const btVector3 &location, const char *textString )
{

}

void PhysicsDebugDrawer::setDebugMode( int debugMode )
{
    mDebugModes = (DebugDrawModes) debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const
{
    return mDebugModes;
}
