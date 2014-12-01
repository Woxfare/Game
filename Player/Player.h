/*

     __    __
    / / /\ \ \ ___ _   _   ____ __ _ _ __ ___
    \ \/  \/ /| _ |\ \/ / |  __/ _` | '__/ _ \
     \  /\  / ||_|| >  <  | |_  (_| | | |  __/
      \/  \/  |___|/_/\_\|_  _|\__,_|_|  \___|
                          | |
                          |_|
Copyright (c) 2014 Fernando Perera & Alejandro Vázquez

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef PLAYER_H
#define PLAYER_H

#include "Ogre.h"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "../Weapons/WeaponAbs.h"
#define MAX_VEL_PLAYER 10

class Player
{
public:
    typedef std::map<std::string, std::map<std::string, std::string>> textureMap;
    /*!
     * \brief Instance
     */
    static Player * Instance( );
    
    /*!
     * \brief DefaultDestructor
     */
    ~Player();

    /*!
     * \brief getPosition
     * \return
     */
     Ogre::Vector3 getPosition( void );

     /*!
      * \brief setPositon
      * \return
      */
      void setPosition( Ogre::Vector3 );

      /*!
       * \brief setPositon
       * \return
       */
      void setPosition( float x, float y, float z );

      /*!
       * \brief getSceneNode
       * \return
       */
      Ogre::SceneNode *  getSceneNode();

      void setWeaponsList(std::vector<std::string> weapons);
      void setRGB(int r, int g, int b);

      textureMap createMaterials(textureMap textures);

      void setMaterials(textureMap textures);

      /*!
       * \brief initialize
       */
      void initialize();

      void initializeMaterials();
      void destroyMaterials();
      void changeWeaponUp();
      void changeWeaponDown();
      void addAmmoToWeapon( std::string weaponName, int value);

      /*!
       * \brief unload
       */
      void unload();

      /*!
       * \brief update
       * \param elapsedTime
       */
      void update(double elapsedTime);

      /*!
       * \brief setDirectionToMove
       * \param x
       * \param y
       * \param z
       */
      void setDirectionToMove(float x, float y, float z);

      /*!
       * \brief setOrientation
       * \param x
       * \param y
       * \param z
       */
      void setOrientation(float x, float y, float z);

      /*!
       * \brief addRotation
       * \param increment
       */
      void addRotation(float increment);

      /*!
       * \brief jump
       */
      void jump();

      /*!
       * \brief shot
       */
      void shot();

      /*!
       * \brief addLifePoints
       * \param lifePoints
       */
      void addLifePoints(int lifePoints);

      /*!
       * \brief getLifePoints
       * \return
       */
      int getLifePoints();

      /*!
       * \brief setMaxLifePoints
       * \param points
       */
      void setMaxLifePoints( int points );

      /*!
       * \brief getCurrentWeapon
       * \return
       */
      WeaponAbs * getCurrentWeapon();

      /*!
       * \brief setDefaultWeapon
       */
      void        setDefaultWeapon(WeaponAbs*);

      /*!
       * \brief getName
       * \return
       */
      Ogre::String getName();

      /*!
       * \brief setName
       */
      void setName( Ogre::String );

      /*!
       * \brief addGold
       * \param gold
       */
      void addGold( int gold );

      /*!
       * \brief getGold
       * \return
       */
      int getGold( void );

      int getRacha( void );

private:
      /*!
       * \brief Player
       */
      Player( );

private:
      float _frgb[3];
      static Player * _pInstance;
      Ogre::Vector3 _vPosition;
      Ogre::Vector3 _vDirectionToMove;
      Ogre::Vector3 _vOrientation;
      Ogre::SceneNode * _pPlayerNode;
      Ogre::Entity    * _pPlayerEntity;
      Ogre::AnimationState * _pAnimationStateTop;
      Ogre::AnimationState * _pAnimationStateBottom;
      btRigidBody * _pRigidBody;
      float jumping;
      double _increment;
      textureMap _mMaterials;
      std::vector<std::string> _vWeaponsNameList;
      int _soundIndex;
      int _soundChannel;

      // Life
      int         _lifePoints;
      int         _maxLifePoints;

      // Gold
      int         _totalGold;
      int         _racha;
      int         _shotCount;
      int         _currentShotMax;
      float       _timeRacha;

      // Weapons
      std::vector<WeaponAbs*> _vWeapon;
      int _indexCurrentWeapon;
      WeaponAbs *             _currentWeapon;
      WeaponAbs *             _defaultWeapon;

      Ogre::String           _name;
};

#endif // Player_H
