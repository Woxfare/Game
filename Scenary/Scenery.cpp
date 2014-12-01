#include "Scenery.h"
#include "../Display/OgreManager.h"
#include "../Physics/PhysicsManager.h"
#include "../Physics/BulletMeshStrider.h"
#include "../Physics/PhysicMotionState.h"
#include <qDebug>

Scenery * Scenery::_pInstance = 0;

Scenery::Scenery( )
{
    _mapName = "";
    _baseTextureName = "";
    _frgb[0] = 1;
    _frgb[1] = 1;
    _frgb[2] = 1;

}

Scenery::~Scenery()
{

}

Scenery * Scenery::Instance()
{
    if(!_pInstance)
        _pInstance = new Scenery();

    return _pInstance;
}

void
Scenery::initialize()
{
    if(_mapName != "")
    {
        qDebug() << "Inicializando Mapa...";
        createBasePlane(100,100);
        initializeSceneryMesh();
        qDebug() << "All Ok";
    }

}

void
Scenery::unload()
{
    qDebug() << "Descargando escenario...";
    Ogre::SceneManager* sceneMgr = OgreManager::Instance()->getSceneManager();

    std::vector<ScenerySection*>::iterator it = _scenerySections.end();
    while(it != _scenerySections.end())
    {
        PhysicsManager::Instance()->removeEntity(*it);
        it = _scenerySections.erase(it);
    }

    //Eliminar el plano

    sceneMgr->getSceneNode("BasePlane")->detachAllObjects();
    sceneMgr->destroySceneNode("BasePlane");
    sceneMgr->destroyEntity( "base_plane" );

    sceneMgr->getSceneNode("Scenery")->detachAllObjects();
    sceneMgr->destroySceneNode( "Scenery" );
    sceneMgr->destroyEntity( "Scenery" );

    sceneMgr->getSceneNode("SceneryRootNode")->detachAllObjects();
    sceneMgr->destroySceneNode("SceneryRootNode");

    Ogre::MeshManager::getSingleton().remove("base_plane");
    Ogre::MaterialManager::getSingleton().remove("base_plane");
    Ogre::MaterialManager::getSingleton().remove("base_Plane");

    std::vector<ScenerySection*>::iterator itS = _scenerySections.end();
    while(itS != _scenerySections.end())
    {
        PhysicsManager::Instance()->removeEntity(*itS);
        itS = _scenerySections.erase(itS);
    }

    //Eliminar el plano
    PhysicsManager::Instance()->remove(_basePlaneBody);
}

void
Scenery::setColor(int r, int g, int b)
{
    _frgb[0] = r/255.0f;
    _frgb[1] = g/255.0f;
    _frgb[2] = b/255.0f;
}

void
Scenery::setColor(float r, float g, float b)
{
    _frgb[0] = r;
    _frgb[0] = g;
    _frgb[0] = b;
}

void
Scenery::setBaseTextureName(std::string name)
{
    _baseTextureName = name;
}

void
Scenery::setMapName(std::string name)
{
    _mapName = name;
}

void
Scenery::addEnemyDoor(float x, float y, float z)
{
    _enemyDoors.push_back(Ogre::Vector3(x,y,z));
}

std::vector<Ogre::Vector3>
Scenery::getEnemyDoors()
{
    return _enemyDoors;
}

void
Scenery::createBasePlane(float width, float profundidad, float y)
{
    Ogre::SceneManager* pSceneManager = OgreManager::Instance()->getSceneManager();

    Ogre::ManualObject * basePlane = pSceneManager->createManualObject("base_Plane");

    qDebug() << "Creando Material";
    Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().create("base_Plane",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    materialPtr->setAmbient(Ogre::ColourValue(_frgb[0],_frgb[1],_frgb[2]));
    materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState(_baseTextureName);

    qDebug() << "Creando Plano";
    basePlane->begin("base_Plane", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    basePlane->normal(0,1,0);
    basePlane->position(width/2.0f, y, profundidad/2.0f);
    basePlane->textureCoord(1, 1);
    basePlane->position( width/2.0f, y, -profundidad/2.0f);
    basePlane->textureCoord(1, 0);
    basePlane->position( -width/2.0f, y, -profundidad/2.0f);
    basePlane->textureCoord(0, 0);
    basePlane->position(-width/2.0f, y, profundidad/2.0f);
    basePlane->textureCoord(0, 1);
    basePlane->quad(0,1,2,3);
    basePlane->end();

    qDebug() << "Convert Mesh";
    basePlane->convertToMesh("base_plane");
    qDebug() << "Destroy Manual";
    pSceneManager->destroyManualObject("base_Plane");

    qDebug() << "Creating Entity and attaching";
    _pSceneryRootNode = pSceneManager->getRootSceneNode()->createChildSceneNode( "SceneryRootNode" );
    Ogre::SceneNode * planeNode = _pSceneryRootNode->createChildSceneNode("BasePlane");
    Ogre::Entity * Plane = pSceneManager->createEntity("base_plane", "base_plane");
    planeNode->attachObject( Plane );
    planeNode->setVisible(true);

    qDebug() << "Collisions";
    _basePlaneBody = PhysicsManager::Instance()->addCollissionPlane( planeNode );
    qDebug() << "Collisions2";
}

void
Scenery::initializeSceneryMesh()
{
    qDebug() << "Collisions3";
    Ogre::SceneManager* pSceneManager = OgreManager::Instance()->getSceneManager();

    qDebug() << "Loading Scenery";
    Ogre::Entity * scenery = pSceneManager->createEntity("Scenery", _mapName + ".mesh");
    qDebug() << "Creating Scene Node";
    Ogre::SceneNode * sceneryNode = _pSceneryRootNode->createChildSceneNode("Scenery");

    sceneryNode->setPosition(0,0.5,0);
    sceneryNode->attachObject(scenery);
    sceneryNode->setVisible(true);

    qDebug() << "Hola1";
    /*Ogre::String RessourceGroupName = "ScenaryGroup";
	if(!Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(RessourceGroupName ))
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup( RessourceGroupName );
*/
    qDebug() << "Hola2";
	Ogre::MeshPtr MeshPtr = scenery->getMesh();

    qDebug() << "Hola3";
	MeshStrider* Strider = new MeshStrider(MeshPtr.get());
    qDebug() << "Hola4";
	try
	{
		btCollisionShape* CollisionShape = new btBvhTriangleMeshShape(Strider,true,true);

		Ogre::Quaternion q( sceneryNode->_getDerivedOrientation() );
		Ogre::Vector3 pos( sceneryNode->_getDerivedPosition() );

        qDebug() << "Hola5";
		btQuaternion btq( q.x, q.y, q.z,q.w );
		btVector3 btpos( pos.x, pos.y, pos.z );
		btTransform *bulletTrsf = new btTransform( btq, btpos );
        btVector3 localInertia(0, 0, 0);

        qDebug() << "Hola7";
        PhysicMotionState * motionState = new PhysicMotionState(*bulletTrsf, sceneryNode);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(1000, motionState, CollisionShape, localInertia);

        qDebug() << "Hola8";
        ScenerySection * sceneryS = new ScenerySection(&rbInfo);
        _scenerySections.push_back(sceneryS);
        PhysicsManager::Instance()->addEntity(sceneryS);
        qDebug() << "Hola9";

	} 
	catch( ... )
	{
        qDebug() << "CATCH ERROR FATAL PARA LAS FISICAS DEL ESCENARIO!!!";
	}
    qDebug() << "Hola4";
	
}

Ogre::SceneNode *
Scenery::getSceneryRootNode()
{
    return _pSceneryRootNode;
}
