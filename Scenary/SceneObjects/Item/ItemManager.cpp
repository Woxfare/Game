#include "ItemManager.h"
#include "../../../Physics/PhysicsManager.h"
#include "qDebug"

ItemManager * ItemManager::_pInstance = 0;

ItemManager::ItemManager()
{
    _vItemsInScene = std::vector<ItemAbs*>();
}

ItemManager::~ItemManager()
{

}

ItemManager *
ItemManager::Instance()
{
    if(_pInstance == NULL)
        _pInstance = new ItemManager();

    return _pInstance;
}


void
ItemManager::addItem( std::string itemName, Ogre::Vector3 position )
{
    std::map<std::string, ItemType*>::iterator it = _mItemTypes.find(itemName);
    if(it != _mItemTypes.end())
    {
        ItemAbs * item = it->second->createNewItem(position);
        if(item)
        {
            qDebug() << "ITEM NO NULO";
            _vItemsInScene.push_back(item);
        }
        else
        {
            qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXItemNulo";
        }
    }
}

void
ItemManager::update(double elapsedTime)
{
    std::vector<ItemAbs*>::iterator it = _vItemsInScene.begin();

    while( it != _vItemsInScene.end() )
    {
        qDebug() << (*it)->getName().c_str();
        if((*it)->isCollected())
        {
            qDebug() << "collected1" << (*it)->getName().c_str();
            (*it)->collected();
            qDebug() << "collected2" << (*it)->getName().c_str();
            PhysicsManager::Instance()->removeEntity((*it));
            it = _vItemsInScene.erase(it);
            qDebug() << "deleted";
        }
        else
        {
            it++;
        }
    }

}

void
ItemManager::createNewType(std::string &name, std::string &itemType, std::map<std::string, std::string> textures, int value, int rgb[3], std::map<std::string, std::string> extraAttr)
{
    qDebug() << "Creating Item Type: " << name.c_str();
    ItemType * type = new ItemType(name, itemType, textures, value, rgb, extraAttr);
    std::string nameAux = name;
    _mItemTypes[nameAux] = type;
}

void
ItemManager::unload()
{
    //FOR elminando bots
    qDebug() << "ItemManager_UNLOAD BOTS";
    std::vector<ItemAbs*>::iterator itItems = _vItemsInScene.begin();
    while ( itItems != _vItemsInScene.end() )
    {
        delete((*itItems));
        itItems = _vItemsInScene.erase(itItems);
    }
    _vItemsInScene.clear();

    //For eliminando types
    qDebug() << "ItemManager_UNLOAD TYPES";
    std::map<std::string, ItemType*>::iterator itTypes = _mItemTypes.begin();
    while ( itTypes != _mItemTypes.end() )
    {
        delete(itTypes->second);
        itTypes = _mItemTypes.erase(itTypes);
    }
    _mItemTypes.clear();
}

void
ItemManager::addRandomItem(Ogre::Vector3 position)
{
    int itemIndex = Ogre::Math::IFloor(Ogre::Math::RangeRandom(0, _mItemTypes.size() - 1));

    std::map<std::string, ItemType*>::iterator it = _mItemTypes.begin();

    std::string key;

    qDebug() << itemIndex;
    for(; it != _mItemTypes.end(); it++ )
    {
        itemIndex--;
        if(itemIndex < 0)
        {
            key = it->first;
            break;
        }
    }

    ItemManager::Instance()->addItem(key, position);
}
