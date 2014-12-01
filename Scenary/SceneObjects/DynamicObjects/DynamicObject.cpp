#include "DynamicObject.h"
#include "qDebug"

DynamicObject::DynamicObject(std::string name, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo) : entity(entity::DynamicObj, rbInfo)
{
    qDebug() << "Name" << name.c_str();
    _name = name;
    qDebug() << "_Name" << _name.c_str();

    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    _pDynamicObjectEntity = sceneManager->createEntity(_name, "baseCubeMesh.mesh");
    _pDynamicObjectEntity->setCastShadows(true);
    Ogre::SceneNode * boxNode = sceneManager->getSceneNode("_DynamicObjects_")->createChildSceneNode(_name);
    boxNode->attachObject(_pDynamicObjectEntity);
    boxNode->setPosition(position);

    setMaterials(materials);

    setSceneNode(boxNode);
}

DynamicObject::~DynamicObject()
{
    //DestroyEntity
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    qDebug() << "Destroy Name" << _name.c_str();

    Ogre::SceneNode * node = sceneManager->getSceneNode(_name);
    node->detachAllObjects();
    sceneManager->destroySceneNode(node);
    sceneManager->destroyEntity(_pDynamicObjectEntity );
}

void
DynamicObject::setSceneNode( Ogre::SceneNode * dynamicObjectNode )
{
    dynamic_cast<PhysicMotionState*>(getMotionState())->setNode(dynamicObjectNode);
}

void
DynamicObject::setMaterials( std::map<std::string, std::string> materials)
{
    std::map<std::string, std::string>::iterator faceIt = materials.begin();

    for(; faceIt != materials.end(); faceIt++)
    {
        for(int i = 0; i < _pDynamicObjectEntity->getNumSubEntities(); i++)
        {
            if(_pDynamicObjectEntity->getSubEntity(i)->getMaterialName().compare(faceIt->first) == 0)
            {
                _pDynamicObjectEntity->getSubEntity(i)->setMaterialName(faceIt->second);
                break;
            }
        }
    }

}
