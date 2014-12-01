#include "./DynamicObjectManager.h"
#include "../../../Physics/PhysicsManager.h"

#include <qDebug>


DynamicObjectManager * DynamicObjectManager::_pInstance = 0;

DynamicObjectManager *
DynamicObjectManager::Instance()
{
    if(_pInstance == 0)
        _pInstance = new DynamicObjectManager();

    return _pInstance;
}

DynamicObjectManager::DynamicObjectManager()
{

}

void
DynamicObjectManager::createNewType(std::string &name, std::map<std::string, std::string> textures, int rgb[3], std::vector<Ogre::Vector3*> positions)
{
    qDebug() << "Control 1";
    DynamicObjectType * type = new DynamicObjectType(name, textures, rgb, positions);
    qDebug() << "Control 2";
    _mDynamicObjTypes[name] = type;
}

void
DynamicObjectManager::addDynamicObject( DynamicObject * dynamicObject )
{
    _vDynamicObjInScene.push_back(dynamicObject);
}

void
DynamicObjectManager::addDynamicObject( std::string &name, Ogre::Vector3 position )
{
    std::map<std::string, DynamicObjectType*>::iterator it = _mDynamicObjTypes.find(name);
    if(it != _mDynamicObjTypes.end())
    {
        DynamicObject * obj = it->second->createNewObj(position);
        if(obj)
        {
            qDebug() << "OBJECT NO NULO";
            _vDynamicObjInScene.push_back(obj);
        }
        else
        {
            qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXOBJECTNulo";
        }
    }
}

void
DynamicObjectManager::initialise()
{
    OgreManager::Instance()->getSceneManager()->getRootSceneNode()->createChildSceneNode("_DynamicObjects_");
    std::map<std::string, DynamicObjectType*>::iterator it = _mDynamicObjTypes.begin();
    for(;it != _mDynamicObjTypes.end();it++)
    {
        it->second->initialise();
    }
}


void
DynamicObjectManager::unload()
{
    qDebug() << "Unload";
    OgreManager::Instance()->getSceneManager()->getSceneNode("_DynamicObjects_")->detachAllObjects();
    OgreManager::Instance()->getSceneManager()->destroySceneNode("_DynamicObjects_");

    std::vector<DynamicObject*>::iterator itObj = _vDynamicObjInScene.begin();
    qDebug() << "Unload" << _vDynamicObjInScene.size();
    while(itObj != _vDynamicObjInScene.end())
    {
        qDebug() << "Holamundo" << (*itObj)->getName().c_str();
        PhysicsManager::Instance()->removeEntity(*itObj);
        itObj = _vDynamicObjInScene.erase(itObj);
    }
    _vDynamicObjInScene.clear();

    std::map<std::string, DynamicObjectType*>::iterator it = _mDynamicObjTypes.begin();
    while(it != _mDynamicObjTypes.end())
    {
        delete(it->second);
        it = _mDynamicObjTypes.erase(it);
    }
    _mDynamicObjTypes.clear();
}
