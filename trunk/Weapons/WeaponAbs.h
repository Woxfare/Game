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
#ifndef WEAPONABS_H
#define WEAPONABS_H

#include "OGRE/Ogre.h"

class WeaponAbs
{
public:
    enum weaponType : int {Pistol, Shotgun, MachineGun};

    WeaponAbs(std::string name, float maxDistance, float maxAmmo, float timeBetweenShots, int damange );
    virtual ~WeaponAbs();

    virtual bool PlayerShot(Ogre::Vector3 position, Ogre::Vector3 orientation)=0;
    virtual bool BotShot(Ogre::Vector3 orientation) = 0;
    virtual void updateShot(float elapsedTime)=0;
    virtual int getType()=0;
    std::string getName();
    void addAmmo(int ammo);
    int getAmmo();
    int getSoundIndex();
    void setVisible( bool visible);
    void setActivate( bool activate );
    bool getActivate();


protected:
    bool  _activate;
    bool  _visible;
    int   _maxAmmo;
    int   _damange;
    int   _currentAmmo;
    int   _soundIndex;
    int   _soundChannel;
    float _timeNextShot;
    float _timeBetweenShots;
    float _timeAnimationShot;
    float _maxDistance;
    std::string _name;

    Ogre::BillboardSet * set;
    Ogre::SceneNode * _pWeaponNode;

};

#endif // WeaponAbs_H
