#include "TutorialApplication.h"
#include "OgreMath.h"
#include "OgreManager.h"
#include "qDebug"

#define OGRE_PLATFORM  OGRE_PLATFORM_WIN32
//#define OGRE_PLATFORM  OGRE_PLATFORM_MACOS
//#define OGRE_PLATFORM  OGRE_PLATFORM_LINUX

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
    index = 0;
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{

}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, LPCSTR(e.getFullDescription().c_str()), LPCSTR("An exception has occured!"), MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
