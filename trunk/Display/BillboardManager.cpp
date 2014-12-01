#include "BillboardManager.h"
#include "../Display/OgreManager.h"
#include <qDebug>

BillboardManager * BillboardManager::_pInstance = 0;

BillboardManager::BillboardManager( )
{
    invisible = 1;
}

BillboardManager::~BillboardManager()
{

}

BillboardManager * BillboardManager::Instance()
{
    if(!_pInstance)
        _pInstance = new BillboardManager();

    return _pInstance;
}

void
BillboardManager::initialize()
{
}

void
BillboardManager::unload()
{
}

void
BillboardManager::update(double elapsedTime)
{
}
