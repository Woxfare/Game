#include "ItemAmmo.h"
#include <string>
#include "../../../Player/Player.h"
#include <qDebug>

ItemAmmo::ItemAmmo(std::string name, int value, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo) : ItemAbs(name, value, position, materials, rbInfo)
{
}

Ogre::String
ItemAmmo::message( void )
{
    return "Ammo Collected";
}

void
ItemAmmo::collected( void )
{
    Player::Instance()->addAmmoToWeapon(_weaponName, _value);
}

void
ItemAmmo::setWeaponName(std::string weaponName)
{
    qDebug() << weaponName.c_str();
    _weaponName = weaponName;
}

