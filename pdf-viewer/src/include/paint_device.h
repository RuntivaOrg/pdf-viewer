#pragma once

#include "macros.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wc++98-compat-pedantic"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include <SDL.h>
#pragma GCC diagnostic pop


class PaintDevice
{
public:
  virtual ~PaintDevice() = default;
  virtual SDL_Renderer *renderer() = 0;
  virtual int width() const = 0;
  virtual int height() const = 0;
};
