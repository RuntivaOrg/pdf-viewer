#include "painter.h"
#include "paint_device.h"
#include "deja_vu_sans_mono.h"
#include "color.h"
#include "SDL_ttf.h"

#include "macros.h"
DISABLE_WARNINGS
#include "SDL.h"
#include "SDL_image.h"
ENABLE_WARNINGS

#include <stdexcept>


Painter::Painter(PaintDevice *paintDevice):
  renderer_(paintDevice->renderer()),
  width_(paintDevice->width()),
  height_(paintDevice->height())
{
  font_ = TTF_OpenFontRW(SDL_RWFromMem(static_cast<void*>(const_cast<unsigned char *>(DejaVuSansMono)), sizeof(DejaVuSansMono)), 1, 10);
  if (font_ == nullptr) {
    printf("Error: TTF_OpenFont\n");
    abort();
  }
}

Painter::~Painter()
{
  for (auto &i: glyphCache_)
    SDL_DestroyTexture(std::get<0>(i.second));
  TTF_CloseFont(font_);
}

void Painter::drawLine(int x1, int y1, int x2, int y2)
{
  SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void Painter::drawPoint(int x, int y)
{
  SDL_RenderDrawPoint(renderer_, x, y);
}

void Painter::drawRect(int x, int y, int width, int height)
{
    //Uint8 r, g, b, a;
    //SDL_GetRenderDrawColor(renderer_, &r, &g, &b, &a);
    //printf("rect color: %X%X%X\n", r, g, b);
  SDL_Rect rect = { x, y, width, height };
  SDL_RenderFillRect(renderer_, &rect);
}

void Painter::setColor(Color color)
{
  SDL_SetRenderDrawColor(renderer_, (color >> 24) & 0xff, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
}

void Painter::renderGlyph(wchar_t ch, int x, int y, Color fg, Color bg)
{
  SDL_Texture *texture;
  int width, height;
  auto iter = glyphCache_.find(std::make_tuple(ch, fg, bg));
  if (iter != end(glyphCache_))
  {
    texture = std::get<0>(iter->second);
    width = std::get<1>(iter->second);
    height = std::get<2>(iter->second);
    glyphCacheAge_.erase(std::get<3>(iter->second));
    std::get<3>(iter->second) = glyphCacheAge_.insert(end(glyphCacheAge_), iter->first);
  }
  else
  {
    if (glyphCache_.size() > 4096)
    {
      const auto itemsCount = glyphCache_.size();
      for (size_t j = 0; j < itemsCount / 2; ++j)
      {
        auto key = glyphCacheAge_.front();
        auto iter2 = glyphCache_.find(key);
        SDL_DestroyTexture(std::get<0>(iter2->second));
        glyphCache_.erase(iter2);
        glyphCacheAge_.erase(begin(glyphCacheAge_));
      }
    }
    auto surf = TTF_RenderGlyph_Shaded(font_, static_cast<uint16_t>(ch), toSdlColor(fg), toSdlColor(bg));
    if (surf == nullptr)
    {
      setColor(bg);
      drawRect(x, y, glyphWidth(), glyphHeight());
      return;
    }
    texture = SDL_CreateTextureFromSurface(renderer_, surf);
    if (texture == nullptr)
    {
      SDL_FreeSurface(surf);
      printf("ERROR: CreateTexture\n");
      abort();

    }
    width = surf->w;
    height = surf->h;
    SDL_FreeSurface(surf);
    GlyphCacheKey key = std::make_tuple(ch, fg, bg);
    glyphCache_.insert(std::make_pair(key, std::make_tuple(texture, width, height, glyphCacheAge_.insert(end(glyphCacheAge_), key))));
  }
  SDL_Rect rect { x, y, width, height };
  SDL_RenderCopy(renderer_, texture, nullptr, &rect);
}

int Painter::glyphWidth() const
{
  int minx, maxx, miny, maxy, advance;
  TTF_GlyphMetrics(font_, L'W', &minx, &maxx, &miny, &maxy, &advance);
  return advance;
}

int Painter::glyphHeight() const
{
  return TTF_FontLineSkip(font_) + 1;
}


void Painter::render_image(const std::vector<uint8_t>& img_buf, int x, int y, int w, int h) {

  /* load the image */
  SDL_Surface *image = nullptr;
  
  SDL_RWops* rw = SDL_RWFromMem(static_cast<void*>(const_cast<uint8_t*>(img_buf.data())), static_cast<int>(img_buf.size()));
  image = IMG_LoadPNG_RW(rw);

  //image = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_BGRA32);
  //image = SDL_CreateRGBSurfaceWithFormat(0, w, h, 24, SDL_PIXELFORMAT_RGB24);
  //SDL_PIXELFORMAT_RGB24
  //image = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0xFF0000, 0xFF00, 0xFF);

  //if (SDL_MUSTLOCK(image)) SDL_LockSurface(image);
  ////image = SDL_CreateRGBSurface(0, w, h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
  //if (image) {
  //    memcpy(image->pixels, img_buf.data(), img_buf.size());// static_cast<size_t>(w * h * 4));
  //}

  //if (SDL_MUSTLOCK(image)) SDL_UnlockSurface(image);

  //free(data);


  ////const char* filename = "C:\\Temp\\SDL2_IMage\\demos\\lena.bmp";
  //const char* filename = "airplane.pdf.0.ppm";
  ////const char* filename = "lena.png";
  ////SDL_Surface* image;
  //const char* filename = "C:\\work\\PDF\\SampleFiles\\airplane.pdf.0.ppm";
  ////image=IMG_Load(filename);

  //image=IMG_LoadTyped_RW(SDL_RWFromFile(filename, "rb"), 1, "PNM");

  //if(!image)
  //{
  //  /* image failed to load */
  //  printf("ERROR: IMG_Load: %s\n", IMG_GetError());
  //  return;
  //}

  /* print some info about the image */
//  printf("loaded %s: %dx%d %dbpp\n", filename,
//      image->w, image->h, image->format->BitsPerPixel);
  int iw = image->w;
  int ih = image->h;
  if (iw != w || ih != h) {
    printf("render_image issue: image invalid size: w: %d:%d h: %d:%d\n", iw, w, ih, h);
  }
  SDL_Texture *texture;
  texture = SDL_CreateTextureFromSurface(renderer_, image);
  if (texture == nullptr)
  {
    SDL_FreeSurface(image);
    printf("CreateTexture2\n");
    abort();
  }
  SDL_FreeSurface(image);

  SDL_Rect rect { x, y, iw, ih };
  //SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, texture, nullptr, &rect);
  //SDL_RenderPresent(renderer_);
  SDL_DestroyTexture(texture);
}


// void Painter::render_image([[maybe_unused]]const std::vector<uint8_t>& img_buf, int x, int y, int w, int h) {

//   /* load the image */
//   SDL_Surface *image;
//   //image = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_BGRA32);
//   //image = SDL_CreateRGBSurfaceWithFormat(0, w, h, 24, SDL_PIXELFORMAT_RGB24);
//   //SDL_PIXELFORMAT_RGB24
//   image = SDL_CreateRGBSurface(0, w, h, 32, 0xFF000000, 0xFF0000, 0xFF00, 0xFF);

//   if (SDL_MUSTLOCK(image)) SDL_LockSurface(image);
//   //image = SDL_CreateRGBSurface(0, w, h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
//   if (image) {
//       memcpy(image->pixels, img_buf.data(), img_buf.size());// static_cast<size_t>(w * h * 4));
//   }

//   if (SDL_MUSTLOCK(image)) SDL_UnlockSurface(image);

//   //free(data);


//   ////const char* filename = "C:\\Temp\\SDL2_IMage\\demos\\lena.bmp";
//   //const char* filename = "airplane.pdf.0.ppm";
//   ////const char* filename = "lena.png";
//   ////SDL_Surface* image;
//   //const char* filename = "C:\\work\\PDF\\SampleFiles\\airplane.pdf.0.ppm";
//   ////image=IMG_Load(filename);

//   //image=IMG_LoadTyped_RW(SDL_RWFromFile(filename, "rb"), 1, "PNM");

//   //if(!image)
//   //{
//   //  /* image failed to load */
//   //  printf("ERROR: IMG_Load: %s\n", IMG_GetError());
//   //  return;
//   //}

//   /* print some info about the image */
// //  printf("loaded %s: %dx%d %dbpp\n", filename,
// //      image->w, image->h, image->format->BitsPerPixel);
//   int iw = image->w;
//   int ih = image->h;
//   if (iw != w || ih != h) {
//     printf("render_image issue: image invalid size\n");
//   }
//   SDL_Texture *texture;
//   texture = SDL_CreateTextureFromSurface(renderer_, image);
//   if (texture == nullptr)
//   {
//     SDL_FreeSurface(image);
//     throw std::runtime_error("CreateTexture");
//   }
//   SDL_FreeSurface(image);

//   SDL_Rect rect { x, y, w, h };
//   //SDL_RenderClear(renderer_);
//   SDL_RenderCopy(renderer_, texture, nullptr, &rect);
//   SDL_RenderPresent(renderer_);
//   SDL_DestroyTexture(texture);
// }
