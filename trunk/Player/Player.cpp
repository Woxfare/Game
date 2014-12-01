#include "Player.h"
#include "../Display/OgreManager.h"
#include <qDebug>
#include "../Physics/PhysicsManager.h"
#include "../Weapons/WeaponManager.h"
#include "../Weapons/Pistol.h"


Player * Player::_pInstance = 0;

Player::Player( ) :
    jumping( 0 ) ,
    _increment(0),
    _vDirectionToMove( Ogre::Vector3(1,0,0) ),
    _vOrientation(Ogre::Vector3( cos(90.f), 0, sin( 90.f ) ) ),
    _vPosition( Ogre::Vector3(0,10,0) ),
    _totalGold( 0 ),
    _lifePoints( 100 ),
    _maxLifePoints( 100 ),
    _currentWeapon( NULL ),
    _defaultWeapon( NULL )
{
    Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("Damange");

    _racha = 1;
    _timeRacha=0;
    Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","DamangeElement"));
    panel->setMaterialName("damange");
    panel->setPosition(0,0);

    overlay->add2D(panel);
    overlay->hide();

    _soundIndex = OgreManager::Instance()->getSoundManager()->CreateLoopedSound(Ogre::String("Hearbeat.mp3"));
    _soundChannel = INVALID_SOUND_CHANNEL;
}

Player::~Player()
{
  if(_currentWeapon)
    delete _currentWeapon;
  if(_defaultWeapon)
    delete _defaultWeapon;
}

Player * Player::Instance() { return (_pInstance) ? _pInstance : _pInstance = new Player();}

void Player::addLifePoints(int lifePointsInc)
{
    if(lifePointsInc < 0)
    {
        ((Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().getOverlayElement("DamangeElement")))
                ->setColour(Ogre::ColourValue(1,1,1,1));
        Ogre::OverlayManager::getSingleton().getByName("Damange")->show();
    }

    if(_lifePoints != 0)
        _lifePoints += lifePointsInc;

    if(_lifePoints < 0)
    {
        _lifePoints = 0;
    }
    else if(_lifePoints > _maxLifePoints)
    {
        _lifePoints = _maxLifePoints;
    }

    if(_lifePoints < 20 && _soundChannel == INVALID_SOUND_CHANNEL)
    {
        OgreManager::Instance()->getSoundManager()->PlaySound(_soundIndex, _pPlayerNode, &_soundChannel);
    }
    else if(_lifePoints >= 20 && _soundChannel != INVALID_SOUND_CHANNEL)
    {
        OgreManager::Instance()->getSoundManager()->StopSound(&_soundChannel);
        _soundChannel = INVALID_SOUND_CHANNEL;
    }

}

void
Player::setMaterials(textureMap textures)
{
    _mMaterials = createMaterials(textures);
}


int Player::getLifePoints()
{
    return _lifePoints;
}

Ogre::Vector3
Player::getPosition( void )
{
    return _vPosition;
}

void
Player::setPosition( Ogre::Vector3 position )
{
    _vPosition = position;
}

void
Player::setPosition( float x, float y, float z )
{
    _vPosition.x = x;
    _vPosition.y = y;
    _vPosition.z = z;
}

void
Player::setWeaponsList(std::vector<std::string> weapons)
{
    _vWeaponsNameList = weapons;
}

void Player::setRGB(int r,int g,int b)
{
    _frgb[0] = r/255.0f;
    _frgb[1] = g/255.0f;
    _frgb[2] = b/255.0f;
}

void
Player::destroyMaterials()
{
    std::map<std::string, std::map<std::string, std::string>>::iterator locationIt;
    std::map<std::string, std::string>::iterator faceIt;

    std::vector<QString> materialList;

    for(locationIt = _mMaterials.begin(); locationIt != _mMaterials.end(); locationIt++)
    {
        faceIt = locationIt->second.begin();
        for(; faceIt != locationIt->second.end(); faceIt++)
        {
            QString matName =faceIt->second.c_str();
            if( std::find(materialList.begin(), materialList.end(), matName) != materialList.end() )
            {
                Ogre::MaterialManager::getSingleton().remove( _mMaterials[locationIt->first][faceIt->first] );
                materialList.push_back(matName);
            }
        }
    }

    _mMaterials.clear();
}

Player::textureMap
Player::createMaterials(textureMap textures)
{

    QString basicName;
    QString completeName;

    textureMap::iterator locationIt = textures.begin();
    std::map<std::string, std::string>::iterator faceIt;

    std::map<QString, std::string> materialList;
    textureMap materialMap;

    //Optimiza simplificando el número de materiales dentro del mismo bot,
    //uniendo los materiales que usan la misma textura.

    basicName = _name.c_str();

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
                if(tex != "")
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

void
Player::initializeMaterials()
{

    textureMap::iterator locationIt = _mMaterials.begin();
    std::map<std::string, std::string>::iterator faceIt;

    std::string basicName;
    std::string completeName;

    for(;locationIt != _mMaterials.end(); locationIt++)
    {
        basicName = locationIt->first + "Mat";
        faceIt = locationIt->second.begin();
        for(; faceIt != locationIt->second.end(); faceIt++)
        {
            completeName = basicName + faceIt->first;

            _pPlayerEntity->getSubEntity(completeName)->setMaterialName(faceIt->second);
        }
    }
}

void
Player::initialize()
{
    qDebug() << "Inicializando player";
    Ogre::SceneManager * sceneMgr = OgreManager::Instance()->getSceneManager();

    _pPlayerEntity = sceneMgr->createEntity( "PLAYER_", "Head.mesh" );
    _pPlayerNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "PLAYERNODE_" );
    _pPlayerNode->scale(0.45,0.45,0.45);
    _pPlayerNode->setPosition(_vPosition);
    _pPlayerNode->attachObject( _pPlayerEntity );
    _pPlayerEntity->setCastShadows(true);

    _pAnimationStateTop = _pPlayerEntity->getAnimationState("WalkTop");
    _pAnimationStateTop->setEnabled(false);

    _pAnimationStateTop = _pPlayerEntity->getAnimationState("Shotting");
    _pAnimationStateTop->setEnabled(true);
    _pAnimationStateTop->setLoop(true);

    _pAnimationStateBottom = _pPlayerEntity->getAnimationState("WalkBottom");
    _pAnimationStateBottom->setEnabled(true);
    _pAnimationStateBottom->setLoop(true);


    if( _vWeaponsNameList.size() == 0 )
    {
        _defaultWeapon = new Pistol("DefaultWeapon", _pPlayerNode,10, 40, 0.8, 50);
        _vWeapon.push_back(_defaultWeapon);
    }
    else
    {
        for(_indexCurrentWeapon = 0; _indexCurrentWeapon < _vWeaponsNameList.size(); _indexCurrentWeapon++)
        {
            _vWeapon.push_back( WeaponManager::Instance()->getNewWeaponInstance(_vWeaponsNameList[_indexCurrentWeapon], _pPlayerNode));
        }
        _indexCurrentWeapon = 0;
        _currentWeapon = _vWeapon[_indexCurrentWeapon];
        _currentWeapon->setActivate(true);
        _currentWeapon->setVisible(true);
    }

    _pRigidBody = PhysicsManager::Instance()->addDynamicBot(_pPlayerNode);

    _lifePoints = _maxLifePoints;
    _racha = 1;
    _shotCount = 0;
    _currentShotMax = 2;
    jumping = 0;

    initializeMaterials();
}

void
Player::unload()
{
    qDebug() << "Descargando Player...";
    Ogre::SceneManager * sceneMgr = OgreManager::Instance()->getSceneManager();

    PhysicsManager::Instance()->remove(_pRigidBody);
    Ogre::SceneNode * node = sceneMgr->getSceneNode("PLAYERNODE_");
    node->detachAllObjects();
    sceneMgr->destroySceneNode(node);
    sceneMgr->destroyEntity( "PLAYER_" );

    std::vector<WeaponAbs*>::iterator itWeapon = _vWeapon.begin();
    while ( itWeapon != _vWeapon.end() )
    {
        delete((*itWeapon));
        itWeapon = _vWeapon.erase(itWeapon);
    }
    _vWeapon.clear();
    destroyMaterials();
}

void
Player::update(double elapsedTime)
{
    qDebug() << "Update Player";
    Ogre::Vector3 direction = _vDirectionToMove;
    Ogre::Vector3 frontDirection(0,0,1);
    Ogre::Matrix3 mat;
    float vel = 5;

    frontDirection.getRotationTo(_vOrientation).Inverse().ToRotationMatrix(mat);

    direction = direction*mat;

    _pRigidBody->activate(true);
    _pRigidBody->setLinearVelocity(btVector3(direction.x*vel, _pRigidBody->getLinearVelocity().y() ,direction.z*vel));

    Ogre::Vector3 originalPosition(0,0,-1);
    _vOrientation.y = 0;

    Ogre::Quaternion rot = originalPosition.getRotationTo(_vOrientation);

    btQuaternion rotation = btQuaternion(0,rot.y,0, rot.w);

    btTransform transRigidBody = _pRigidBody->getCenterOfMassTransform();
    transRigidBody.setRotation(rotation) ;
    _pRigidBody->setCenterOfMassTransform(transRigidBody);

    _vPosition = _pPlayerNode->getPosition();

    //**** Damange ****//
    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().getOverlayElement("DamangeElement"));
    if(panel->getColour().a > 0)
    {
        panel->setColour(Ogre::ColourValue(1,1,1,panel->getColour().a - elapsedTime*2));
        panel->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, panel->getColour().a);
    }
    else
    {
        Ogre::OverlayManager::getSingleton().getByName("Damange")->hide();
    }

    _currentWeapon->updateShot(elapsedTime);

    //SELECTOR DE ANIMACION
    if(jumping == 1.5 || (_pAnimationStateTop->getAnimationName() == "Jump" && _pAnimationStateTop->getEnabled()))
    {
        qDebug() << "Jump";
        if(_pAnimationStateTop->getAnimationName() != "Jump")
        {
            _pAnimationStateTop->setEnabled(false);
            _pAnimationStateTop = sceneManager->getEntity("PLAYER_")->getAnimationState("Jump");
            _pAnimationStateTop->setTimePosition(0);
            _pAnimationStateTop->setLoop(false);
            _pAnimationStateTop->setEnabled(true);
        }
        else
        {
            if(_pAnimationStateTop->getTimePosition() == _pAnimationStateTop->getLength())
                _pAnimationStateTop->setEnabled(false);
        }
    }
    else if(_vDirectionToMove.isZeroLength())
    {
        if(_pAnimationStateTop->getAnimationName() != "Shotting")
        {
            _pAnimationStateTop->setEnabled(false);
            _pAnimationStateTop = sceneManager->getEntity("PLAYER_")->getAnimationState("Shotting");
            _pAnimationStateTop->setLoop(false);
            _pAnimationStateTop->setEnabled(true);
        }
        qDebug() << "Direction0";
        if(_pAnimationStateBottom->getAnimationName() != "StopBottom")
        {
            _pAnimationStateBottom->setEnabled(false);
            _pAnimationStateBottom = sceneManager->getEntity("PLAYER_")->getAnimationState("StopBottom");
            _pAnimationStateBottom->setLoop(false);
            _pAnimationStateBottom->setEnabled(true);
        }
    }
    else
    {
        if(_pAnimationStateTop->getAnimationName() != "Shotting")
        {
            _pAnimationStateTop->setEnabled(false);
            _pAnimationStateTop = sceneManager->getEntity("PLAYER_")->getAnimationState("Shotting");
            _pAnimationStateTop->setLoop(true);
            _pAnimationStateTop->setEnabled(true);
        }
        qDebug() << "Walk";
        if(_pAnimationStateBottom->getAnimationName()!="WalkBottom")
        {
            _pAnimationStateBottom->setEnabled(false);
            _pAnimationStateBottom = sceneManager->getEntity("PLAYER_")->getAnimationState("WalkBottom");
            _pAnimationStateBottom->setLoop(true);
            _pAnimationStateBottom->setEnabled(true);
        }
    }


    _timeRacha-=elapsedTime;
    jumping -= elapsedTime;
    if(_timeRacha < 0)
    {
        _timeRacha = 0;
        _racha = 1;
        _shotCount = 0;
        _currentShotMax = 2;
    }
    if(_pAnimationStateTop != NULL)
        _pAnimationStateTop->addTime(elapsedTime);
    if(_pAnimationStateBottom != NULL)
        _pAnimationStateBottom->addTime(elapsedTime);

}

void
Player::jump()
{
    if(jumping < 0)
    {
        jumping = 1.5;
        _pRigidBody->applyCentralImpulse(btVector3(0,4.5,0));
    }
}

void
Player::setDirectionToMove(float x, float y, float z)
{
    _vDirectionToMove.x = x;
    _vDirectionToMove.y = y;
    _vDirectionToMove.z = z;
}

void
Player::setOrientation(float x, float y, float z)
{
    _vOrientation.x = x;
    _vOrientation.y = y;
    _vOrientation.z = z;
}

void
Player::addRotation(float increment)
{
    _increment +=increment;
    _vOrientation.x = cos(90 - _increment);
    _vOrientation.y = 0;
    _vOrientation.z = sin(90 - _increment);
}

Ogre::SceneNode *
Player::getSceneNode()
{
    return OgreManager::Instance()->getSceneManager()->getSceneNode("PLAYERNODE_");
}

void
Player::shot()
{
    if(_currentWeapon->PlayerShot(_vPosition, _vOrientation))
    {
        _timeRacha = 8;
        if(_shotCount < _currentShotMax )
        {
            _shotCount++;
        }
        else
        {
            _racha++;
            _shotCount = 0;
            _currentShotMax = _currentShotMax*_currentShotMax;
        }
    }
}

void Player::addAmmoToWeapon( std::string weaponName, int value)
{
    std::vector<WeaponAbs*>::iterator it = _vWeapon.begin();
    qDebug() << _vWeapon.size();
    while(it != _vWeapon.end())
    {
        QString wName = (*it)->getName().c_str();
        wName = wName.mid(0, wName.lastIndexOf('_'));
        qDebug() << "weapon" << wName;
        qDebug() << weaponName.c_str();
        if(weaponName == wName.toStdString())
        {
            qDebug() << "WTF";
            (*it)->addAmmo(value);
            break;
        }
        it++;
    }
}

int Player::getRacha( void )
{
    return _racha;
}

WeaponAbs *
Player::getCurrentWeapon()
{
    return _currentWeapon;
}

void
Player::setDefaultWeapon( WeaponAbs* weapon )
{
    _defaultWeapon = weapon;
}
Ogre::String
Player::getName()
{
    return _name;
}

void
Player::setName(Ogre::String name)
{
    _name = name;
}

void
Player::setMaxLifePoints( int points )
{
    _maxLifePoints = points;
}

void
Player::addGold( int gold )
{
    _totalGold += gold*_racha;
}

int
Player::getGold( void )
{
    return _totalGold;
}

void
Player::changeWeaponUp()
{
    _currentWeapon->setVisible(false);
    int oldIndex = _indexCurrentWeapon;
    do{
        qDebug() << "Hola";
        _indexCurrentWeapon++;
        if(_indexCurrentWeapon >= _vWeapon.size())
            _indexCurrentWeapon = 0;
    }while(!_vWeapon[_indexCurrentWeapon]->getActivate() && oldIndex != _indexCurrentWeapon);

    _currentWeapon = _vWeapon[_indexCurrentWeapon];
    _currentWeapon->setVisible(true);
}

void
Player::changeWeaponDown()
{
    _currentWeapon->setVisible(false);
    int oldIndex = _indexCurrentWeapon;
    do
    {
        qDebug() << "Hola";
        _indexCurrentWeapon--;
        if(_indexCurrentWeapon < 0)
            _indexCurrentWeapon = _vWeapon.size()-1;
    }while(!_vWeapon[_indexCurrentWeapon]->getActivate() && oldIndex != _indexCurrentWeapon);

    _currentWeapon = _vWeapon[_indexCurrentWeapon];
    _currentWeapon->setVisible(true);
}

