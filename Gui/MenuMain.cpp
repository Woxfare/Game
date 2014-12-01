#include "MenuMain.h"
#include <qDebug>

MenuMain::MenuMain() : MenuAbs()
{

}

MenuMain::~MenuMain()
{

}

void
MenuMain::enter()
{
    OgreBites::SdkTrayManager * guiManager = OgreManager::Instance()->getGuiManager();

    Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("LogoWoxFare");

    Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","LogoWoxFareElement"));
    panel->setMaterialName("MainBackground");
    panel->setPosition(0,0);

    overlay->add2D(panel);
    overlay->show();



    guiManager->createLabel(OgreBites::TL_CENTER, "Main-MainLbl", "Main Menu", 250);
    guiManager->createButton(OgreBites::TL_CENTER, "Main-Load", "Load", 250);
    guiManager->createButton(OgreBites::TL_CENTER, "Main-Exit", "Exit", 250);

    _isVisible = true;
}

void
MenuMain::exit()
{
    OgreBites::SdkTrayManager * guiManager = OgreManager::Instance()->getGuiManager();

    Ogre::OverlayManager::getSingleton().destroyOverlayElement("LogoWoxFareElement");
    Ogre::OverlayManager::getSingleton().destroy("LogoWoxFare");

    guiManager->destroyWidget("Main-MainLbl");
    guiManager->destroyWidget("Main-Load");
    guiManager->destroyWidget("Main-Exit");

    _isVisible = false;
}
