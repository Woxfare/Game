#include "CameraController.h"

CameraController * CameraController::_pInstance = 0;

CameraController::CameraController()
{

}

CameraController::~CameraController()
{

}

CameraController * CameraController::Instance()
{
    if(!_pInstance)
        _pInstance = new CameraController();

    return _pInstance;
}

void
CameraController::update()
{
    _pNode->setPosition(_pTarget->getPosition().x,
                      _pTarget->getPosition().y + 20,
                      _pTarget->getPosition().z - 30);
    _pCamera->lookAt(_pTarget->getPosition());
}

void
CameraController::setCamera(Ogre::SceneNode * cameraNode, Ogre::Camera * camera)
{
    _pNode = cameraNode;
    _pCamera = camera;
}

void
CameraController::setTarget(Ogre::SceneNode * target)
{
    _pTarget = target;
}

Ogre::SceneNode *
CameraController::getCameraNode()
{
    return _pNode;
}
