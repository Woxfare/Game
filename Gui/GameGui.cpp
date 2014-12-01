#include "GameGui.h"
#include <qDebug>
#include "../Player/Player.h"
#include "../Bots/BotManager.h"

GameGui::GameGui() : MenuAbs()
{
    _totalTime = 0;
    _index = 0;
}

GameGui::~GameGui()
{

}

void
GameGui::enter()
{

    if(!_isVisible)
    {
        Ogre::Overlay * overlay = Ogre::OverlayManager::getSingleton().create("UserGui");

        Ogre::PanelOverlayElement * panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","GoldElement"));
        panel->setMaterialName("Gold");
        panel->setWidth(0.04);
        panel->setHeight(0.04*_aspectRatio);
        panel->setPosition(0.95,0.145);

        overlay->add2D(panel);

        panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","HealthElement"));
        panel->setMaterialName("Health");
        panel->setWidth(0.04);
        panel->setHeight(0.04*_aspectRatio);
        panel->setPosition(0.95,0.03);

        overlay->add2D(panel);

        panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","DeadEnemiesElement"));
        panel->setMaterialName("DeadEnemies");
        panel->setWidth(0.04);
        panel->setHeight(0.04*_aspectRatio);
        panel->setPosition(0.95,0.09);

        overlay->add2D(panel);

        panel = (Ogre::PanelOverlayElement *)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel","CurrentWeaponElement"));
        panel->setMaterialName("PistolType");
        panel->setWidth(0.15);
        panel->setHeight(0.15*_aspectRatio);
        panel->setPosition(0.85,0.7);

        overlay->add2D(panel);

        overlay->show();
    }
    _isVisible = true;
}

void
GameGui::exit()
{
    if(_isVisible)
    {
        Ogre::OverlayManager::getSingleton().destroyOverlayElement("GoldElement");
        RemoveNumericTextures("Gold");
        Ogre::OverlayManager::getSingleton().destroyOverlayElement("HealthElement");
        RemoveNumericTextures("Health");
        Ogre::OverlayManager::getSingleton().destroyOverlayElement("DeadEnemiesElement");
        RemoveNumericTextures("DeadEnemies");
        Ogre::OverlayManager::getSingleton().destroyOverlayElement("CurrentWeaponElement");
        RemoveNumericTextures("CurrentAmmo");

        Ogre::OverlayManager::getSingleton().destroy("UserGui");
    }
    _isVisible = false;
}

void
GameGui::update(double elapsedTime)
{
    updateNumericTexture("Gold", "NumericTextYellow", Ogre::OverlayManager::getSingleton().getByName("UserGui"), Player::Instance()->getGold(),0.9, 0.145);
    updateNumericTexture("Health", "NumericTextGreen",Ogre::OverlayManager::getSingleton().getByName("UserGui"), Player::Instance()->getLifePoints(),0.9, 0.025);
    updateNumericTexture("DeadEnemies", "NumericTextRed", Ogre::OverlayManager::getSingleton().getByName("UserGui"), BotManager::Instance()->getNumKilledEnemies(),0.9, 0.085);
    updateNumericTexture("CurrentAmmo", "NumericTextBrown", Ogre::OverlayManager::getSingleton().getByName("UserGui"), Player::Instance()->getCurrentWeapon()->getAmmo(), 0.96,0.9);

    updateRacha(Ogre::OverlayManager::getSingleton().getByName("UserGui"));
    //Update weapon info
    switch(Player::Instance()->getCurrentWeapon()->getType())
    {
        case WeaponAbs::Pistol:
            Ogre::OverlayManager::getSingleton().getOverlayElement("CurrentWeaponElement")->setMaterialName("PistolType");
            break;
        case WeaponAbs::MachineGun:
            Ogre::OverlayManager::getSingleton().getOverlayElement("CurrentWeaponElement")->setMaterialName("MachineGunType");
            break;
        case WeaponAbs::Shotgun:
            Ogre::OverlayManager::getSingleton().getOverlayElement("CurrentWeaponElement")->setMaterialName("ShotgunType");
            break;
    }

}

void
GameGui::RemoveNumericTextures(std::string baseName)
{
    int index=0;
    Ogre::PanelOverlayElement * panel;

    Ogre::OverlayManager * overlayManager = Ogre::OverlayManager::getSingletonPtr();

    std::stringstream overlayEName;
    overlayEName << baseName;
    overlayEName << "Num_";
    overlayEName << index;

    try
    {
        panel = (Ogre::PanelOverlayElement *)overlayManager->getOverlayElement(overlayEName.str());
    }
    catch(...)
    {
        panel = 0;
    }

    while(panel)
    {
        overlayManager->destroyOverlayElement(overlayEName.str());

        index++;

        overlayEName.clear();
        overlayEName << baseName;
        overlayEName << "Num_";
        overlayEName << index;

        try
        {
            panel = (Ogre::PanelOverlayElement *)overlayManager->getOverlayElement(overlayEName.str());
        }
        catch(...)
        {
            panel = 0;
        }

    }
}

void
GameGui::updateNumericTexture(std::string baseName, std::string texName, Ogre::Overlay * overlay, int value, float initialX, float initialY, int sentido)
{
    //Represetar Integer con texturas:
    //Simbolos : 0 1 2 3 4 5 6 7 8 9
    //Todos en una misma textura, separada en 10 partes iguales.
    int currentValue=value;
    int num=0;
    int index=0;
    Ogre::PanelOverlayElement * panel=0;

    Ogre::OverlayManager * overlayManager = Ogre::OverlayManager::getSingletonPtr();

    std::stringstream overlayEName;

    if(value >= 0)
    {
        do
        {
            num = currentValue % 10;
            overlayEName.clear();
            overlayEName << baseName;
            overlayEName << "Num_";
            overlayEName << index;

            try
            {
                panel = (Ogre::PanelOverlayElement*)overlayManager->getOverlayElement(overlayEName.str());
            }
            catch(Ogre::Exception e)
            {
                panel = (Ogre::PanelOverlayElement *)(overlayManager->createOverlayElement("Panel", overlayEName.str()));
                panel->setMaterialName(texName);
                panel->setWidth(0.04);
                panel->setHeight(0.04*_aspectRatio);
                panel->setPosition(initialX-index*0.04*sentido,initialY);
                overlay->add2D(panel);
            }
            panel->setUV(num*0.1, 0, num*0.1 + 0.1, 1);
            currentValue = currentValue/10;
            index++;
        }while(currentValue != 0);

        while(panel != 0)
        {
            overlayEName.clear();
            overlayEName << baseName;
            overlayEName << "Num_";
            overlayEName << index;

            try
            {
                panel = (Ogre::PanelOverlayElement*)overlayManager->getOverlayElement(overlayEName.str());

                overlayManager->destroyOverlayElement(panel);
                panel = 0;
            }catch( ... )
            {
                panel = 0;
            }
            index++;
        }
    }
}

void
GameGui::updateRacha(Ogre::Overlay * overlay)
{
    int racha = Player::Instance()->getRacha();
    Ogre::OverlayManager * overlayManager = Ogre::OverlayManager::getSingletonPtr();

    Ogre::PanelOverlayElement * panel;
    try
    {
        panel = (Ogre::PanelOverlayElement*)overlayManager->getOverlayElement("RachaNum");
        if( racha == 1)
        {
            overlayManager->destroyOverlayElement(panel);
            panel = 0;
        }
        else
        {
            panel->setUV((float)(racha - 2)/3.0f, 0, (racha - 2)/3.0f + 0.33, 1);
            qDebug() << "Change1E";
        }
    } catch ( ... )
    {
        if( racha != 1)
        {
            panel = (Ogre::PanelOverlayElement *)(overlayManager->createOverlayElement("Panel", "RachaNum"));
            panel->setMaterialName("RachaNum");
            panel->setWidth(0.15);
            panel->setHeight(0.15*_aspectRatio);
            panel->setPosition(0.02, 0.8);
            overlay->add2D(panel);
            panel->setUV((float)(racha - 2)/3.0f, 0, (racha - 2)/3.0f + 0.33, 1);
        }
    }
}
