#include "BotType.h"
#include "../Physics/PhysicsManager.h"
#include "./BotTipes/BotSimple.h"
#include "./BotTipes/BotBoss.h"
#include "./Behaviours/Follower.h"
#include "./Behaviours/Shotter.h"
#include "qDebug"
#include "../Physics/PhysicMotionState.h"

BotType::BotType(std::string &name, std::string &botType, std::string &botBehaviour, textureMap textures, int life, int points, int rgb[3])
{
    qDebug() << "Creando botType...";
    qDebug() << life;
    _botName = name;
    _botType = botType;
    _botBehaviour = botBehaviour;
    _index = 0;
    _life = life;
    _points = points;
    _frgb[0] = rgb[0]/255;
    _frgb[1] = rgb[1]/255;
    _frgb[2] = rgb[2]/255;

    _materials = createMaterials(textures);
}

BotType::~BotType()
{
    qDebug() << "Destruyendo botType...";
    destroyMaterials();
}

void BotType::addItemName(std::string itemName)
{
    _vItemsNames.push_back(itemName);
}

void BotType::setItemNames(std::vector<std::string> itemsNames)
{
    _vItemsNames = itemsNames;
}

std::string BotType::getBotType()
{
    return _botType;
}

void
BotType::destroyMaterials()
{
    std::map<std::string, std::map<std::string, std::string>>::iterator locationIt;
    std::map<std::string, std::string>::iterator faceIt;

    std::vector<QString> materialList;

    for(locationIt = _materials.begin(); locationIt != _materials.end(); locationIt++)
    {
        faceIt = locationIt->second.begin();
        for(; faceIt != locationIt->second.end(); faceIt++)
        {
            QString matName =faceIt->second.c_str();
            if( std::find(materialList.begin(), materialList.end(), matName) != materialList.end() )
            {
                Ogre::MaterialManager::getSingleton().remove( _materials[locationIt->first][faceIt->first] );
                materialList.push_back(matName);
            }
        }
    }

    _materials.clear();
}

textureMap
BotType::createMaterials(textureMap textures)
{

    QString basicName;
    QString completeName;

    std::map<std::string, std::map<std::string, std::string>>::iterator locationIt = textures.begin();
    std::map<std::string, std::string>::iterator faceIt;

    std::map<QString, std::string> materialList;
    std::map<std::string, std::map<std::string, std::string>> materialMap;

    //Optimiza simplificando el número de materiales dentro del mismo bot,
    //uniendo los materiales que usan la misma textura.

    basicName = _botName.c_str();

    for(;locationIt != textures.end(); locationIt++)
    {
        faceIt = locationIt->second.begin();
        for(; faceIt != locationIt->second.end(); faceIt++)
        {
            QString tex =faceIt->second.c_str();
            tex.mid(0, tex.lastIndexOf('.'));
            completeName = basicName + "_" + tex;

            //Comprobar si el material ya esta creado.
            if(materialList.find(tex) == materialList.end())
            {
                Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().create(completeName.toStdString(),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                materialPtr->setAmbient(Ogre::ColourValue(_frgb[0],_frgb[1],_frgb[2]));
                if(faceIt->second != "")
                    materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState(faceIt->second);


                materialList[tex] = completeName.toStdString();
                materialMap[locationIt->first][faceIt->first] = completeName.toStdString();
            }
            else
            {
                materialMap[locationIt->first][faceIt->first] = materialList[tex];
            }
        }
    }
    return materialMap;
}

BotAbs *
BotType::createNewBot(Ogre::Vector3 position)
{
    char botname[50];
    BehaviourAbs * behaviour;
    BotAbs * bot;

    //Create the new name
    sprintf(botname, "%s_%d", _botName.c_str(), _index);

    qDebug() << botname;
    //Create the behaviour
    if(_botBehaviour.compare("Follower") == 0)
    {
        qDebug() << "Follower";
        behaviour = new Follower(_life, _points, _vItemsNames);
    }
    else if(_botBehaviour.compare("Shotter")==0)
    {
        qDebug() << "Shotter";
        behaviour = new Shotter(_life, _points, _vItemsNames);
    }
    else
    {
        qDebug() << "NO SHOTTER NO FOLLOWER: " << _botBehaviour.c_str();
        return NULL;
    }
    //Create the bot
    if(_botType.compare("Simple")==0)
    {
        qDebug() << "Simple";
        //Ogre::Vector3 positionProvisional(Ogre::Math::RangeRandom(-10,10),10,Ogre::Math::RangeRandom(-10,10));
        //Inicializar Physicas
        btCollisionShape * colShape = new btBoxShape(btVector3(0.5, 0.9, 0.3));
        //mCollisionShapes.push_back(colShape);
        btTransform boxTransform;
        boxTransform.setIdentity();

        btScalar mass(1);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        boxTransform.setOrigin(btVector3(position.x, position.y, position.z));

        PhysicMotionState * motionState = new PhysicMotionState(boxTransform, NULL, 0.9);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

        bot = new BotSimple(botname, behaviour, _materials, position, &rbInfo);

        PhysicsManager::Instance()->addEntity(bot);
    }
    else if(_botType.compare("Advanced")==0)
    {
        qDebug() << "Boss";
        //Ogre::Vector3 positionProvisional(Ogre::Math::RangeRandom(-10,10),10,Ogre::Math::RangeRandom(-10,10));
        //Inicializar Physicas
        btCollisionShape * colShape = new btBoxShape(btVector3(1, 1.8, 0.6));
        //mCollisionShapes.push_back(colShape);
        btTransform boxTransform;
        boxTransform.setIdentity();

        btScalar mass(1);
        btVector3 localInertia(0, 0, 0);

        colShape->calculateLocalInertia(mass, localInertia);

        boxTransform.setOrigin(btVector3(position.x, position.y, position.z));

        PhysicMotionState * motionState = new PhysicMotionState(boxTransform, NULL, 1.8);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);

        bot = new BotBoss(botname, behaviour, _materials, position, &rbInfo);

        PhysicsManager::Instance()->addEntity(bot);

    }
    else
        return NULL;

    //Increment Index
    _index++;

    return bot;
}
