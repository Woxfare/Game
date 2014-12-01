#include "WeaponAbs.h"
#include "../Physics/PhysicsManager.h"
#include "../Display/OgreManager.h"

#include "qDebug"

WeaponAbs::WeaponAbs(std::string name, float maxDistance, float maxAmmo, float timeBetweenShots, int damange)
{
    _timeBetweenShots = timeBetweenShots;
    _timeNextShot = 0;
    _maxDistance = maxDistance;
    _timeAnimationShot = 0.2;
    _maxAmmo = maxAmmo;
    _currentAmmo = _maxAmmo;
    _damange = damange;
    _name = name;
    _activate = false;
    _visible = false;
}

WeaponAbs::~WeaponAbs()
{
}

std::string
WeaponAbs::getName()
{
    return _name;
}

void
WeaponAbs::addAmmo(int ammo)
{
    if(_activate)
    {
        qDebug() << "ACTIVATE TRUE";
        _currentAmmo += ammo;
        if(_currentAmmo > _maxAmmo)
            _currentAmmo = _maxAmmo;
    }
    else
        _activate = true;
}

int
WeaponAbs::getAmmo()
{
    return _currentAmmo;
}

int
WeaponAbs::getSoundIndex( void )
{
    return _soundIndex;
}

void
WeaponAbs::setVisible( bool visible)
{
    _visible = visible;
    _pWeaponNode->setVisible(visible,false);
}

bool
WeaponAbs::getActivate( void )
{
    return _activate;
}

void WeaponAbs::setActivate( bool activate )
{
    _activate = activate;
}
