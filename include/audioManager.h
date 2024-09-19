#pragma once
#define OLC_PGEX_MINIAUDIO
#include <cstdint>
#include <vector>
#include <string>
#include "olcPGEX_MiniAudio.h"
#include "miscPrimitives.h"

using namespace std;

enum BASIC_SOUNDS : uint8_t
{
  WALK_SOUND,
  JUMP_SOUND,
  RUN_SOUND,
  ATTACK_SOUND,
  HEAL_SOUND,
  DAMAGE_SOUND,

  AMBIENT_SOUND,
  INTERACT_SOUND,
  NOTIFICATION_SOUND,
  ERROR_SOUND,

  BASIC_SOUNDS_SIZE,
};

class AudioManager
{
private:
  //This vecotor is used to map the internal audio IDs with enum members we can understand
  vector<int> audios;
  olc::MiniAudio audioEngine;
  AudioManager();

public:
  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;
  static AudioManager& Get();
  static void AddAudio(const uint8_t& audioNumber, const string& audioName);
  static void RemoveAudio(const uint8_t& audioNumber);
  static void PlaySound(const uint8_t& audioNumber);
  static void PlaySoundAtLocation(const uint8_t& audioNumber, Player* player, const Vector3D& audioLocation, const float& dropoffRate, const float& dropoffDistance);
};
