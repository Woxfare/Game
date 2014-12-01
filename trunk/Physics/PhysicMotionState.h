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
#ifndef PhysicMotionState_H
#define PhysicMotionState_H

// Includes
#include "Ogre.h"
#include <btBulletDynamicsCommon.h>

class PhysicMotionState : public btMotionState
{
public:
    /*!
     * \brief The motion state constructor
     * \param initialpos, The intial position
     * \param node, The node of the scene
     */
    PhysicMotionState( const btTransform &initialpos, Ogre::SceneNode *node, float yDesviation = 0 );

    /*!
     * \brief The motion state destructor
     */
    ~PhysicMotionState();

    /*!
     * \brief Sets the node to the motion state
     * \param node
     */
    void setNode(Ogre::SceneNode *node);

    /*!
     * \brief Gets the world transformation to be seted to the scene node
     * \param worldTrans
     */
    virtual void getWorldTransform(btTransform &worldTrans) const;

    /*!
     * \brief setWorldTransform
     * \param worldTrans
     */
    virtual void setWorldTransform(const btTransform &worldTrans);

protected:
    Ogre::SceneNode *mVisibleobj;
    btTransform mPos1;
    float _yDesviation;
};

#endif // PhysicMotionState_H
