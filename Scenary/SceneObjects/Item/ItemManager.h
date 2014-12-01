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
#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include "ItemAbs.h"
#include "ItemType.h"

class ItemManager
{
public:
    static ItemManager * Instance();
    ~ItemManager();
    void addItem( std::string itemName, Ogre::Vector3 position );
    void update(double elapsedTime);
    void createNewType(std::string &name, std::string &itemType, std::map<std::string, std::string> textures, int value, int rgb[3], std::map<std::string, std::string> extraAttr);
    void unload();
    void addRandomItem(Ogre::Vector3 position);

private:
    ItemManager();

private:
    static ItemManager * _pInstance;
    std::vector<ItemAbs*> _vItemsInScene;
    std::map<std::string, ItemType*> _mItemTypes;
};

#endif // ItemManager_H
