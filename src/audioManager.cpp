#include <cmath>
#include <numbers>
#include "audioManager.h"
#include "userAudio.h"
#include "geometricPrimitives.h"
#include "vectorMathEssentials.h"
#include "miscFunctions.h"

AudioManager::AudioManager()
{
  this->audios = vector<int>(USER_AUDIO_SIZE, -1);
}

//Return the singleton instance of the AudioManager class
AudioManager& AudioManager::Get()
{
  static AudioManager instance;
  return instance;
}

//Registers a new audio
void AudioManager::AddAudio(const uint8_t& audioNumber, const string& audioName)
{
  string filePath = GetPathFromAudio({audioName});
  if(filePath == "NULL PATH")
  {
    cerr << "Failed to load audio file named " << audioName << '\n';
    return;
  }
  Get().audios[audioNumber] = Get().audioEngine.LoadSound(filePath);
}

//Removes a registered audio
void AudioManager::RemoveAudio(const uint8_t& audioNumber)
{
  int audioToRemove = Get().audios[audioNumber];
  Get().audioEngine.UnloadSound(audioToRemove);
}

//This function will play a sound at a max volume and with a pan of 1 as default values
void AudioManager::PlaySound(const uint8_t& audioNumber)
{
  int audioToPlay = Get().audios[audioNumber];
  Get().audioEngine.Toggle(audioToPlay);
}

//This function will play a function with a pan and volume such that it seems that it is Playing
//from a certain location
void AudioManager::PlaySoundAtLocation(const uint8_t& audioNumber, Player* player, const Vector3D& audioLocation,const float& dropoffRate, const float& dropoffDistance)
{
  //This is where we will calculate the volume needed
  if(dropoffDistance <= 0 || dropoffRate <= 0 || dropoffRate >= 1.0f)
  {
    cerr << "Error: Distance cannot be zero or dropoffRate is not between 0 and 1. Playing sound without location information\n";
    Get().PlaySound(audioNumber);
  }

  const Vector3D& sourceLocation = player->camera.cameraPosition;
  const Vector3D& facingVector = player->camera.GetFacingVector();

  const float dropoffNum = 1.0f / (1.0f - dropoffRate);
  float distance = GetDistanceBetweenPoints(sourceLocation, audioLocation);
  float exponentialValue = (-1.0f * distance * log(dropoffNum)) / dropoffDistance;

  //This is the final volume value
  float volume = pow(numbers::e, exponentialValue);
  
  //This is where we will calculate the pan needed
  Vector3D relativeTarget = SubtractVector(audioLocation, sourceLocation);
  float dotProduct = GetDotProduct(relativeTarget, facingVector);
  float relativeTargetMagnitude = GetVectorLength(relativeTarget);
  float facingVectorMagnitude = GetVectorLength(facingVector);
  float angle = acos((dotProduct / (relativeTargetMagnitude * facingVectorMagnitude)));
  //This is the final pan value
  float pan = sin(angle);
  cout << "Pan: " << pan << '\n';
  int trueAudioNumber = Get().audios[audioNumber];
  Get().audioEngine.SetVolume(trueAudioNumber, volume);
  Get().audioEngine.SetPan(trueAudioNumber, pan);
  auto& audiosVec = Get().audios;
  Get().audioEngine.Toggle(trueAudioNumber);
}

