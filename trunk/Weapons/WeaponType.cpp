#include "WeaponType.h"
#include "Shotgun.h"
#include "Pistol.h"
#include "MachineGun.h"
#include "../Physics/PhysicsManager.h"
#include "../Physics/PhysicMotionState.h"

#include <QString>
#include <qDebug>

WeaponType::WeaponType(std::string &name, WeaponAbs::weaponType weaponType, int maxAmmo, int damange, int rgb[3])
{
    _name = name;
    _weaponType = weaponType;
    _frgb[0] = rgb[0]/255;
    _frgb[1] = rgb[1]/255;
    _frgb[2] = rgb[2]/255;
    _index   = 0;
    _maxAmmo = maxAmmo;
    _damange = damange;

}

WeaponType::~WeaponType()
{
    qDebug() << "Destruyendo itemType...";
    //destroyMaterials();
}

void
WeaponType::destroyMaterials()
{
    std::map<std::string, std::string>::iterator faceIt;

    std::vector<std::string> materialList;

    for(faceIt = _materials.begin(); faceIt != _materials.end(); faceIt++)
    {
        std::string matName =faceIt->second;
        if( std::find(materialList.begin(), materialList.end(), matName) != materialList.end() )
        {
            Ogre::MaterialManager::getSingleton().remove( _materials[faceIt->first] );
            materialList.push_back(matName);
        }
    }

    _materials.clear();
}

std::map<std::string, std::string>
WeaponType::createMaterials(std::map<std::string, std::string> textures)
{

    QString basicName;
    QString completeName;

    std::map<std::string, std::string>::iterator faceIt;

    std::map<QString, std::string> materialList;
    std::map<std::string, std::string> materialMap;

    //Optimiza simplificando el número de materiales dentro del mismo bot,
    //uniendo los materiales que usan la misma textura.

    basicName = _name.c_str();

    faceIt = textures.begin();

    for(; faceIt != textures.end(); faceIt++)
    {
        QString tex =faceIt->second.c_str();
        tex.mid(0, tex.lastIndexOf('.'));
        completeName = basicName + "_" + tex;

        //Comprobar si el material ya esta creado.
        if(materialList.find(tex) == materialList.end())
        {
            Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().create(completeName.toStdString(),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            materialPtr->setAmbient(Ogre::ColourValue(_frgb[0],_frgb[1],_frgb[2]));
            materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState(faceIt->second);


            materialList[tex] = completeName.toStdString();
            materialMap[faceIt->first] = completeName.toStdString();
        }
        else
        {
            materialMap[faceIt->first] = materialList[tex];
        }
    }
    return materialMap;
}


WeaponAbs * WeaponType::createNewWeapon(Ogre::SceneNode * owner)
{
    char weaponName[50];
    WeaponAbs * weapon;
    //Create the new name
    sprintf(weaponName, "%s_%d", _name.c_str(), _index);

    switch(_weaponType)
    {
        case WeaponAbs::Pistol:
            weapon = new Pistol(weaponName, owner,50, _maxAmmo, 0.8, _damange);
            break;

        case WeaponAbs::Shotgun:
            weapon = new Shotgun(weaponName, owner, 40, _maxAmmo, 1.5, _damange);
            break;

        case WeaponAbs::MachineGun:
            weapon = new MachineGun(weaponName, owner, 100, _maxAmmo, 0.2, _damange);
            break;
    }

    _index++;
    return weapon;
}
