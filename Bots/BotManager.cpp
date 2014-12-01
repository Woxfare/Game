#include "BotManager.h"
#include "../Physics/PhysicsManager.h"
#include "qDebug"

BotManager * BotManager::_pInstance = 0;

BotManager::BotManager()
{
    _vBotsInScene = std::vector<BotAbs*>();
    _vBotTypes = std::map<std::string, BotType*>();
    _itNextSimpleBot = _vBotTypes.begin();
    _itNextBossBot = _vBotTypes.begin();
    _killedEnemies = 0;
}

BotManager::~BotManager()
{

}

BotManager *
BotManager::Instance()
{
    if(_pInstance == NULL)
        _pInstance = new BotManager();

    return _pInstance;
}


void
BotManager::addBot( std::string botName, Ogre::Vector3 position )
{
    std::map<std::string, BotType*>::iterator it = _vBotTypes.find(botName);
    if(it != _vBotTypes.end())
    {
        BotAbs * bot = it->second->createNewBot(position);
        if(bot)
        {
            qDebug() << "BOT NO NULO";
            _vBotsInScene.push_back(bot);
        }
        else
        {
            qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXBotNulo";
        }
    }
}

void
BotManager::addNextSimpleBot( Ogre::Vector3 position )
{
    //Comprobar el tipo:
    std::map<std::string, BotType *>::iterator itbegin = _itNextSimpleBot;

    while(_itNextSimpleBot->second->getBotType() != "Simple")
    {
        if(_itNextSimpleBot == _vBotTypes.end())
            _itNextSimpleBot = _vBotTypes.begin();

        qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ" <<_itNextSimpleBot->second->getBotType().c_str();
        _itNextSimpleBot++;

        if(_itNextBossBot == _vBotTypes.end())
            _itNextBossBot = _vBotTypes.begin();
        qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ" <<_itNextSimpleBot->second->getBotType().c_str();

        if(_itNextSimpleBot == itbegin)
        {
            qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZ LIADA";
            break;
        }
    }

    BotAbs * bot = _itNextSimpleBot->second->createNewBot(position);
    if(bot)
    {
        qDebug() << "BOT NO NULO";
        _vBotsInScene.push_back(bot);
    }
    else
    {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXBotNulo";
    }

    //incrementar al siguiente bot simple.
    _itNextSimpleBot++;

    if(_itNextSimpleBot == _vBotTypes.end())
        _itNextSimpleBot = _vBotTypes.begin();

}

void
BotManager::addNextBoss()
{
    //Comprobar el tipo:
    std::map<std::string, BotType *>::iterator itbegin = _itNextBossBot;

    while(_itNextBossBot->second->getBotType() != "Advanced")
    {
        if(_itNextBossBot == _vBotTypes.end())
            _itNextBossBot = _vBotTypes.begin();

        qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ" <<_itNextBossBot->second->getBotType().c_str();
        _itNextBossBot++;

        if(_itNextBossBot == _vBotTypes.end())
            _itNextBossBot = _vBotTypes.begin();
        qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ" <<_itNextBossBot->second->getBotType().c_str();

        if(_itNextBossBot == itbegin)
        {
            qDebug() << "ZZZZZZZZZZZZZZZZZZZZZZZZ LIADA";
            break;
        }
    }

    Ogre::Vector3 position(Ogre::Math::RangeRandom(-20,20),10,Ogre::Math::RangeRandom(-20,20));
    BotAbs * bot = _itNextBossBot->second->createNewBot(position);
    if(bot)
    {
        qDebug() << "BOT NO NULO";
        _vBotsInScene.push_back(bot);
    }
    else
    {
        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXBotNulo";
    }

    //incrementar al siguiente bot simple.
    _itNextBossBot++;

    if(_itNextBossBot == _vBotTypes.end())
        _itNextBossBot = _vBotTypes.begin();
}

void
BotManager::restartNumKilledEnemies()
{
    _killedEnemies = 0;
}

int
BotManager::getNumKilledEnemies()
{
   return _killedEnemies;
}

void
BotManager::update(double elapsedTime)
{
    //for para todos los bots
    std::vector<BotAbs*>::iterator it = _vBotsInScene.begin();
    while( it != _vBotsInScene.end())
    {
        (*it)->updateBot(elapsedTime);
        //Si el bot activa el flag de remove*, eliminarlo de la lista.
        //  ** Un bot activa su flag de remove cuando acaba la animacion de muerte.
        if((*it)->getRemoveFlag())
        {
            qDebug() << "Bot death del";

            PhysicsManager::Instance()->removeEntity(*it);
            qDebug() << "ERASE OK";
            it = _vBotsInScene.erase(it);
            _killedEnemies +=1;

        }
        else
        {
            it++;
        }
    }
}

void
BotManager::createNewType(std::string &name, std::string &botType, std::string &botBehaviour, std::vector<std::string> itemsNames, textureMap textures, int life, int points, int rgb[3])
{
    qDebug() << life;
    BotType * type = new BotType(name, botType, botBehaviour, textures, life, points, rgb);
    type->setItemNames(itemsNames);
    qDebug() << "Creating Type: " << name.c_str();
    _vBotTypes[name] = type;
    _itNextSimpleBot = _vBotTypes.begin();
    _itNextBossBot = _vBotTypes.begin();
}

void
BotManager::unload()
{
    //FOR elminando bots
    qDebug() << "BOTMANAGER_UNLOAD BOTS";
    std::vector<BotAbs*>::iterator itBots = _vBotsInScene.begin();
    while ( itBots != _vBotsInScene.end() )
    {
        delete((*itBots));
        itBots = _vBotsInScene.erase(itBots);
    }
    _vBotsInScene.clear();

    //For eliminando types
    qDebug() << "BOTMANAGER_UNLOAD TYPES";
    std::map<std::string, BotType*>::iterator itTypes = _vBotTypes.begin();
    while ( itTypes != _vBotTypes.end() )
    {
        delete(itTypes->second);
        itTypes = _vBotTypes.erase(itTypes);
    }
    _vBotTypes.clear();
}
