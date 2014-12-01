#include "DynamicObjectType.h"
#include "../../../Physics/PhysicsManager.h"
#include "DynamicObjectManager.h"
#include <QString>
#include <qDebug>

DynamicObjectType::DynamicObjectType(std::string &name)
{
    _name = name;
    _index = 0;
}

DynamicObjectType::DynamicObjectType(std::string &name, std::map<std::string, std::string> textures, int rgb[3], std::vector<Ogre::Vector3*> positions)
{
    _name = name;
    _index = 0;

    _frgb[0] = rgb[0];
    _frgb[1] = rgb[1];
    _frgb[2] = rgb[2];

    _vPositions = positions;
    _materials = createMaterials(textures);
}

DynamicObjectType::~DynamicObjectType()
{

}

DynamicObject * DynamicObjectType::createNewObj(Ogre::Vector3 position)
{
    char objName[50];
    DynamicObject * dynamicObject;
    //Create the new name
    sprintf(objName, "%s_%d", _name.c_str(), _index);

    //Creating item rigidBody
    btCollisionShape * colShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));

    btTransform boxTransform;
    boxTransform.setIdentity();

    btScalar mass(1.0);
    btVector3 localInertia(0, 0, 0);


    colShape->calculateLocalInertia(mass, localInertia);

    boxTransform.setOrigin(btVector3(position.x, position.y, position.z));

    PhysicMotionState * motionState = new PhysicMotionState(boxTransform, NULL);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

    dynamicObject = new DynamicObject(objName, position, _materials, &rbInfo);

    if(dynamicObject)
    {
        _index++;
        PhysicsManager::Instance()->addEntity(dynamicObject);
    }

    return dynamicObject;
}

std::map<std::string, std::string> DynamicObjectType::createMaterials(std::map<std::string, std::string> textures)
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
            if(tex != "")
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

void DynamicObjectType::destroyMaterials()
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

void
DynamicObjectType::initialise()
{
    std::vector<Ogre::Vector3*>::iterator it = _vPositions.begin();
    for(;it != _vPositions.end();it++)
    {
        DynamicObjectManager::Instance()->addDynamicObject(createNewObj((*(*it))));
    }
}
