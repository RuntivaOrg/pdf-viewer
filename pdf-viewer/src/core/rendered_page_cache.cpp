#include "core/rendered_page_cache.h"

#include <iterator>
#include <cassert>

#if defined(USE_SYSTEM_ZLIB)
#include <zlib.h>
#else
#include "third_party/zlib/zlib.h"
#endif

#define COMPRESS_RENDER_PAGE_CACHE = 1


#ifdef COMPRESS_RENDER_PAGE_CACHE
  enum class DeflateLevel {
  DEFLATE_NONE = 0,
  DEFLATE_BEST_SPEED = 1,
  DEFLATE_BEST = 9,
  DEFLATE_DEFAULT = -1
};

int deflate_array(const std::vector<unsigned char>& source,
                  std::vector<unsigned char>& dest,
                  const int level);

int inflate_array(const std::vector<unsigned char>& source,
                  std::vector<unsigned char>& dest);

void zerr(int ret);

#endif

RenderedPageCache::RenderedPageCache(bool enable_cache,
    size_t max_cache_size_bytes)
    : cache_enabled(enable_cache),
      max_cache_size_bytes(max_cache_size_bytes),
      current_cache_size_bytes(0) {

}

RenderedPageCache::~RenderedPageCache() { }

RenderedPageCache::CacheItem::CacheItem(int page_index,
                                        float scale,
                                        std::vector<uint8_t> data)
    : page_index(page_index),
      scale(scale), data(std::move(data)) {}

RenderedPageCache::CacheItem::~CacheItem() {}

RenderedPageCache::CacheItem::CacheItem(const CacheItem& other) {
  page_index = other.page_index;
  scale = other.scale;
  data.assign(other.data.begin(), other.data.end());
}

bool RenderedPageCache::exists(int page_index, float scale) const {
    if (!cache_enabled)
        return false;

    for (auto it = _cache.begin(); it < _cache.end(); it++) {
    if (it->page_index == page_index && it->scale == scale)
        return true;
    }

    return false;
}

std::optional<std::vector<uint8_t>> RenderedPageCache::get(
    int page_index,
    float scale) const {

    if (!cache_enabled)
        return std::nullopt;

    for (auto it = _cache.begin(); it < _cache.end(); it++) {
        if (it->page_index == page_index && it->scale == scale) {

#ifdef COMPRESS_RENDER_PAGE_CACHE
            std::vector<uint8_t> vec_out;

            int zlib_ret = inflate_array(it->data, vec_out);
            if (zlib_ret != Z_OK) {
                zerr(zlib_ret);
                return std::nullopt;
            }

            return std::make_optional<std::vector<uint8_t>>(vec_out);
#else
          return std::make_optional<std::vector<uint8_t>>(it->data);
#endif
        }
    }

    return std::nullopt;
}

void RenderedPageCache::add(
    const int page_index,
    const float scale,
    const std::vector<uint8_t>& render_data) {

    if (!cache_enabled)
        return;

#ifdef COMPRESS_RENDER_PAGE_CACHE
    std::vector<uint8_t> vec_buffer;
    int zlib_ret =
        deflate_array(render_data, vec_buffer, Z_DEFAULT_COMPRESSION);
    if (zlib_ret != Z_OK) {
      zerr(zlib_ret);
      return; // zlib_ret;
    }
#endif

    for (auto it = _cache.begin(); it < _cache.end(); it++) {
      if (it->page_index == page_index && it->scale == scale) {
          // subtrack current item buffer size and then add new buffer size  
          current_cache_size_bytes -= it->data.size();

#ifdef COMPRESS_RENDER_PAGE_CACHE
          current_cache_size_bytes += vec_buffer.size();
          it->data = vec_buffer;
#else
          current_cache_size_bytes += render_data.size();
          it->data = render_data;
#endif
          // NOTE: We do not do any cache cleanp when an add replaces an
          // existing cache item
          return;
      }
    }

#ifdef COMPRESS_RENDER_PAGE_CACHE
    current_cache_size_bytes += vec_buffer.size();
    const CacheItem item = CacheItem(page_index, scale, vec_buffer);
#else
    current_cache_size_bytes += render_data.size();
    const CacheItem item = CacheItem(page_index, scale, render_data);
#endif
    _cache.push_back(std::move(item));



    printf("Creating render cache item: %d - %f (cache size: %llu)\n",
           page_index, scale, current_cache_size_bytes);

    // TODO: Need to loop and delete until we get back under the max limit
    if (current_cache_size_bytes > max_cache_size_bytes && _cache.size() > 1) {
        
        //current_cache_size_bytes -= _cache.begin()->data.size();
        //_cache.erase(_cache.begin());   

        //printf("clearing render cache item: %d - %f (cache size: %llu)\n",
        //     page_index, scale, current_cache_size_bytes);
    }
}


void RenderedPageCache::remove(int page_index, float scale) {
    if (!cache_enabled)
        return;

    for (auto it = _cache.begin(); it < _cache.end(); it++) {
        if (it->page_index == page_index && it->scale == scale) {
            current_cache_size_bytes -= it->data.size();

            printf("Deleting render cache item: %d - %f (cache size: %llu)\n",
                 page_index, scale, current_cache_size_bytes);

            _cache.erase(it);
            return;
        }
    }
}

void RenderedPageCache::flush() {
    printf("Flushing render cache\n");
    current_cache_size_bytes = 0;
    _cache.clear();
}



//void compress_memory(void* in_data,
//                     size_t in_data_size,
//                     std::vector<uint8_t>& out_data) {
//  std::vector<uint8_t> buffer;
//
//  const size_t BUFSIZE = 128 * 1024;
//  uint8_t temp_buffer[BUFSIZE];
//
//  z_stream strm;
//  strm.zalloc = 0;
//  strm.zfree = 0;
//  strm.next_in = reinterpret_cast<uint8_t*>(in_data);
//  strm.avail_in = in_data_size;
//  strm.next_out = temp_buffer;
//  strm.avail_out = BUFSIZE;
//
//  deflateInit(&strm, Z_BEST_COMPRESSION);
//
//  while (strm.avail_in != 0) {
//    [[maybe_unused]]
//    int res = deflate(&strm, Z_NO_FLUSH);
//    assert(res == Z_OK);
//    if (strm.avail_out == 0) {
//      buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
//      strm.next_out = temp_buffer;
//      strm.avail_out = BUFSIZE;
//    }
//  }
//
//  int deflate_res = Z_OK;
//  while (deflate_res == Z_OK) {
//    if (strm.avail_out == 0) {
//      buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
//      strm.next_out = temp_buffer;
//      strm.avail_out = BUFSIZE;
//    }
//    deflate_res = deflate(&strm, Z_FINISH);
//  }
//
//  assert(deflate_res == Z_STREAM_END);
//  buffer.insert(buffer.end(), temp_buffer,
//                temp_buffer + BUFSIZE - strm.avail_out);
//  deflateEnd(&strm);
//
//  out_data.swap(buffer);
//}

// The following was pulled from the previous runtiva-pdf FlateFilter cpp file

#define CHUNK 16384

void write_to_vec(std::vector<unsigned char>& out,
                  const unsigned char* buf,
                  size_t bufLen) {
  out.insert(out.end(), buf, buf + bufLen);
}

size_t read_from_vec(const std::vector<unsigned char>& in,
                     unsigned char*& inBuf,
                     size_t& inPosition) {
  size_t from = inPosition;
  inBuf = const_cast<unsigned char*>(in.data()) + inPosition;
  inPosition += std::min(static_cast<size_t>(CHUNK), in.size() - from);
  return inPosition - from;
}

bool is_end_of_vec(const std::vector<unsigned char>& in,
                   const size_t& inPosition) {
  return (in.size() == inPosition);
}

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
int deflate_array(const std::vector<unsigned char>& source,
                  std::vector<unsigned char>& dest,
                  const int level) {
  int ret, flush;
  unsigned have;
  z_stream strm;
  unsigned char* in;
  unsigned char out[CHUNK];

  size_t sourcePosition = 0; /* position indicator of "source" */

  /* allocate deflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  ret = deflateInit(&strm, level);
  if (ret != Z_OK)
    return ret;

  /* compress until end of file */
  do {
    // strm.avail_in = (uInt)source.readsome((char*)in, CHUNK);
    strm.avail_in = static_cast<uInt>(
        read_from_vec(source, (unsigned char*&)in, sourcePosition));

    if (strm.avail_in == 0) {
      break;
    }

    flush = is_end_of_vec(source, sourcePosition) ? Z_FINISH : Z_NO_FLUSH;

    strm.next_in = in;

    /* run deflate() on input until output buffer not full, finish
        compression if all of source has been read in */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = deflate(&strm, flush);   /* no bad return value */
      assert(ret != Z_STREAM_ERROR); /* state not clobbered */

      have = CHUNK - strm.avail_out;

      write_to_vec(dest, out, have);
    } while (strm.avail_out == 0);
    assert(strm.avail_in == 0); /* all input will be used */

    /* done when last data in file processed */
  } while (flush != Z_FINISH);
  assert(ret == Z_STREAM_END); /* stream will be complete */

  /* clean up and return */
  (void)deflateEnd(&strm);
  return Z_OK;
}

/* Decompress from source vector to dest vector until source vector is
   processed. inflate_array() returns Z_OK on success, Z_MEM_ERROR if memory
   could not be allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
int inflate_array(const std::vector<unsigned char>& source,
                  std::vector<unsigned char>& dest) {
  int ret;
  unsigned have;
  z_stream strm = {};
  unsigned char* in;
  unsigned char out[CHUNK];

  size_t sourcePosition = 0; /* position indicator of "source" */

  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  ret = inflateInit(&strm);
  if (ret != Z_OK)
    return ret;

  /* decompress until deflate stream ends or end of file */
  do {
    strm.avail_in =
        (uInt)read_from_vec(source, (unsigned char*&)in, sourcePosition);

    if (strm.avail_in == 0)
      break;
    strm.next_in = in;

    /* run inflate() on input until output buffer not full */
    do {
      strm.avail_out = CHUNK;
      strm.next_out = out;
      ret = inflate(&strm, Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR); /* state not clobbered */
      switch (ret) {
        case Z_NEED_DICT:
          ret = Z_DATA_ERROR; /* and fall through */
          __fallthrough;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
          (void)inflateEnd(&strm);
          return ret;
      }
      have = CHUNK - strm.avail_out;

      write_to_vec(dest, (const unsigned char*)out, have);

    } while (strm.avail_out == 0);

    /* done when inflate() says it's done */
  } while (ret != Z_STREAM_END);

  /* clean up and return */
  (void)inflateEnd(&strm);
  return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret) {
  fputs("zpipe: ", stderr);
  switch (ret) {
    case Z_ERRNO:
      if (ferror(stdin))
        fputs("error reading stdin\n", stderr);
      if (ferror(stdout))
        fputs("error writing stdout\n", stderr);
      break;
    case Z_STREAM_ERROR:
      fputs("invalid compression level\n", stderr);
      break;
    case Z_DATA_ERROR:
      fputs("invalid or incomplete deflate data\n", stderr);
      break;
    case Z_MEM_ERROR:
      fputs("out of memory\n", stderr);
      break;
    case Z_VERSION_ERROR:
      fputs("zlib version mismatch!\n", stderr);
  }
}