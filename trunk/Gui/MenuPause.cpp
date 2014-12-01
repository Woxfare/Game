#include "MenuPause.h"
#include <qDebug>

MenuPause::MenuPause() : MenuAbs()
{

}

MenuPause::~MenuPause()
{

}

void
MenuPause::enter()
{
    OgreBites::SdkTrayManager * guiManager = OgreManager::Instance()->getGuiManager();

    guiManager->createLabel(OgreBites::TL_CENTER, "Pause-PauseLbl", "PAUSE", 250);
    guiManager->createButton(OgreBites::TL_CENTER, "Pause-Resume", "Resume", 250);
    guiManager->createButton(OgreBites::TL_CENTER, "Pause-Exit", "Exit", 250);
    _isVisible = true;
}

void
MenuPause::exit()
{
    OgreBites::SdkTrayManager * guiManager = OgreManager::Instance()->getGuiManager();

    guiManager->destroyWidget("Pause-PauseLbl");
    guiManager->destroyWidget("Pause-Resume");
    guiManager->destroyWidget("Pause-Exit");
    _isVisible = false;
}

