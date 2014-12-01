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
#ifndef WEAPONTYPE_H
#define WEAPONTYPE_H

#include "./WeaponAbs.h"

class WeaponType
{
public:
    WeaponType(std::string &name, WeaponAbs::weaponType weaponType, int maxAmmo, int damange, int rgb[3]);
    ~WeaponType();

    WeaponAbs * createNewWeapon(Ogre::SceneNode * owner);
    std::map<std::string, std::string> createMaterials(std::map<std::string, std::string> textures);
    void destroyMaterials();

private:
    int         _index;
    int         _maxAmmo;
    int         _damange;
    float       _frgb[3];
    WeaponAbs::weaponType _weaponType;
    std::string _name;
    std::map<std::string, std::string> _materials;
};

#endif // ITEMTYPE_H
