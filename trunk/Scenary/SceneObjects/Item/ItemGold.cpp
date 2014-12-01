#include "ItemGold.h"
#include "../../../Player/Player.h"
#include <string>

ItemGold::ItemGold(std::string name, int value, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo) : ItemAbs(name, value, position, materials, rbInfo)
{
}

Ogre::String
ItemGold::message( void )
{
    return "Gold Collected";
}

void
ItemGold::collected( void )
{
    // The item will add gold to the player
    Player::Instance()->addGold( _value );
}
