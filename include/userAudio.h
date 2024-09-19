#pragma once
#include <cstdint>
#include "audioManager.h"
/*
  * The user of the game engine will be able to define their own audio here
  * They will be added to the audios vector at the end
  * Thus the user will be able to easily and efficiently play audio using the enum number
  * these are uint8_t's so it will be easy to play audio efficiently
  */

enum USER_AUDIO : uint8_t
{
  //NOTE:
  //THE FIRST ELEMENT MUST BE EQUAL TO BASIC_SOUNDS_SIZE
  //DRASTIC CONSEQUENCES WILL OCCUR IF THIS DOES NOT HAPPEN
  //YOU HAVE BEEN WARNED
  //RENAME THE FIRST PART OF THIS SOUND
  FIRST_AUDIO_NAME = BASIC_SOUNDS_SIZE,
  
  //LEAVE THIS ELEMENT AS IS. IT IS USED INTERNALLY
  USER_AUDIO_SIZE,
};
