TEMPLATE = app
TARGET = Gameplay

DEFINES -= UNICODE

CONFIG += qt

QT += widgets

win32 {
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #                             OGRE ENGINE INCLUDES & LIBS
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LIBS *= user32.lib
    LIBS += -L$(OGRE_HOME)\\boost\\lib
    release:LIBS += -L$(OGRE_HOME)\\lib\\Release
    debug:LIBS += -L$(OGRE_HOME)\\lib\\Debug
    INCLUDEPATH += $(OGRE_HOME)\\include
    INCLUDEPATH += $(OGRE_HOME)\\include\\OIS
    INCLUDEPATH += $(OGRE_HOME)\\include\\OGRE
    INCLUDEPATH += $(OGRE_HOME)\\boost

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #                           BULLET PHYSICS INCLUDES & LIBS
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LIBS += -LD:\\bullet\\lib
    INCLUDEPATH += D:\\bullet\\src

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #                           AUDIO FMOD INCLUDES & LIBS
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    LIBS += -LC:\\FMOD\\api\\lib
    INCLUDEPATH += C:\\FMOD\\api\\inc
}

debug {
    TARGET = $$join(TARGET,,,d)
    LIBS *= -lOgreMain_d -lOIS_d
    LIBS *= -lBulletCollision_vs2010_debug -lBulletDynamics_vs2010_debug -lLinearMath_vs2010_debug
    LIBS *= -lfmodex_vc
}

release
{
    LIBS *= -lOgreMain -lOIS
    #LIBS *= -lBulletCollision_vs2010 -lBulletDynamics_vs2010 -lLinearMath_vs2010
}

SOURCES += \
    Display/BaseApplication.cpp \
    Player/Player.cpp \
    Display/TutorialApplication.cpp \
    Display/OgreManager.cpp \
    Camera/CameraController.cpp \
    Bots/BotManager.cpp \
    Bots/BotAbs.cpp \
    Bots/Behaviours/Follower.cpp \
    Bots/Behaviours/BehaviourAbs.cpp \
    Bots/BotTipes/BotSimple.cpp \
    Scenary/Scenery.cpp \
    Keyboard/KeyboardController.cpp \
    Logic/Game.cpp \
    Display/BillboardManager.cpp \
    Physics/PhysicMotionState.cpp \
    Physics/PhysicsManager.cpp \
    Gui/MenuPause.cpp \
    Gui/MenuAbs.cpp \
    Gui/MenuMain.cpp \
    Weapons/WeaponAbs.cpp \
    Physics/PhysicsDebugDrawer.cpp \
    Scenary/SceneObjects/Item/ItemManager.cpp \
    Scenary/SceneObjects/Item/ItemAbs.cpp \
    Scenary/SceneObjects/Item/ItemHealth.cpp \
    Scenary/SceneObjects/Item/ItemGold.cpp \
    Scenary/SceneObjects/Item/ItemAmmo.cpp \
    Bots/BotType.cpp \
    Physics/PhysicsDebugDrawer.cpp \
    Logic/LogicLoader.cpp \
    Physics/BulletMeshStrider.cpp \
    entity.cpp \
    Scenary/ScenerySection.cpp \
    Sound/SoundManager.cpp \
    Scenary/SceneObjects/Item/ItemType.cpp \
    Gui/GameGui.cpp \
    Scenary/SceneObjects/DynamicObjects/DynamicObjectType.cpp \
    Scenary/SceneObjects/DynamicObjects/DynamicObject.cpp \
    Scenary/SceneObjects/DynamicObjects/DynamicObjectManager.cpp \
    Bots/Behaviours/Shotter.cpp \
    Bots/BotTipes/BotBoss.cpp \
    Weapons/Shotgun.cpp \
    Weapons/Pistol.cpp \
    Weapons/MachineGun.cpp \
    Weapons/WeaponType.cpp \
    Weapons/WeaponManager.cpp

HEADERS += \
    Display/BaseApplication.h \
    Player/Player.h \
    Display/TutorialApplication.h \
    Display/OgreManager.h \
    Camera/CameraController.h \
    Bots/BotManager.h \
    Bots/BotAbs.h \
    Bots/Behaviours/Follower.h \
    Bots/Behaviours/BehaviourAbs.h \
    Bots/BotTipes/BotSimple.h \
    Scenary/Scenery.h \
    Keyboard/KeyboardController.h \
    Logic/Game.h \
    Display/BillboardManager.h \
    Physics/PhysicMotionState.h \
    Physics/PhysicsManager.h \
    Gui/MenuPause.h \
    Gui/MenuAbs.h \
    Gui/MenuMain.h \
    Weapons/WeaponAbs.h \
    Physics/PhysicsDebugDrawer.h \
    Scenary/SceneObjects/Item/ItemManager.h \
    Scenary/SceneObjects/Item/ItemAbs.h \
    Scenary/SceneObjects/Item/ItemHealth.h \
    Scenary/SceneObjects/Item/ItemGold.h \
    Scenary/SceneObjects/Item/ItemAmmo.h \
    Bots/BotType.h \
    Logic/LogicLoader.h \
    Physics/BulletMeshStrider.h \
    entity.h \
    Scenary/ScenerySection.h \
    Sound/SoundManager.h \
    Scenary/SceneObjects/Item/ItemType.h \
    Gui/GameGui.h \
    Scenary/SceneObjects/DynamicObjects/DynamicObjectType.h \
    Scenary/SceneObjects/DynamicObjects/DynamicObject.h \
    Scenary/SceneObjects/DynamicObjects/DynamicObjectManager.h \
    Bots/Behaviours/Shotter.h \
    Bots/BotTipes/BotBoss.h \
    Weapons/Shotgun.h \
    Weapons/Pistol.h \
    Weapons/MachineGun.h \
    Weapons/WeaponType.h \
    Weapons/WeaponManager.h


