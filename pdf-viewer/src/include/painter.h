#pragma once
#include "color.h"
#include "macros.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wc++98-compat-pedantic"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include <SDL.h>
#include <SDL_ttf.h>
#pragma GCC diagnostic pop

#include <string>
#include <map>
#include <tuple>
#include <list>
#include <vector>

class PaintDevice;

class Painter
{
public:
  Painter(PaintDevice *);
  ~Painter();
  void drawLine(int x1, int y1, int x2, int y2);
  void drawPoint(int x, int y);
  void drawRect(int x1, int y1, int x2, int y2);
  void setColor(Color);
  void renderGlyph(wchar_t ch, int x, int y, Color fg, Color bg);
  int glyphWidth() const;
  int glyphHeight() const;
  void render_image(const std::vector<uint8_t>& img_buf, int x, int y, int w, int h);
private:
  SDL_Renderer *renderer_;
  int width_;
  int height_;
  TTF_Font *font_;
  typedef std::tuple<wchar_t, Color, Color> GlyphCacheKey;
  typedef std::map<GlyphCacheKey, std::tuple<SDL_Texture *, int, int, std::list<GlyphCacheKey>::iterator> > GlyphCache;
  GlyphCache glyphCache_;
  std::list<GlyphCacheKey> glyphCacheAge_;
};
