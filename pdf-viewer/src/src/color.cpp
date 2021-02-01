#include "color.h"

SDL_Color toSdlColor(Color value)
{
  SDL_Color color;
  color.r = static_cast<Uint8>((value >> 24) & 0xff); 
  color.g = static_cast<Uint8>((value >> 16) & 0xff); 
  color.b = static_cast<Uint8>((value >> 8) & 0xff); 
  color.a = static_cast<Uint8>(value & 0xff);
  return color;
}
