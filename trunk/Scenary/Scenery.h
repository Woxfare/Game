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
#ifndef SCENERY_H
#define SCENERY_H
#include "Ogre.h"
#include "ScenerySection.h"

class Scenery
{
public:
    /*!
     * \brief Instance
     */
    static Scenery * Instance( );
    
    /*!
     * \brief DefaultDestructor
     */
    ~Scenery();

    void setColor(int r, int g, int b);
    void setColor(float r, float g, float b);
    void setBaseTextureName(std::string name);
    void setMapName(std::string name);
    void addEnemyDoor(float x, float y, float z);
    std::vector<Ogre::Vector3> getEnemyDoors();

    void initialize();
    void unload();
    Ogre::SceneNode *  getSceneryRootNode();

private:
      /*!
       * \brief Player
       */
      Scenery( );

      void createBasePlane(float width, float profundidad, float y=0.0f);
      void initializeSceneryMesh();

private:
      float _frgb[3];
      std::string _baseTextureName;
      std::string _mapName;
      static Scenery  * _pInstance;
      Ogre::SceneNode * _pSceneryRootNode;
      std::vector<Ogre::Vector3> _enemyDoors;
      std::vector<ScenerySection*> _scenerySections;
      btRigidBody * _basePlaneBody;
};

#endif // Scenery_H
