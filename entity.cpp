#include "entity.h"


entity::entity(ENTITYTYPES entityType, btRigidBodyConstructionInfo * btInfo) : btRigidBody(*btInfo)
{
    _entityType = entityType;
}

entity::~entity()
{

}

entity::ENTITYTYPES entity::getType()
{
    return _entityType;
}
