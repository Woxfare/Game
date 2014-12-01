#include "SoundManager.h"
#include <fmod_errors.h>


#define INITIAL_VECTOR_SIZE   100
#define INCREASE_VECTOR_SIZE  20

#define DOPPLER_SCALE         1.0
#define DISTANCE_FACTOR       1.0
#define ROLLOFF_SCALE         0.5


template<> SoundManager* Ogre::Singleton<SoundManager>::msSingleton = 0;


void SoundInstance::Clear(void)
   {
   fileName.clear();
   streamPtr.setNull();
   fileArchive = NULL;
   fmodSound = NULL;
   soundType = SOUND_TYPE_INVALID;
   }


void ChannelInstance::Clear(void)
   {
   sceneNode = NULL;
   prevPosition = Ogre::Vector3(0, 0, 0);
   }



class FileLocator : public Ogre::ResourceGroupManager
   {
public:
   FileLocator() {}
   ~FileLocator() {}
   Ogre::Archive *Find(Ogre::String &filename)
      {
      ResourceGroup* grp = getResourceGroup("Popular");
      if (!grp)
         OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Cannot locate a resource group called 'General'", "ResourceGroupManager::openResource");

      Ogre::LogManager::getSingleton().logMessage( Ogre::String("SoundManager::GroupName ")+ grp->name );

      OGRE_LOCK_MUTEX(grp->OGRE_AUTO_MUTEX_NAME) // lock group mutex
      ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(filename);
      if (rit != grp->resourceIndexCaseSensitive.end())
         {
         // Found in the index
         Ogre::Archive *fileArchive = rit->second;
         filename = fileArchive->getName() + "/" + filename;

         Ogre::LogManager::getSingleton().logMessage( Ogre::String("SoundManager::Filename ")+ filename );
         return fileArchive;
         }
      return NULL;
      }
   };

SoundManager::SoundManager()
{
    system = NULL;
    prevListenerPosition = Ogre::Vector3(0, 0, 0);
    soundInstanceVector = new SoundInstanceVector;

    // Initialized to zero, but pre-incremented in GetNextSoundInstanceIndex(), so vector starts at one.
    nextSoundInstanceIndex = 0;

    // Start off with INITIAL_VECTOR_SIZE soundInstanceVectors.  It can grow from here.
    soundInstanceVector->resize(INITIAL_VECTOR_SIZE);
    for (int vectorIndex = 0; vectorIndex < INITIAL_VECTOR_SIZE; vectorIndex++)
      {
      soundInstanceVector->at(vectorIndex) = new SoundInstance;
      soundInstanceVector->at(vectorIndex)->Clear();
      }

    for (int channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
      channelArray[channelIndex].Clear();
}

SoundManager::~SoundManager()
{
    for (int vectorIndex = 0; vectorIndex < (int)soundInstanceVector->capacity(); vectorIndex++)
    {
        Ogre::LogManager::getSingleton().logMessage("SoundManager::Deleting Sound'" + soundInstanceVector->at(vectorIndex)->fileName + "'");
        soundInstanceVector->at(vectorIndex)->fileName.clear();
        delete soundInstanceVector->at(vectorIndex);
        Ogre::LogManager::getSingleton().logMessage("SoundManager::Deleted OK'" + soundInstanceVector->at(vectorIndex)->fileName + "'");
    }

    Ogre::LogManager::getSingleton().logMessage("SoundManager::Releasing Sound System");
    if (system)
        system->release();
    Ogre::LogManager::getSingleton().logMessage("SoundManager::Releasing Sound System OK");
}


void SoundManager::Initialize(void)
{
    FMOD_RESULT result;
    FMOD_SPEAKERMODE speakermode;

    // Create the main system object.
    result = FMOD::System_Create(&system);
    if (result != FMOD_OK)
      OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    result = system->getDriverCaps(0, 0, 0, 0, &speakermode);
    if (result != FMOD_OK)
        OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    result = system->setSpeakerMode(speakermode);
    if (result != FMOD_OK)
         OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    result = system->init(MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
    if (result != FMOD_OK)
      OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    system->set3DSettings(DOPPLER_SCALE, DISTANCE_FACTOR, ROLLOFF_SCALE);
    if (result != FMOD_OK)
      OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");

    /*result = system->setFileSystem(&fmodFileOpenCallback, &fmodFileCloseCallback, &fmodFileReadCallback, &fmodFileSeekCallback, 2048);
    if (result != FMOD_OK)
      OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FMOD error! (" + Ogre::StringConverter::toString(result) + "): " + FMOD_ErrorString(result), "SoundManager::Initialize");*/

    Ogre::LogManager::getSingleton().logMessage("SoundManager Initialized");
}


SoundManager* SoundManager::getSingletonPtr(void) { return msSingleton; }
SoundManager& SoundManager::getSingleton(void) { assert( msSingleton );  return ( *msSingleton ); }


void SoundManager::FrameStarted(Ogre::SceneNode *listenerNode, Ogre::Real timeElapsed)
{
    int            channelIndex;
    FMOD::Channel *nextChannel;
    FMOD_VECTOR    listenerPosition;
    FMOD_VECTOR    listenerForward;
    FMOD_VECTOR    listenerUp;
    FMOD_VECTOR    listenerVelocity;
    Ogre::Vector3  vectorVelocity;
    Ogre::Vector3  vectorForward;
    Ogre::Vector3  vectorUp;

    if (timeElapsed > 0)
      vectorVelocity = (listenerNode->getPosition() - prevListenerPosition) / timeElapsed;
    else
      vectorVelocity = Ogre::Vector3(0, 0, 0);

    vectorForward = listenerNode->getOrientation().zAxis();
    vectorForward.normalise();

    vectorUp = listenerNode->getOrientation().yAxis();
    vectorUp.normalise();

    listenerPosition.x = listenerNode->getPosition().x;
    listenerPosition.y = listenerNode->getPosition().y;
    listenerPosition.z = listenerNode->getPosition().z;

    listenerForward.x = vectorForward.x;
    listenerForward.y = vectorForward.y;
    listenerForward.z = vectorForward.z;

    listenerUp.x = vectorUp.x;
    listenerUp.y = vectorUp.y;
    listenerUp.z = vectorUp.z;

    listenerVelocity.x = vectorVelocity.x;
    listenerVelocity.y = vectorVelocity.y;
    listenerVelocity.z = vectorVelocity.z;

    // update 'ears'
    system->set3DListenerAttributes(0, &listenerPosition, &listenerVelocity, &listenerForward, &listenerUp);
    system->update();

    prevListenerPosition = listenerNode->getPosition();

    for (channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
      {
      if (channelArray[channelIndex].sceneNode != NULL)
         {
         system->getChannel(channelIndex, &nextChannel);
         if (timeElapsed > 0)
            vectorVelocity = (channelArray[channelIndex].sceneNode->getPosition() - channelArray[channelIndex].prevPosition) / timeElapsed;
         else
            vectorVelocity = Ogre::Vector3(0, 0, 0);

         listenerPosition.x = channelArray[channelIndex].sceneNode->getPosition().x;
         listenerPosition.y = channelArray[channelIndex].sceneNode->getPosition().y;
         listenerPosition.z = channelArray[channelIndex].sceneNode->getPosition().z;

         listenerVelocity.x = vectorVelocity.x;
         listenerVelocity.y = vectorVelocity.y;
         listenerVelocity.z = vectorVelocity.z;

         nextChannel->set3DAttributes(&listenerPosition, &listenerVelocity);
         channelArray[channelIndex].prevPosition = channelArray[channelIndex].sceneNode->getPosition();
         }
      }
}

int SoundManager::CreateStream(Ogre::String &fileName) { return CreateSound(fileName, SOUND_TYPE_2D_SOUND); }
int SoundManager::CreateSound(Ogre::String &fileName) { return CreateSound(fileName, SOUND_TYPE_3D_SOUND); }
int SoundManager::CreateLoopedSound(Ogre::String &fileName){ return CreateSound(fileName, SOUND_TYPE_3D_SOUND_LOOPED); }
int SoundManager::CreateLoopedStream(Ogre::String &fileName) {return CreateSound(fileName, SOUND_TYPE_2D_SOUND_LOOPED); }
FMOD_RESULT SoundManager::fmodFileCloseCallback(void *handle, void *userdata) { return FMOD_OK; }

// fileName is actually a pointer to a SoundInstance, passed in from CreateSound().
FMOD_RESULT SoundManager::fmodFileOpenCallback(const char *fileName, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
    SoundInstance *soundInstance;

    assert(fileName);

    soundInstance = (SoundInstance *)fileName;
    assert(soundInstance->fileArchive);

    *handle = (void *)soundInstance;
    *userdata = NULL;

    soundInstance->streamPtr = soundInstance->fileArchive->open(soundInstance->fileName);
    if (soundInstance->streamPtr.isNull())
      {
      *filesize = 0;
      return FMOD_ERR_FILE_NOTFOUND;
      }

    *filesize = (unsigned int)soundInstance->streamPtr->size();
    return FMOD_OK;
}

FMOD_RESULT SoundManager::fmodFileReadCallback(void *handle, void *buffer, unsigned int sizeBytes, unsigned int *bytesRead, void *userData)
{
    SoundInstance *soundInstance;

    soundInstance = (SoundInstance *)handle;
    *bytesRead = (unsigned int)soundInstance->streamPtr->read(buffer, (size_t)sizeBytes);
    if (*bytesRead == 0)
      return FMOD_ERR_FILE_EOF;

    return FMOD_OK;
}


FMOD_RESULT SoundManager::fmodFileSeekCallback(void *handle, unsigned int pos, void *userdata)
{
    SoundInstance *soundInstance;

    soundInstance = (SoundInstance *)handle;
    soundInstance->streamPtr->seek((size_t)pos);
    return FMOD_OK;
}



int SoundManager::CreateSound(Ogre::String &fileName, SOUND_TYPE soundType)
{
    Ogre::Archive * fileArchive;
    FMOD_RESULT     result;
    FMOD::Sound   * sound;
    Ogre::String    fullPathName;
    SoundInstance * newSoundInstance;

    int soundIndex;
    soundIndex = FindSound(fileName, soundType);
    if (soundIndex != INVALID_SOUND_INDEX)
      return soundIndex;

    fullPathName = fileName;
    FileLocator * fileLocator = (FileLocator * )Ogre::ResourceGroupManager::getSingletonPtr();
    fileArchive = fileLocator->Find(fullPathName);
    if (!fileArchive)
    {
        Ogre::LogManager::getSingleton().logMessage("SoundManager::CreateSound could not find sound '" + fileName + "'");
        return INVALID_SOUND_INDEX;
    }

    IncrementNextSoundInstanceIndex();
    newSoundInstance = soundInstanceVector->at(nextSoundInstanceIndex);
    newSoundInstance->fileName = fileName;
    newSoundInstance->fileArchive = fileArchive;
    newSoundInstance->soundType = soundType;

    switch (soundType)
      {
      case SOUND_TYPE_3D_SOUND:
         {
         result = system->createSound(fullPathName.c_str(), FMOD_3D, 0, &sound);
         break;
         }

      case SOUND_TYPE_3D_SOUND_LOOPED:
         {
         result = system->createSound(fullPathName.c_str(), FMOD_LOOP_NORMAL | FMOD_3D | FMOD_HARDWARE, 0, &sound);
         break;
         }

      case SOUND_TYPE_2D_SOUND:
         {
         result = system->createStream(fullPathName.c_str(), FMOD_DEFAULT, 0, &sound);
         break;
         }

      case SOUND_TYPE_2D_SOUND_LOOPED:
         {
         result = system->createStream(fullPathName.c_str(), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_HARDWARE, 0, &sound);
         break;
         }

      default:
         {
         Ogre::LogManager::getSingleton().logMessage("SoundManager::CreateSound could not load sound '" + fileName + "' (invalid soundType)");
         return INVALID_SOUND_INDEX;
         }

      }

    if (result != FMOD_OK)
      {
      Ogre::LogManager::getSingleton().logMessage("SoundManager::CreateSound could not load sound '" + fileName + "'  FMOD Error:" + FMOD_ErrorString(result));
      return INVALID_SOUND_INDEX;
      }

    newSoundInstance->fmodSound = sound;
    return nextSoundInstanceIndex;
}

void SoundManager::IncrementNextSoundInstanceIndex(void)
{
    int oldVectorCapacity;

    oldVectorCapacity = (int)soundInstanceVector->capacity();
    nextSoundInstanceIndex += 1;
    if (nextSoundInstanceIndex < oldVectorCapacity)
      return;

    int vectorIndex;
    SoundInstanceVector *newSoundInstanceVector;

    // Create a new, larger SoundInstanceVector
    newSoundInstanceVector = new SoundInstanceVector;
    newSoundInstanceVector->resize(oldVectorCapacity + INCREASE_VECTOR_SIZE);

    // Check Ogre.log for these messages, and change INITIAL_VECTOR_SIZE to be a more appropriate value
    Ogre::LogManager::getSingleton().logMessage("SoundManager::IncrementNextSoundInstanceIndex increasing size of soundInstanceVector to " +
                                               Ogre::StringConverter::toString(oldVectorCapacity + INCREASE_VECTOR_SIZE));

    // Copy values from old vector to new
    for (vectorIndex = 0; vectorIndex < oldVectorCapacity; vectorIndex++)
      newSoundInstanceVector->at(vectorIndex) = soundInstanceVector->at(vectorIndex);

    int newVectorCapacity;

    newVectorCapacity = (int)newSoundInstanceVector->capacity();
    // Clear out the rest of the new vector
    while (vectorIndex < newVectorCapacity)
      {
      newSoundInstanceVector->at(vectorIndex) = new SoundInstance;
      newSoundInstanceVector->at(vectorIndex)->Clear();
      vectorIndex++;
      }

    // Clear out the old vector and point to the new one.
    soundInstanceVector->clear();
    delete(soundInstanceVector);
    soundInstanceVector = newSoundInstanceVector;
}


void SoundManager::PlaySound(int soundIndex, Ogre::SceneNode *soundNode, int *channelIndex)
{
    int            channelIndexTemp;
    FMOD_RESULT    result;
    FMOD_VECTOR    initialPosition;
    FMOD::Channel *channel;
    SoundInstance *soundInstance;

    if (soundIndex == INVALID_SOUND_INDEX)
      return;

    if (channelIndex)
      channelIndexTemp = *channelIndex;
    else
      channelIndexTemp = INVALID_SOUND_CHANNEL;

    Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound => Checking sound index");
    assert((soundIndex > 0) && (soundIndex < (int)soundInstanceVector->capacity()));
    Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound => Checking sound index.. It's OK");

    // If the channelIndex already has a sound assigned to it, test if it's the same sceneNode.
    if ((channelIndexTemp != INVALID_SOUND_CHANNEL) && (channelArray[channelIndexTemp].sceneNode != NULL))
    {
        Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound => Getting Channel");
        result = system->getChannel(channelIndexTemp, &channel);
        if (result == FMOD_OK)
        {
            bool isPlaying;

            result = channel->isPlaying(&isPlaying);
            if ((result == FMOD_OK) && (isPlaying == true) && (channelArray[channelIndexTemp].sceneNode == soundNode))
            {
                Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound => Another sound for this scene node");
                return;  // Already playing this sound attached to this node.
            }
        }
    }

    Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound correctly");
    soundInstance = soundInstanceVector->at(soundIndex);
    // Start the sound paused
    result = system->playSound(FMOD_CHANNEL_FREE, soundInstance->fmodSound, true, &channel);
    if (result != FMOD_OK)
      {
      Ogre::LogManager::getSingleton().logMessage(Ogre::String("SoundManager::PlaySound could not play sound  FMOD Error:") + FMOD_ErrorString(result));
      if (channelIndex)
         *channelIndex = INVALID_SOUND_CHANNEL;
      return;
      }

    channel->getIndex(&channelIndexTemp);
    channelArray[channelIndexTemp].sceneNode = soundNode;

    if (soundNode)
      {
      channelArray[channelIndexTemp].prevPosition = soundNode->getPosition();

      initialPosition.x = soundNode->getPosition().x;
      initialPosition.y = soundNode->getPosition().y;
      initialPosition.z = soundNode->getPosition().z;
      channel->set3DAttributes(&initialPosition, NULL);
      }

    result = channel->setVolume(1.0);
    // This is where the sound really starts.
    result = channel->setPaused(false);

    if (channelIndex)
      *channelIndex = channelIndexTemp;

    Ogre::LogManager::getSingleton().logMessage("SoundManager::PlaySound correctly");
}



void SoundManager::Set3DMinMaxDistance(int channelIndex, float minDistance, float maxDistance)
   {
   FMOD_RESULT    result;
   FMOD::Channel *channel;

   if (channelIndex == INVALID_SOUND_CHANNEL)
      return;

   result = system->getChannel(channelIndex, &channel);
   if (result == FMOD_OK)
      channel->set3DMinMaxDistance(minDistance, maxDistance);
   }



void SoundManager::StopAllSounds(void)
   {
   int            channelIndex;
   FMOD_RESULT    result;
   FMOD::Channel *nextChannel;

   for (channelIndex = 0; channelIndex < MAX_SOUND_CHANNELS; channelIndex++)
      {
      result = system->getChannel(channelIndex, &nextChannel);
      if ((result == FMOD_OK) && (nextChannel != NULL))
         nextChannel->stop();
      channelArray[channelIndex].Clear();
      }
   }


void SoundManager::StopSound(int *channelIndex)
   {
   if (*channelIndex == INVALID_SOUND_CHANNEL)
      return;

   FMOD::Channel *soundChannel;

   assert((*channelIndex > 0) && (*channelIndex < MAX_SOUND_CHANNELS));

   system->getChannel(*channelIndex, &soundChannel);
   soundChannel->stop();

   channelArray[*channelIndex].Clear();
   *channelIndex = INVALID_SOUND_CHANNEL;
   }



int SoundManager::FindSound(Ogre::String &fileName, SOUND_TYPE soundType)
{
int            vectorIndex;
int            vectorCapacity;
SoundInstance *nextSoundInstance;

vectorCapacity = (int)soundInstanceVector->capacity();
for (vectorIndex = 0; vectorIndex < vectorCapacity; vectorIndex++)
  {
  nextSoundInstance = soundInstanceVector->at(vectorIndex);
  if ((soundType == nextSoundInstance->soundType) && (fileName == nextSoundInstance->fileName))
//if ((soundType == nextSoundInstance->soundType) && (fileName == nextSoundInstance->fileArchive->getName()))
     return vectorIndex;
  }

return INVALID_SOUND_INDEX;
}



float SoundManager::GetSoundLength(int soundIndex)
   {
   if (soundIndex == INVALID_SOUND_INDEX)
      return 0.0;

   assert((soundIndex > 0) && (soundIndex < (int)soundInstanceVector->capacity()));

   unsigned int   soundLength;   // length in milliseconds
   FMOD_RESULT    result;
   SoundInstance *soundInstance;

   soundInstance = soundInstanceVector->at(soundIndex);
   if (soundInstance)
      {
      result = soundInstance->fmodSound->getLength(&soundLength, FMOD_TIMEUNIT_MS);
      if (result != FMOD_OK)
         {
         Ogre::LogManager::getSingleton().logMessage(Ogre::String("SoundManager::GetSoundLength could not get length  FMOD Error:") + FMOD_ErrorString(result));
         return 0.0;
         }
      }
   else
      {
      Ogre::LogManager::getSingleton().logMessage(Ogre::String("SoundManager::GetSoundLength could not find soundInstance"));
      return 0.0;
      }

   return (float)soundLength / 1000.0f;
   }

SoundInstance *SoundManager::GetSoundInstance(int soundIndex)
{
   return soundInstanceVector->at(soundIndex);
}

FMOD::Channel *SoundManager::GetSoundChannel(int channelIndex)
{
   if (channelIndex == INVALID_SOUND_CHANNEL)
      return NULL;

   FMOD::Channel *soundChannel;

   assert((channelIndex > 0) && (channelIndex < MAX_SOUND_CHANNELS));

   system->getChannel(channelIndex, &soundChannel);
   return soundChannel;
}
