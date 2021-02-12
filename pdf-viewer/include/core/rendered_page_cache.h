#ifndef CORE_RENDERED_PAGE_CACHE_H_
#define CORE_RENDERED_PAGE_CACHE_H_
#pragma once

#include <vector>
#include <string>
#include <optional>

static constexpr size_t const& DEFAULT_RENDER_CACHE_SIZE = 10 * 1024 * 1024; // 10MB 
class RenderedPageCache {
public:
    RenderedPageCache(bool enable_cache = true,
                    size_t max_cache_size_bytes = DEFAULT_RENDER_CACHE_SIZE);
    ~RenderedPageCache();

    bool exists(const int page_index, const float scale) const;
    std::optional<std::vector<uint8_t>> get(const int page_index,
                                                   const float scale) const;
    void add(const int page_index,
                                    const float scale,
                                    const std::vector<uint8_t>& render_data);
    void remove(const int page_index, const float scale);
    void flush();


 private:
    class CacheItem {
    public:
        CacheItem(int page_index,
                float scale,
                std::vector<uint8_t> data);
        ~CacheItem();
        CacheItem(const CacheItem& other);
        CacheItem& operator=(const CacheItem& other) {
          page_index = other.page_index;
          scale = other.page_index;
          data = other.data;

          return *this;
        }

        int page_index;
        float scale;
        std::vector<uint8_t> data;
    };

    bool cache_enabled;
    size_t max_cache_size_bytes;
    size_t current_cache_size_bytes;
    std::vector<CacheItem> _cache;


};

#endif // CORE_RENDERED_PAGE_CACHE_H_
