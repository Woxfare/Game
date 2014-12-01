#include "BotBoss.h"
#include "QDebug";
#include "../../Display/OgreManager.h"

BotBoss::BotBoss(std::string name, BehaviourAbs * behaviour, textureMap materials, Ogre::Vector3 position, btRigidBodyConstructionInfo * btInfo) : BotAbs(name, btInfo)
{
    qDebug() << "Creando Bot Advanced...";
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();
    _pBotEntity = sceneManager->createEntity( name, "Head.mesh" );
    _pBotNode = sceneManager->getRootSceneNode()->createChildSceneNode( name );
    _pBotEntity->setCastShadows(true);
    _pBotNode->setPosition(position);
    _pBotNode->scale(0.9,0.9,0.9);
    _pBotNode->attachObject( _pBotEntity );

    _pBehaviour = behaviour;

    setSceneNode(_pBotNode);

    setMaterials(materials);
    _pBehaviour->initialize(_pBotNode);
}

BotBoss::~BotBoss()
{
    //Buscar y eliminar el NODO del bot.
    qDebug() << "Destruyendo Bots Advanced de Escena...";
    Ogre::SceneManager * sceneMgr = OgreManager::Instance()->getSceneManager();

    Ogre::SceneNode * node = sceneMgr->getSceneNode(_name);
    node->detachAllObjects();
    sceneMgr->destroySceneNode(node);
    sceneMgr->destroyEntity( _name );
    delete _pBehaviour;
}

void
BotBoss::setMaterials( std::map<std::string, std::map<std::string, std::string>> materials)
{

    std::map<std::string, std::map<std::string, std::string>>::iterator locationIt = materials.begin();
    std::map<std::string, std::string>::iterator faceIt;

    std::string basicName;
    std::string completeName;

    for(;locationIt != materials.end(); locationIt++)
    {
        basicName = locationIt->first + "Mat";
        faceIt = locationIt->second.begin();
        for(; faceIt != locationIt->second.end(); faceIt++)
        {
            completeName = basicName + faceIt->first;

            _pBotEntity->getSubEntity(completeName)->setMaterialName(faceIt->second);
        }
    }
}
