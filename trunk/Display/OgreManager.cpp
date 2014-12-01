#include "OgreManager.h"

OgreManager * OgreManager::_pInstance = 0;

OgreManager::OgreManager()
{
}

OgreManager::~OgreManager()
{
}

OgreManager * OgreManager::Instance()
{
    if(!_pInstance)
        _pInstance = new OgreManager();

    return _pInstance;
}

void
OgreManager::setSceneManager(Ogre::SceneManager * sceneManager)
{
    _pSceneManager = sceneManager;
}

void
OgreManager::setRoot(Ogre::Root * root)
{
    _pRoot = root;
}

void
OgreManager::setMainCamera(Ogre::Camera * camera)
{
    _pCamera = camera;
}

void
OgreManager::setWindow(Ogre::RenderWindow * window)
{
    _pWindow = window;
}

void
OgreManager::setGuiManager(OgreBites::SdkTrayManager * guiManager)
{
    _pGuiManager = guiManager;
}

void
OgreManager::setSoundManager(SoundManager * soundManager)
{
    _soundManager = soundManager;
}

SoundManager *
OgreManager::getSoundManager()
{
    return _soundManager;
}

Ogre::SceneManager *
OgreManager::getSceneManager()
{
    return _pSceneManager;
}

Ogre::Root *
OgreManager::getRoot()
{
    return _pRoot;
}

Ogre::Camera *
OgreManager::getMainCamera()
{
    return _pCamera;
}

Ogre::RenderWindow *
OgreManager::getWindow()
{
    return _pWindow;
}

OgreBites::SdkTrayManager *
OgreManager::getGuiManager()
{
    return _pGuiManager;
}

Ogre::ManualObject*
OgreManager::createCubeMesh(Ogre::String name, Ogre::String matName) {

   Ogre::ManualObject* cube = new Ogre::ManualObject(name);

   cube->begin(matName);

   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,1);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,1);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,0);
   cube->position(0.5,0.5,1.0);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,1);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(1,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(1,0);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(0,1);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(1,1);
   cube->position(0.5,0.5,1.0);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(0,0);

   cube->triangle(0,1,2);      cube->triangle(3,1,0);
   cube->triangle(4,5,6);      cube->triangle(4,7,5);
   cube->triangle(8,9,10);      cube->triangle(10,7,8);
   cube->triangle(4,11,12);   cube->triangle(4,13,11);
   cube->triangle(14,8,12);   cube->triangle(14,15,8);
   cube->triangle(16,17,18);   cube->triangle(16,19,17);
   cube->end();

   return cube;
}
