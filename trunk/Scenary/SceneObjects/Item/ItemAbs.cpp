#include "ItemAbs.h"
#include "../../../Display/OgreManager.h"
#include "../../../Physics/PhysicMotionState.h"
#include "../../../Player/Player.h"
#include "qDebug"

ItemAbs::ItemAbs(std::string name, int value, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo) : entity(entity::Item, rbInfo)
{
    _name = "Item"+name;
    _value = value;

    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    _pItemEntity = sceneManager->createEntity(_name, "baseCubeMesh.mesh");
    _pItemEntity->setCastShadows(true);
    Ogre::SceneNode * boxNode = sceneManager->getSceneNode("_Items_")->createChildSceneNode(_name);
    boxNode->attachObject(_pItemEntity);
    boxNode->setPosition(position);
    boxNode->setScale(0.5,0.5,0.5);

    setMaterials(materials);

    setSceneNode(boxNode);

    btVector3 dir(0,1,0);
    applyCentralImpulse(dir*3);

}

ItemAbs::~ItemAbs()
{
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    Ogre::SceneNode * node = sceneManager->getSceneNode(_name);
    node->detachAllObjects();
    sceneManager->destroySceneNode(node);
    sceneManager->destroyEntity(_name );
}

std::string ItemAbs::getName()
{
    return _name;
}
void ItemAbs::setName( Ogre::String name )
{
    _name = name;
}
int ItemAbs::getValue()
{
    return _value;
}
void ItemAbs::setValue(int value){
    _value = value;
}

void
ItemAbs::setSceneNode( Ogre::SceneNode * itemNode )
{
    dynamic_cast<PhysicMotionState*>(getMotionState())->setNode(itemNode);
}

void
ItemAbs::setMaterials( std::map<std::string, std::string> materials)
{
    std::map<std::string, std::string>::iterator faceIt = materials.begin();

    qDebug() << "mat" << _pItemEntity->getSubEntity(0)->getMaterialName().c_str();
    for(; faceIt != materials.end(); faceIt++)
    {
        for(int i = 0; i < _pItemEntity->getNumSubEntities(); i++)
        {
            if(_pItemEntity->getSubEntity(i)->getMaterialName().compare(faceIt->first) == 0)
            {
                qDebug() << "Stting material on item";
                _pItemEntity->getSubEntity(i)->setMaterialName(faceIt->second);
                break;
            }
        }
    }

}

bool
ItemAbs::isCollected()
{
    Ogre::Vector3 playerPos = Player::Instance()->getPosition();

    btTransform itemTrans = getWorldTransform();

    btVector3 itemPos = itemTrans.getOrigin();

    float dist = playerPos.distance(Ogre::Vector3(itemPos.x(), itemPos.y(), itemPos.z()));

    if(dist > 1)
    {
        return false;
    }
    return true;
}
