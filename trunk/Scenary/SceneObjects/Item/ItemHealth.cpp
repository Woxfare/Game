#include "ItemHealth.h"
#include "../../../Player/Player.h"
#include <string>

ItemHealth::ItemHealth(std::string name, int value, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo) : ItemAbs(name, value, position, materials,  rbInfo)
{

}

Ogre::String
ItemHealth::message( void )
{
    return "Life Collected";
}

void
ItemHealth::collected( void )
{
    // The item will add life to the player
    Player::Instance()->addLifePoints( _value );
}
