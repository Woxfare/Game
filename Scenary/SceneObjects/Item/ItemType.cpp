#include "ItemType.h"
#include "ItemHealth.h"
#include "ItemGold.h"
#include "ItemAmmo.h"
#include "../../../Physics/PhysicsManager.h"
#include "../../../Physics/PhysicMotionState.h"

#include <QString>
#include <qDebug>

ItemType::ItemType(std::string &name, std::string &itemType, std::map<std::string, std::string> textures, int value, int rgb[3], std::map<std::string, std::string> extraAttr)
{
    _name = name;
    _itemType = itemType;
    _value = value;
    _frgb[0] = rgb[0]/255;
    _frgb[1] = rgb[1]/255;
    _frgb[2] = rgb[2]/255;
    _index   = 0;

    _materials = createMaterials(textures);
    _extraAttr = extraAttr;
}

ItemType::~ItemType()
{
    qDebug() << "Destruyendo itemType...";
    destroyMaterials();
}

void
ItemType::destroyMaterials()
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
ItemType::createMaterials(std::map<std::string, std::string> textures)
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


ItemAbs * ItemType::createNewItem(Ogre::Vector3 position)
{
    char itemName[50];
    ItemAbs * item;
    //Create the new name
    sprintf(itemName, "%s_%d", _name.c_str(), _index);

    //Creating item rigidBody
    btCollisionShape * colShape = new btBoxShape(btVector3(0.35, 0.35, 0.35));

    btTransform boxTransform;
    boxTransform.setIdentity();

    btScalar mass(1.0);
    btVector3 localInertia(0, 0, 0);


    colShape->calculateLocalInertia(mass, localInertia);

    btVector3 btPos(position.x, position.y, position.z);
    boxTransform.setOrigin(btPos);

    PhysicMotionState * motionState = new PhysicMotionState(boxTransform, NULL);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

    qDebug() << _itemType.c_str();
    //Create the item with type
    if(_itemType.compare("Health") == 0)
    {
        qDebug() << "Health";
        item = new ItemHealth(itemName, _value, position, _materials, &rbInfo);
    }
    else if(_itemType.compare("Gold") == 0)
    {
        qDebug() << "Gold";
        item = new ItemGold(itemName, _value, position, _materials, &rbInfo);
    }
    else if(_itemType.compare("Ammo") == 0)
    {
        qDebug() << "Ammo";
        item = new ItemAmmo(itemName, _value, position, _materials, &rbInfo);
        ((ItemAmmo*)item)->setWeaponName(_extraAttr["weaponName"]);
    }
    else
    {
        qDebug() << "NOT CORRECT: " << _name.c_str();
        return NULL;
    }
    _index++;

    if(item)
    {
        PhysicsManager::Instance()->addEntity(item);
    }

    return item;
}


