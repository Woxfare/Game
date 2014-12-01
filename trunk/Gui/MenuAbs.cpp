#include "MenuAbs.h"
#include <qDebug>

MenuAbs::MenuAbs()
{
    _isVisible = false;
    _aspectRatio = (float)(OgreManager::Instance()->getWindow()->getWidth())/((float)(OgreManager::Instance()->getWindow()->getHeight()));
}

MenuAbs::~MenuAbs()
{

}

bool
MenuAbs::isVisible()
{
    return _isVisible;
}
