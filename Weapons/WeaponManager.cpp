#include "WeaponManager.h"
#include "../Physics/PhysicsManager.h"
#include "qDebug"

WeaponManager * WeaponManager::_pInstance = 0;

WeaponManager::WeaponManager()
{
}

WeaponManager::~WeaponManager()
{

}

WeaponManager *
WeaponManager::Instance()
{
    if(_pInstance == NULL)
        _pInstance = new WeaponManager();

    return _pInstance;
}

void
WeaponManager::createNewType(std::string &name, WeaponAbs::weaponType weaponType, int maxAmmo, int damange, int rgb[3])
{
    WeaponType * type = new WeaponType(name, weaponType, maxAmmo, damange, rgb);
    qDebug() << "Creating Item Type: " << name.c_str();
    _mWeaponTypes[name] = type;
}

void
WeaponManager::unload()
{
    //For eliminando types
    qDebug() << "ItemManager_UNLOAD TYPES";
    std::map<std::string, WeaponType*>::iterator itTypes = _mWeaponTypes.begin();
    while ( itTypes != _mWeaponTypes.end() )
    {
        delete(itTypes->second);
        itTypes = _mWeaponTypes.erase(itTypes);
    }
    _mWeaponTypes.clear();
}

WeaponAbs *
WeaponManager::getNewWeaponInstance(std::string weaponType, Ogre::SceneNode * owner)
{
    WeaponAbs * weapon = 0;
    std::map<std::string, WeaponType*>::iterator itTypes = _mWeaponTypes.find(weaponType);
    if(itTypes != _mWeaponTypes.end())
    {
        weapon = itTypes->second->createNewWeapon(owner);
    }

    return weapon;
}
