#include "LogicLoader.h"

#include <QFile>
#include <QFileDialog>
#include <QApplication>
#include <QDialog>
#include <QDebug>
#include <QXmlStreamReader>

#include "../Player/Player.h"
#include "../Bots/BotManager.h"
#include "../Scenary/Scenery.h"
#include "../Scenary/SceneObjects/Item/ItemManager.h"
#include "../Scenary/SceneObjects/DynamicObjects/DynamicObjectManager.h"
#include "../Weapons/WeaponManager.h"
#include "Game.h"

LogicLoader * LogicLoader::_pInstance = 0;
LogicLoader* LogicLoader::Instance(){ return (_pInstance) ? _pInstance : _pInstance = new LogicLoader; }

LogicLoader::LogicLoader() : gameLoaded( false ), _path_cnf_file("") {}


bool
LogicLoader::isGameLoaded()
{
    return gameLoaded;
}

void
LogicLoader::load( void )
{
    if(gameLoaded)
        clear();

    int i = 1;
    QApplication app(i,NULL);
    QDialog* dialog = new QDialog();

    QString absFilename = QFileDialog::getOpenFileName(dialog, "Open a Game file", "./mediaOgre/games", "Woxfare Game ( *.wxf" );

    if( absFilename == "" )
        return;

    _path_cnf_file = absFilename.left(absFilename.lastIndexOf("/")) + ("/");

    QFile file(absFilename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

    QXmlStreamReader xml;

    xml.setDevice( &file );

    while (!xml.atEnd())
    {
        if( xml.isStartElement() )
        {
          qDebug() << xml.name().toString();
          if(xml.name() == "player")
          {
              qDebug() << "playerLoading...";
              loadPlayer( xml );
              qDebug() << "playerLoading...END";
          }

          if(xml.name() == "dynamicobjects")
          {
              qDebug() << "dynamicLoading...";
              loadDynamicObjects( xml );
              qDebug() << "dynamicLoading...END";
          }

          if(xml.name() == "textures")
          {
            qDebug() << "texLoading...";
            loadTextures( xml );
            qDebug() << "texLoading...END";
          }

          if(xml.name() == "item")
          {
            qDebug() << "itemLoading...";
            loadItem( xml );
            qDebug() << "itemLoading...END";
          }

          if(xml.name() == "enemy")
          {
            qDebug() << "enemyLoading...";
            loadEnemyType( xml );
            qDebug() << "enemyLoading...END";
          }

          if(xml.name() == "map")
          {
            //Color, textura, nombre mesh mapa, doorPos
              loadMap( xml );
          }

          if(xml.name() == "game")
          {
            qDebug() << "gameLoading...";
            loadGame( xml );
            qDebug() << "gameLoading...END";
          }
          if(xml.name() == "weapons")
          {
              qDebug() << "weaponsLoading...";
              loadWeapons( xml );
              qDebug() << "weaponsLoading...END";
          }

          xml.readNext();
        } else {
          xml.readNext();
        }
    }

    if (xml.hasError())
        gameLoaded = false;
    else
        gameLoaded = true;

    file.close();
}

void
LogicLoader::loadTextures(QXmlStreamReader& xml)
{
  xml.readNext();

  while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "textures"))
  {
    if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
    {
      xml.readNext(); // next loop
      continue;
    }

    if( xml.name() == "texture")
    {
      QString pixmapName = xml.attributes().value("name").toString();
      QPixmap pixmap(_path_cnf_file + pixmapName );

      if( pixmap.isNull() )
      {
          continue;
      }

      QString path = "./mediaOgre/materials/textures/";
      pixmap.save(path + pixmapName, "PNG");
      xml.readNext();
    }
    xml.readNext();
  }
}

void
LogicLoader::loadPlayer(QXmlStreamReader& xml)
{

  Player * player = Player::Instance();
  player->setName( xml.attributes().value("name").toString().toStdString() );
  xml.readNext();
  int r,g,b;

  Player::textureMap textures;
  std::vector<std::string> weapons;

  while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "player"))
  {
    if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
    {
      xml.readNext(); // next loop
      continue;
    }

    if( xml.name() == "color")
    {
      parseColor( xml, r, g, b );
    }

    if( xml.name() == "life")
    {

      player->setMaxLifePoints( xml.attributes().value("value").toString().toInt() );
      xml.readNext();
    }

    if( xml.name() == "textures")
    {
      parseCharacterTextures( xml, textures );
    }

    if( xml.name() == "weapon")
    {

        //Añadimos lista de armas posibles, las armas se cargaran en el player
        //En el momento de la inicialización.
        weapons.push_back(xml.attributes().value("name").toString().toStdString());
    }

    xml.readNext();
  }

  //TODO: Coger las texturas y el color y modificar la mesh del jugador

  player->setRGB(r,g,b);
  player->setMaterials(textures);
  player->setWeaponsList(weapons);
}

void
LogicLoader::loadWeapons(QXmlStreamReader& xml)
{
    int r, g, b, maxBullets, damange;
    WeaponAbs::weaponType type;
    std::string name;

    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "weapons"))
    {
        if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
        {
          xml.readNext(); // next loop
          continue;
        }

        if(xml.name() == "weapon")
        {
            name = xml.attributes().value("name").toString().toStdString();
            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "weapon"))
            {
                if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
                {
                  xml.readNext(); // next loop
                  continue;
                }

                if( xml.name() == "color" )
                {
                    parseColor( xml, r, g, b );
                }

                if( xml.name() == "type" )
                {
                    if(xml.attributes().value("value").toString() == "SimpleShot")
                    {
                        type = WeaponAbs::Pistol;
                    }
                    else if(xml.attributes().value("value").toString() == "MultipleShot")
                    {
                        type = WeaponAbs::Shotgun;
                    }
                    else if(xml.attributes().value("value").toString() == "RapidShot")
                    {
                        type = WeaponAbs::MachineGun;
                    }
                }

                if( xml.name() == "color" )
                {
                    parseColor( xml, r, g, b );
                }

                if( xml.name() == "features" )
                {
                    maxBullets = xml.attributes().value("bullets").toString().toInt();
                    damange = xml.attributes().value("bullets").toString().toInt();
                }
                xml.readNext();
            }

            int rgb[3];
            rgb[0] = r;
            rgb[1] = g;
            rgb[2] = b;

            WeaponManager::Instance()->createNewType(name, type, maxBullets, damange, rgb );
        }
        xml.readNext();
    }
}

void
LogicLoader::loadEnemyType(QXmlStreamReader& xml)
{
    /*********************MODELO************************************
     *<enemy name="e1" folder="DefaultEnemies">
          <color r="0" g="170" b="0"/>
          <life value="10"/>
          <points value="0"/>
          <type value="BigBoss"/>
          <item value="Muni"/>
          <weapon name="Arma1"/>
          <textures>
              <Head Back="" Bottom="" Front="" Left="" Right="" Top=""/>
              <Body Back="" Bottom="" Front="" Left="" Right="" Top=""/>
              <ArmL Back="" Bottom="" Front="" Left="" Right="" Top=""/>
              <ArmR Back="" Bottom="" Front="" Left="" Right="" Top=""/>
              <LegL Back="" Bottom="" Front="" Left="" Right="" Top=""/>
              <LegR Back="" Bottom="" Front="" Left="" Right="" Top=""/>
          </textures>
          <type botType="Simple" behaviour="Follower"/>
     *</enemy>
     *******************************************************************/

  int r,g,b, life, points;
  std::string type;
  std::string behaviour;
  std::vector<std::string> itemsNames;
  textureMap textures;

  std::string name = xml.attributes().value("name").toString().toStdString();
  xml.readNext();

  while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "enemy"))
  {
    if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
    {
      xml.readNext(); // next loop
      continue;
    }

    else if( xml.name() == "color")
    {
      parseColor( xml, r, g, b );
    }

    else if( xml.name() == "life")
    {
        life = xml.attributes().value("value").toString().toInt();
    }

    else if( xml.name() == "points")
    {
        points = xml.attributes().value("value").toString().toInt();
    }

    else if( xml.name() == "textures")
    {
      parseCharacterTextures( xml, textures );
    }

    else if( xml.name() == "weapon")
    {

    }
    else if(xml.name() == "item")
    {
        itemsNames.push_back( xml.attributes().value("value").toString().toStdString());
    }
    else if( xml.name() == "type")
    {
        if( "BigBoss" == xml.attributes().value("value").toString())
        {
            type = "Advanced";
            behaviour = "Shotter";
        }
        if( "JuniorBoss" == xml.attributes().value("value").toString())
        {
            type = "Advanced";
            behaviour = "Follower";
        }
        if( "Runner" == xml.attributes().value("value").toString())
        {
            type = "Simple";
            behaviour = "Follower";
        }
        if( "Gunner" == xml.attributes().value("value").toString())
        {
            type = "Simple";
            behaviour = "Shotter";
        }
    }

    xml.readNext();
  }

  qDebug() << points;
  qDebug() << life;
  int rgb[3] = {r,g,b};
  qDebug() << "BEHAVIOUR TYPE: " << behaviour.c_str();
  BotManager::Instance()->createNewType(name, type, behaviour, itemsNames, textures, life, points, rgb );

}

void
LogicLoader::loadDynamicObjects(QXmlStreamReader& xml)
{
    Ogre::String objName, type;
    int r,g,b;
    std::map<std::string, std::string> textures;
    std::vector<Ogre::Vector3*> positions;

    xml.readNext();

    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "dynamicobjects"))
    {
        if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
        {
            xml.readNext(); // next loop
            continue;
        }

        if(xml.name() == "cube")
        {
            objName = xml.attributes().value("name").toString().toStdString();

            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "cube"))
            {
                if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
                {
                    xml.readNext(); // next loop
                    continue;
                }

                if( xml.name() == "color")
                {
                    parseColor( xml, r, g, b );
                }
                if( xml.name() == "textures")
                {
                    parseCubeTextures( xml, textures );
                }
                if( xml.name() == "pos")
                {
                   int x,y,z;
                   x = xml.attributes().value("x").toString().toInt();
                   y = xml.attributes().value("y").toString().toInt();
                   z= xml.attributes().value("z").toString().toInt();
                   positions.push_back(new Ogre::Vector3(x,y,z));
                }
                xml.readNext(); // next loop
            }
            int rgb[3];
            rgb[0] = r;
            rgb[1] = g;
            rgb[2] = b;
            qDebug() << "Estoy aqui";
            DynamicObjectManager::Instance()->createNewType(objName, textures, rgb, positions);
            positions.clear();
        }
        xml.readNext();
   }

}

//***

void
LogicLoader::loadItem(QXmlStreamReader& xml)
{
    std::string itemName = xml.attributes().value("name").toString().toStdString();
    std::string type;
    std::string weaponName;
    int value = 0;
    int r,g,b;
    std::map<std::string, std::string> textures;

    xml.readNext();

    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "item"))
    {
        if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
        {
            xml.readNext(); // next loop
            continue;
        }

        if( xml.name() == "color")
        {
            parseColor( xml, r, g, b );
        }

        if( xml.name() == "type")
        {
            type = xml.attributes().value("value").toString().toStdString();
            value = xml.attributes().value("quantity").toString().toInt();
            if(type == "Ammo")
                weaponName = xml.attributes().value("weapon").toString().toStdString();

        }
        if( xml.name() == "textures")
        {
            parseCubeTextures( xml, textures );
        }
        xml.readNext(); // next loop
   }

    int rgb[3];
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;

    std::map<std::string, std::string> extraAttr;
    qDebug() << weaponName.c_str();
    extraAttr["weaponName"] = weaponName;
    qDebug() << itemName.c_str();
    ItemManager::Instance()->createNewType(itemName, type, textures, value, rgb, extraAttr);
}

void
LogicLoader::loadMap( QXmlStreamReader &xml )
{
    /*
    <map name="MapaTest" folder="DefaultMaps">
        <color r="255" g="255" b="255"/>
        <terrain texture="terrain_texture.png"/>
        <materials>
            <material name="cajaMetalAdornada.png_254_254_254"/>
            <material name="8640003215_8071ace2f6_o.png_254_254_254"/>
            <material name="crate.png_254_254_254"/>
        </materials>
        <objects>
            ...
        </objects>
        <doors>
            <pos x="6" y="1" z="3"/>
            <pos x="-9" y="0" z="3"/>
        </doors>
    </map>
    */

    QString nameMap = xml.attributes().value("name").toString();
    QString nameTexture;
    int r, g, b;

    xml.readNext();

    Scenery * scenery = Scenery::Instance();

    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "map"))
    {
        if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
        {
            xml.readNext(); // next loop
            continue;
        }

        if( xml.name() == "color")
        {
            parseColor( xml, r, g, b );
        }

        if( xml.name() == "material")
        {
            std::string materialName = xml.attributes().value("name").toString().toStdString();
            std::string tex = xml.attributes().value("texture").toString().toStdString();
            int r,g,b;
            parseColor( xml, r, g, b );

            Ogre::MaterialManager * matManager = Ogre::MaterialManager::getSingletonPtr();
            Ogre::MaterialPtr mat = (Ogre::MaterialPtr) matManager->create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            if(tex != "")
            {
                mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
                mat->getTechnique(0)->getPass(0)->createTextureUnitState(tex);
            }
            mat->getTechnique(0)->getPass(0)->setAmbient(r/255.0f, g/255.0f, b/255.0f );
        }

        if( xml.name() == "terrain")
        {
            nameTexture = xml.attributes().value("texture").toString();
        }

        if( xml.name() == "doors")
        {
            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "doors"))
            {
                if( !(xml.tokenType() == QXmlStreamReader::EndElement) && xml.name() == "pos" )
                {
                    int x = xml.attributes().value("x").toString().toInt();
                    int y = xml.attributes().value("y").toString().toInt();
                    int z = xml.attributes().value("z").toString().toInt();

                    scenery->addEnemyDoor(x, y+0.5, z);
                }
                xml.readNext();
            }
        }

        if( xml.name() == "player")
        {
            int x = xml.attributes().value("x").toString().toInt();
            int y = xml.attributes().value("y").toString().toInt();
            int z = xml.attributes().value("z").toString().toInt();

            Player::Instance()->setPosition(x,y+0.5,z);
        }

        xml.readNext();
    }

    //Copiar el arxivo .mesh
    QString path = "./mediaOgre/models/";

    copyFile(path, nameMap + ".mesh", nameMap + ".mesh");


    qDebug() << nameMap;
    scenery->setMapName(nameMap.toStdString());
    qDebug() << nameTexture;
    scenery->setBaseTextureName(nameTexture.toStdString());
    scenery->setColor(r,g,b);
}

void
LogicLoader::loadGame(QXmlStreamReader &xml)
{
    /*<game name="Test" folder="DefaultGames" rounds="5">
        <enemies number="2" min="1" max="5" bosses="1" interval="1">
            <enemy name="Picachu"/>
            <enemy name="PWR"/>
        </enemies>
        <items number="1" interval="5">
            <item name="ItemTest"/>
        </items>
        <player name="PowerRanger"/>
        <map name="MapaTest"/>
    </game>*/


    int itemNumber, itemInterval;
    int minAllRounds, maxAllRounds, numRounds, enemyInterval, bosses;

    numRounds = xml.attributes().value("rounds").toString().toInt();

    while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "map"))
    {
        if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
        {
            xml.readNext(); // next loop
            continue;
        }


        if( xml.name() == "enemies")
        {
            minAllRounds = xml.attributes().value("min").toString().toInt();
            maxAllRounds = xml.attributes().value("max").toString().toInt();
            bosses = xml.attributes().value("bosses").toString().toInt();
            enemyInterval = xml.attributes().value("interval").toString().toInt();
        }

        if( xml.name() == "items")
        {
            itemInterval = xml.attributes().value("interval").toString().toInt();

            itemNumber = xml.attributes().value("number").toString().toInt();
        }

        xml.readNext();
    }

    Game * game = Game::Instance();
    game->setMaxAllRounds(maxAllRounds);
    game->setMinAllRounds(minAllRounds);
    game->setTimeBetweenEnemy(enemyInterval);
    game->setTimeBetweenItems(itemInterval);
    game->setBosses(bosses);
    game->setNumRounds(numRounds);
}

void
LogicLoader::copyFile(QString path, QString filenameSource, QString filenameDest )
{

    QFile f(_path_cnf_file + filenameSource );
    QFile fs(path + filenameDest );

    f.open(QIODevice::ReadOnly);
    fs.open(QIODevice::WriteOnly);

    if(!(f.isReadable() && fs.isWritable()))
    {
        //Error
        qDebug() << "Error";
        return;
    }

    while( !f.atEnd() )
    {
       QByteArray data = f.read(32);
       fs.write(data,data.size());
    }
    f.close();
    fs.close();
}

void
LogicLoader::clear()
{
    //TODO: Clear all the stuff
}

void
LogicLoader::parseColor( QXmlStreamReader& xml, int& r, int& g, int& b )
{
  r = xml.attributes().value("r").toString().toInt();
  g = xml.attributes().value("g").toString().toInt();
  b = xml.attributes().value("b").toString().toInt();
}

void
LogicLoader::parseCharacterTextures( QXmlStreamReader& xml, std::map<std::string, std::map<std::string, std::string>>& ar_textures )
{
  while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "textures"))
  {
    QString debugName = xml.name().toString();
    if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "")
    {
      xml.readNext(); // next loop
      continue;
    }

    if( xml.name() == "Head" )
    {
      std::map<std::string, std::string> head_parts;
      head_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      head_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      head_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      head_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      head_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      head_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["Head"] = head_parts;
      xml.readNext();
    }

    if( xml.name() == "Body" )
    {
      std::map<std::string, std::string> body_parts;
      body_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      body_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      body_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      body_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      body_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      body_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["Body"] = body_parts;
      xml.readNext();
    }

    if( xml.name() == "ArmL" )
    {
      std::map<std::string, std::string> arm_parts;
      arm_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      arm_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      arm_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      arm_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      arm_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      arm_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["ArmL"] = arm_parts;
      xml.readNext();
    }

    if( xml.name() == "ArmR" )
    {
      std::map<std::string, std::string> arm_parts;
      arm_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      arm_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      arm_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      arm_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      arm_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      arm_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["ArmR"] = arm_parts;
      xml.readNext();
    }

    if( xml.name() == "LegL" )
    {
      std::map<std::string, std::string> leg_parts;
      leg_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      leg_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      leg_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      leg_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      leg_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      leg_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["LegL"] = leg_parts;
      xml.readNext();
    }

    if( xml.name() == "LegR" )
    {
      std::map<std::string, std::string> leg_parts;
      leg_parts["Back"]   = xml.attributes().value("Back").toString().toStdString();
      leg_parts["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
      leg_parts["Front"]  = xml.attributes().value("Front").toString().toStdString();
      leg_parts["Left"]   = xml.attributes().value("Left").toString().toStdString();
      leg_parts["Right"]  = xml.attributes().value("Right").toString().toStdString();
      leg_parts["Top"]    = xml.attributes().value("Top").toString().toStdString();

      ar_textures["LegR"] = leg_parts;
      xml.readNext();
    }

    xml.readNext(); // next loop
  }
}

void
LogicLoader::parseCubeTextures( QXmlStreamReader& xml, std::map<std::string, std::string>& ar_textures )
{
  while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "textures"))
  {
    QString debugName = xml.name().toString();
    if( xml.tokenType() == QXmlStreamReader::EndElement || xml.name() == "" )
    {
      xml.readNext(); // next loop
      continue;
    }

    ar_textures["Back"]   = xml.attributes().value("Back").toString().toStdString();
    ar_textures["Bottom"] = xml.attributes().value("Bottom").toString().toStdString();
    ar_textures["Front"]  = xml.attributes().value("Front").toString().toStdString();
    ar_textures["Left"]   = xml.attributes().value("Left").toString().toStdString();
    ar_textures["Right"]  = xml.attributes().value("Right").toString().toStdString();
    ar_textures["Top"]    = xml.attributes().value("Top").toString().toStdString();

    xml.readNext(); // next loop
  }
}
