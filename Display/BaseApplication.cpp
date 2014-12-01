#include "BaseApplication.h"
#include "OgreManager.h"
#include "../Camera/CameraController.h"
#include "../Keyboard/KeyboardController.h"
#include "../Physics/PhysicsManager.h"
#include "BillboardManager.h"
#include "../Logic/Game.h"
#include "../Gui/MenuPause.h"
#include "../Gui/MenuMain.h"
#include "../Gui/GameGui.h"
#include "../Player/Player.h"
#include <qDebug>
#include "../Logic/LogicLoader.h"

//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mCameraMan(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0),
    _isGameOn(false)
{

}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
    if (mTrayMgr) delete mTrayMgr;
    if (mCameraMan) delete mCameraMan;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    Ogre::RenderSystem *rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");

    mRoot->setRenderSystem(rs);
    rs->setConfigOption("Video Mode", "800 x 600");
    rs->setConfigOption("Colour Depth", "16");
    rs->setConfigOption("FSAA", "0");
    rs->setConfigOption("Full Screen", "No");
    rs->setConfigOption("RTT Preferred Mode", "FBO");

    mWindow = mRoot->initialise(true, "WoxFare");

    return true;
}

//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("MainCam");
    mCamera->setPosition(0,0,0);
    Ogre::SceneNode * cameraNode = mSceneMgr->createSceneNode("CAMERA");
    cameraNode->attachObject(mCamera);
    CameraController::Instance()->setCamera(cameraNode, mCamera);
    // Look back along -Z);
    mCamera->setNearClipDistance(1);

}

//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    //Menu
    mTrayMgr = new OgreBites::SdkTrayManager("Interface", mWindow, mMouse, this);
    mTrayMgr->showCursor();

    // create a params panel for displaying sample details
    mRoot->addFrameListener(this);

    //INITIALIZING AUDIO AND TESTING
    qDebug() << "INITIALIZE SOUND MANAGER";

    SoundManager * soundMgr = new SoundManager;
    soundMgr->Initialize();

    qDebug() << "INITIALIZE SOUND MANAGER... FIN";
    OgreManager::Instance()->setSoundManager(soundMgr);

}

//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{

}

//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}

//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
     Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "./debug/resources_d.cfg";
    mPluginsCfg = "./debug/plugins_d.cfg";
#else
    mResourcesCfg = "./release/resources.cfg";
    mPluginsCfg = "./release/plugins.cfg";
#endif

    if (!setup())
        return;


    OgreManager * ogreManager = OgreManager::Instance();
    ogreManager->setRoot(mRoot);
    ogreManager->setSceneManager(mSceneMgr);
    ogreManager->setWindow(mWindow);
    ogreManager->setMainCamera(mCamera);
    ogreManager->setGuiManager(mTrayMgr);

    PhysicsManager::Instance()->setRootSceneNode( mSceneMgr->getRootSceneNode() );

    _mMenus = std::map<std::string, MenuAbs *>();
    _mMenus["Pause"] = new MenuPause();
    _mMenus["Main"] = new MenuMain();
    _mMenus["Game"] = new GameGui();

    createBaseCubeMesh();

    try{
        qDebug() << _introIdSound;
        int channelId = INVALID_SOUND_CHANNEL;
        OgreManager::Instance()->getSoundManager()->PlaySound( _introIdSound , CameraController::Instance()->getCameraNode(), &channelId);
        Ogre::LogManager::getSingleton().logMessage("BaseApp::Playing Intro OK");
    } catch( ... )
    {
        Ogre::LogManager::getSingleton().logMessage("BaseApp::NO Playing Intro");
    }

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg, "Ogre.cfg", "Ogre.log");

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    _buttonIdSound = OgreManager::Instance()->getSoundManager()->CreateSound( Ogre::String("button.mp3") );
    _introIdSound  = OgreManager::Instance()->getSoundManager()->CreateLoopedSound( Ogre::String("BSO_MENUMAIN.wav") );
    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    mTrayMgr->frameRenderingQueued(evt);

    mTrayMgr->adjustTrays();

    if(_isGameOn)
    {
        mTrayMgr->showFrameStats(OgreBites::TL_LEFT);

        if(Game::Instance()->isBackground() && !_mMenus["Pause"]->isVisible())
        {
            qDebug() << "Pause";
            _mMenus["Game"]->exit();
            _mMenus["Pause"]->enter();
            mTrayMgr->showCursor();
        }
        else if(!Game::Instance()->isBackground() && _mMenus["Pause"]->isVisible())
        {
            qDebug() << "Game";
            _mMenus["Pause"]->exit();
            _mMenus["Game"]->enter();
            mTrayMgr->hideCursor();
        }
        CameraController::Instance()->update();
        Game::Instance()->update(evt.timeSinceLastFrame);
        if(Game::Instance()->getExit())
        {
            //Salir del juego
            Game::Instance()->close();
            _mMenus["Game"]->exit();
            _mMenus["Main"]->enter();
            OgreManager::Instance()->getSoundManager()->StopAllSounds();
            int channelId = INVALID_SOUND_CHANNEL;
            OgreManager::Instance()->getSoundManager()->PlaySound( _introIdSound , CameraController::Instance()->getCameraNode(), &channelId);
            _isGameOn = false;
            mTrayMgr->showCursor();
        }

        if(_mMenus["Game"]->isVisible())
        {
            ((GameGui*)_mMenus["Game"])->update(evt.timeSinceLastEvent);
        }
    }
    else
    {
        if(!_mMenus["Main"]->isVisible())
        {
            _mMenus["Main"]->enter();
            mTrayMgr->showCursor();
        }
        mTrayMgr->hideFrameStats();
    }

    return true;
}

void
BaseApplication::buttonHit(OgreBites::Button *button)
{
    std::string menuName;
    std::string buttonName = button->getName();
    menuName = buttonName.substr(0, buttonName.find('-'));
    buttonName = buttonName.substr(buttonName.find('-')+1);

    int channelId = INVALID_SOUND_CHANNEL;
    OgreManager::Instance()->getSoundManager()->PlaySound( _buttonIdSound , CameraController::Instance()->getCameraNode(), &channelId);

    if(menuName == "Pause")
    {
        qDebug() << "Pause";
        if(buttonName == "Resume")
        {
            Game::Instance()->setBackground(!Game::Instance()->isBackground());
        }
        else if(buttonName == "Exit")
        {
            Game::Instance()->close();
            _mMenus["Pause"]->exit();
            _mMenus["Main"]->enter();
            OgreManager::Instance()->getSoundManager()->StopAllSounds();
            OgreManager::Instance()->getSoundManager()->PlaySound( _introIdSound , CameraController::Instance()->getCameraNode(), &channelId);
            _isGameOn = false;
        }
    }
    else if(menuName == "Main")
    {
        if ( buttonName == "Load")
        {
            LogicLoader * loader = LogicLoader::Instance();
            loader->load();
            if(loader->isGameLoaded())
            {
                //Enable Play
                Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
                Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
                OgreManager::Instance()->getSoundManager()->StopAllSounds();
                Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
                Game::Instance()->initialize();
                BillboardManager::Instance()->initialize();
                qDebug() << "Initializing OK!";
                _mMenus["Main"]->exit();
                _isGameOn = true;
                mTrayMgr->hideCursor();
                _mMenus["Game"]->enter();
            }
        }
        else if(buttonName == "Exit")
        {
            mShutDown = true;
        }
    }
}

//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if(arg.key == OIS::KC_ESCAPE)
    {
         Game::Instance()->setBackground(!Game::Instance()->isBackground());
    }
    else if(arg.key == OIS::KC_J)
    {
        PhysicsManager::Instance()->getDynamicsWorld()->debugDrawWorld();
    }
    else
    {
        KeyboardController::Instance()->pressKey(arg.key);
    }
    return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
{

    KeyboardController::Instance()->releaseKey(arg.key);
    return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
{
    mTrayMgr->injectMouseMove(arg);
    return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    mTrayMgr->injectMouseDown(arg, id);
    return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    mTrayMgr->injectMouseUp(arg, id);
    return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void
BaseApplication::createBaseCubeMesh()
{
    Ogre::SceneManager* pSceneManager = OgreManager::Instance()->getSceneManager();
    Ogre::ManualObject* manual;
    try
    {
        manual = pSceneManager->createManualObject("baseCube");
    } catch(Ogre::Exception e)
    {
    pSceneManager->destroyManualObject("baseCube");
    manual = pSceneManager->createManualObject("baseCube");
    }

    //Setting up material of every face.
    Ogre::MaterialManager::getSingleton().create("Top", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialManager::getSingleton().create("Bottom", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialManager::getSingleton().create("Left", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialManager::getSingleton().create("Right", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialManager::getSingleton().create("Front", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::MaterialManager::getSingleton().create("Back", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    manual->setDynamic(true);

    manual->begin("Bottom", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(0,1,0);
    manual->position(0.5, -0.5, 0.5);
    manual->textureCoord(1, 0);
    manual->position( 0.5, -0.5, -0.5);
    manual->textureCoord(1, 1);
    manual->position( -0.5, -0.5, -0.5);
    manual->textureCoord(0, 1);
    manual->position(-0.5, -0.5, 0.5);
    manual->textureCoord(0, 0);
    manual->quad(3,2,1,0);
    manual->end();

    // Top
    manual->begin("Top", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(0,1,0);
    manual->position(0.5, 0.5, 0.5);
    manual->textureCoord(1, 1);
    manual->position( 0.5, 0.5, -0.5);
    manual->textureCoord(1, 0);
    manual->position( -0.5, 0.5, -0.5);
    manual->textureCoord(0, 0);
    manual->position(-0.5, 0.5, 0.5);
    manual->textureCoord(0, 1);
    manual->quad(0,1,2,3);
    manual->end();

    // Left
    manual->begin("Left", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(1,0,0);
    manual->position(-0.5, -0.5, 0.5);
    manual->textureCoord(1, 1);
    manual->position( -0.5, 0.5, 0.5);
    manual->textureCoord(1, 0);
    manual->position( -0.5, 0.5, -0.5);
    manual->textureCoord(0, 0);
    manual->position(-0.5, -0.5, -0.5);
    manual->textureCoord(0, 1);
    manual->quad(0,1,2,3);
    manual->end();

    // Right
    manual->begin("Right", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(1,0,0);
    manual->position(0.5, -0.5, 0.5);
    manual->textureCoord(0, 1);
    manual->position( 0.5, 0.5, 0.5);
    manual->textureCoord(0, 0);
    manual->position( 0.5, 0.5, -0.5);
    manual->textureCoord(1, 0);
    manual->position(0.5, -0.5, -0.5);
    manual->textureCoord(1, 1);
    manual->quad(3,2,1,0);
    manual->end();

    // Front
    manual->begin("Front", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(0,0,1);
    manual->position(-0.5, -0.5, 0.5);
    manual->textureCoord(0, 1);
    manual->position( -0.5, 0.5, 0.5);
    manual->textureCoord(0, 0);
    manual->position( 0.5, 0.5, 0.5);
    manual->textureCoord(1, 0);
    manual->position(0.5, -0.5, 0.5);
    manual->textureCoord(1, 1);
    manual->quad(3,2,1,0);
    manual->end();

    // Back
    manual->begin("Back", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    manual->normal(0,0,1);
    manual->position(-0.5, -0.5, -0.5);
    manual->textureCoord(1, 1);
    manual->position( -0.5, 0.5, -0.5);
    manual->textureCoord(1, 0);
    manual->position( 0.5, 0.5, -0.5);
    manual->textureCoord(0, 0);
    manual->position(0.5, -0.5, -0.5);
    manual->textureCoord(0, 1);
    manual->quad(0,1,2,3);
    manual->end();

    manual->convertToMesh("baseCubeMesh.mesh");
    pSceneManager->destroyManualObject("baseCube");
}
