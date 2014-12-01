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

#ifndef BEHAVIOURABS_H
#define BEHAVIOURABS_H
#include "OGRE/Ogre.h"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

class BehaviourAbs
{
public:
    virtual void update(double elapsedTime, btRigidBody *rigidBody)=0;
    virtual void initialize(Ogre::SceneNode * owner)=0;
    virtual ~BehaviourAbs()=0;

    BehaviourAbs(int life, int points, std::vector<std::string> itemsNames);
    void setPosition(float x, float y, float z);
    Ogre::Vector3 getPosition();
    void setDirection(float x, float y, float z);
    Ogre::Vector3 getDirection();
    //void setAnimation(std::string name, Ogre::Entity * botEntity, bool loop = 0);
    void stopAllAnimations();
    bool getRemoveFlag();

    void addLife(int life);
    void setLife(int life);
    int  getLife();
    void createRandomItem();



protected:
    bool  _removeFlag;
    int   _points;
    int   _life;
    int   _maxLife;
    float _velocity;
    float _itemProb;
    std::vector<std::string> _vItemsNames;
    Ogre::SceneNode * _sceneNode;
    Ogre::Vector3 _vPosition;
    Ogre::Vector3 _vDirection;
    Ogre::AnimationState * _pAnimationStateTop;
    Ogre::AnimationState * _pAnimationStateBottom;
    Ogre::Ray * _pRayY;

};

#endif // BehaviourAbs_H
