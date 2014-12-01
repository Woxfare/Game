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
#ifndef BOTTYPE_H
#define BOTTYPE_H

#include "OGRE/Ogre.h"
#include "./BotAbs.h"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class BotType
{
public:
    BotType(std::string &name, std::string &botType, std::string &botBehaviour, textureMap textures, int life, int points, int rgb[3]);
    ~BotType();

    BotAbs * createNewBot(Ogre::Vector3 position);
    textureMap createMaterials(textureMap textures);
    void destroyMaterials();
    std::string getBotType();
    void addItemName(std::string itemName);
    void setItemNames(std::vector<std::string> itemsNames);

protected:
    int         _index;
    int         _life;
    int         _points;
    float       _frgb[3];
    std::string _botName;
    std::string _botType;
    std::string _botBehaviour;
    std::vector<std::string> _vItemsNames;
    textureMap _materials;

};

#endif // BotType_H
