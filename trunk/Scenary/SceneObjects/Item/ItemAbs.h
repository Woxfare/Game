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
#ifndef ITEMABS_H
#define ITEMABS_H

#include "OGRE/Ogre.h"
#include "../../../entity.h"

class ItemAbs : public entity
{
public:

    /*!
     * \brief ItemAbs
     */
    ItemAbs(std::string name, int value, Ogre::Vector3 position, std::map<std::string, std::string> materials, btRigidBodyConstructionInfo * rbInfo);

    virtual ~ItemAbs();
    /*!
     * \brief Performs the action depending on the type of the item
     */
    virtual void collected( void ) = 0;

    /*!
     * \brief Gets the message to be shown in the screen of with the info
     * \return
     */
    virtual Ogre::String message( void ) = 0;

    /*!
     * \brief Gets the value of the item
     * \return int
     */
    int getValue();

    /*!
     * \brief setValue
     * \param value
     */
    void setValue( int value );

    /*!
     * \brief getName
     */
    Ogre::String getName();

    /*!
     * \brief setName
     * \param name
     */
    void setName(Ogre::String name );

    void setSceneNode( Ogre::SceneNode * itemNode );

    void setMaterials( std::map<std::string, std::string> materials);

    bool isCollected();

protected:

    /*!
     * \brief The item name
     */
    Ogre::String _name;

    /*!
     * \brief Stores the value of the item
     */
    int _value;

    Ogre::Entity * _pItemEntity;
};

#endif // ITEMABS_H
