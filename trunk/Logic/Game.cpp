#include "Game.h"
#include "../Display/OgreManager.h"
#include "../Player/Player.h"
#include "../Bots/BotManager.h"
#include "../Bots/BotTipes/BotSimple.h"
#include "../Bots/Behaviours/Follower.h"
#include "../Keyboard/KeyboardController.h"
#include "../Camera/CameraController.h"
#include "../Scenary/Scenery.h"
#include "../Display/BillboardManager.h"
#include "../Scenary/SceneObjects/Item/ItemManager.h"
#include "../Scenary/SceneObjects/DynamicObjects/DynamicObjectManager.h"
#include "../Physics/PhysicsManager.h"

#include <qDebug>

Game * Game::_pInstance = 0;

Game::Game( )
{
    _backgroundOn = false;
    _currentRound = 0;
    _maxEnemiesOut= 0;
    _currentEnemiesOut= 0;
    _crono= 0;
    _timeBetweenEnemyCount= 0;
    _timeBetweenItemsCount= 0;
    _bossesMax = 2;
    _bossesLastRound = 0;
    _bossesThisRound = 0;
    _exit = false;
}

Game::~Game()
{

}

Game * Game::Instance()
{
    if(!_pInstance)
        _pInstance = new Game();

    return _pInstance;
}

void
Game::initialize()
{
    qDebug() << "Iniciando juego...";
    Player::Instance()->initialize();
    CameraController::Instance()->setTarget(Player::Instance()->getSceneNode());

    Ogre::SceneManager * sceneManager = OgreManager::Instance()->getSceneManager();

    sceneManager->setAmbientLight(Ogre::ColourValue(0.6f, 0.6f, 0.6f));

    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

    Scenery * scenery = Scenery::Instance();
    scenery->initialize();

    // Create a Light and set its position
    Ogre::Light* light = sceneManager->createLight("MainLight");
    light->setPosition(20.0f, 80.0f, 50.0f);

    _crono= 0;
    _timeBetweenEnemyCount= 0;
    _timeBetweenItemsCount= 0;
    _currentRound = 0;

    //CalcularRondaInicial
    calculateRoundAttr();

    //Create scenenode Structure:
    sceneManager->getRootSceneNode()->createChildSceneNode("_Bots_");
    sceneManager->getRootSceneNode()->createChildSceneNode("_Items_");
    DynamicObjectManager::Instance()->initialise();

    //Sound initialise:
    Ogre::String name = "BSO_Game.wav";
    int idSound = OgreManager::Instance()->getSoundManager()->CreateLoopedSound(name);
    int channel = INVALID_SOUND_CHANNEL;
    OgreManager::Instance()->getSoundManager()->PlaySound(idSound, CameraController::Instance()->getCameraNode(), &channel);
    OgreManager::Instance()->getSoundManager()->GetSoundChannel(channel)->setVolume(0.1);
}

void
Game::update(double elapsedTime)
{
    //Ciclo principal del juego:
    //Actualiza Inputs
    //Actualiza Player
    //Actualiza Bots
    //RONDA:
    //EnemigoNuevo?
    //  Si:Añadir Enemigo de 1 tipo a cada puerta
    //EnemigosMuertos?
    //  Si: Fin de ronda
    //      Iniciar Tmp Espera
    //      RecalcularDatosRonda

    _crono += elapsedTime;
    _timeBetweenEnemyCount += elapsedTime;

    if(_crono > 18)
    {
        if(!_backgroundOn)
        {
            PhysicsManager::Instance()->stepSimulation(elapsedTime,100);
            KeyboardController::Instance()->update(elapsedTime);
            Player::Instance()->update(elapsedTime);
            BotManager::Instance()->update(elapsedTime);
            ItemManager::Instance()->update(elapsedTime);

            if(Player::Instance()->getLifePoints() <= 0)
            {
                try
                {
                    Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("GameLose");

                    Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","GameLoseElement"));
                    panel->setMaterialName("GameLose");
                    panel->setWidth(1);
                    panel->setHeight(1);
                    panel->setPosition(0,0);

                    overlay->add2D(panel);
                    panel->show();
                    overlay->show();
                    _delayTime = 7;
                } catch( Ogre::Exception e )
                {
                    _delayTime-=elapsedTime;
                }
                if(_delayTime < 0)
                {
                    try
                    {
                        Ogre::OverlayManager::getSingleton().destroyOverlayElement("GameLoseElement");
                        Ogre::OverlayManager::getSingleton().destroy("GameLose");
                    }
                    catch( Ogre::Exception e )
                    {
                    }
                    _exit = true;
                }
            }
            else
            {
                if(_currentRound <=_numRounds)
                {
                    if(_delayTime > 0)
                    {
                        _delayTime-=elapsedTime;
                    }
                    else
                    {
                        //Si el contador sobrepasa, añade un enemigo en cada puerta.
                        if(_bossesThisRound > 0)
                        {
                            BotManager::Instance()->addNextBoss();
                            _bossesThisRound--;
                        }
                        if(_timeBetweenEnemyCount > _timeBetweenEnemy)
                        {
                            _timeBetweenEnemyCount = 0;

                            std::vector<Ogre::Vector3> enemyDoors = Scenery::Instance()->getEnemyDoors();
                            std::vector<Ogre::Vector3>::iterator itDoor = enemyDoors.begin();

                            for(;itDoor != enemyDoors.end(); itDoor++)
                            {
                                if(_currentEnemiesOut >= _maxEnemiesOut)
                                {
                                    break;
                                }
                                BotManager::Instance()->addNextSimpleBot((*itDoor));
                                _currentEnemiesOut+=1;
                            }
                        }
                        //Si el sobrepasa el número de enemigos muertos
                        if(BotManager::Instance()->getNumKilledEnemies() == _maxEnemiesOut)
                        {
                            calculateRoundAttr();
                            BotManager::Instance()->restartNumKilledEnemies();
                            _delayTime = 5;
                        }
                    }
                }
                else
                {
                    try
                    {
                        Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("GameWin");

                        Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","GameWinElement"));
                        panel->setMaterialName("GameWin");
                        panel->setWidth(1);
                        panel->setHeight(1);
                        panel->setPosition(0,0);

                        overlay->add2D(panel);
                        panel->show();
                        overlay->show();
                        _delayTime = 5;
                    } catch( Ogre::Exception e )
                    {
                        _delayTime-=elapsedTime;
                    }
                    if(_delayTime < 0)
                    {
                        try
                        {
                            Ogre::OverlayManager::getSingleton().destroyOverlayElement("GameWinElement");
                            Ogre::OverlayManager::getSingleton().destroy("GameWin");
                        }
                        catch( Ogre::Exception e )
                        {
                        }
                        _exit = true;
                    }
                }
            }
        }
    }
    else
    {
        if(_crono > 15)
        {
            try
            {
                Ogre::OverlayManager::getSingleton().destroyOverlayElement("InitialElementImage");
                Ogre::OverlayManager::getSingleton().destroy("InitialImage");
            } catch( Ogre::Exception e )
            {
            }
        }
        else
        {
            try
            {
                Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("InitialImage");

                Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","InitialElementImage"));
                panel->setMaterialName("Initial");
                panel->setWidth(1);
                panel->setHeight(1);
                panel->setPosition(0,0);

                overlay->add2D(panel);
                panel->show();
                overlay->show();
                overlay->setZOrder(500);
            } catch( Ogre::Exception e )
            {
            }
        }
    }
}

bool
Game::getExit()
{
    return _exit;
}

void
Game::calculateRoundAttr()
{
    _currentRound += 1;

    _bossesThisRound = 0;
    if(_currentRound%(_numRounds/_bossesMax+1) == 0 || _numRounds == _currentRound)
    {

        _bossesThisRound = _bossesLastRound+1;
        if(_bossesThisRound > _bossesMax)
            _bossesThisRound = _bossesMax;
        _bossesLastRound = _bossesThisRound;
    }
    float increment = (float)(_maxAllRounds - _minAllRounds)/(float)(_numRounds);
    _maxEnemiesOut = increment*float(_currentRound-1) + _minAllRounds + _bossesThisRound;
    _currentEnemiesOut = _bossesThisRound;
}

void
Game::setBackground(bool backgroundOn)
{
    _backgroundOn = backgroundOn;
}

bool
Game::isBackground()
{
    return _backgroundOn;
}

void
Game::close()
{
    qDebug() << "Cerrando partida...";
    Player::Instance()->unload();
    BotManager::Instance()->unload();
    BillboardManager::Instance()->unload();

    OgreManager::Instance()->getSceneManager()->destroyLight("MainLight");

    qDebug() << "Partida cerrada!";
    OgreManager::Instance()->getSceneManager()->destroySceneNode("_Bots_");
    OgreManager::Instance()->getSceneManager()->destroySceneNode("_Items_");
    qDebug() << "Partida cerrada 2";
    DynamicObjectManager::Instance()->unload();
    Scenery::Instance()->unload();
}


void
Game::setMinAllRounds(int minAllRounds)
{
    _minAllRounds = minAllRounds;
}

void
Game::setMaxAllRounds(int maxAllRounds)
{
    _maxAllRounds = maxAllRounds;
}

void
Game::setTimeBetweenItems(int timeBetweenItems)
{
    _timeBetweenItems = timeBetweenItems;
}

void
Game::setTimeBetweenEnemy(int timeBetweenEnemy)
{
    _timeBetweenEnemy = timeBetweenEnemy;
}

void
Game::setBosses(int bosses)
{
    _bossesMax = bosses;
}

void
Game::setNumRounds(int numRounds)
{
    _numRounds = numRounds;
}
