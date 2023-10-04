//
#pragma once

#include "Context.hpp"
#include <cstdlib>

namespace swrast {


#define SWRAST_MEM_1KB(n) ((1024ULL) * (n))
#define SWRAST_MEM_1MB(n) (SWRAST_MEM_1KB(n) * (1024ULL))
#define SWRAST_MEM_1GB(n) (SWRAST_MEM_1MB(n) * (1024ULL))

class memory_pool_t
{
public:
    memory_pool_t() 
        : m_size_bytes(0ULL)
        , m_base_address(0ULL) 
    { }

    ~memory_pool_t() 
    {
        release();
    }

    // Preallocate a chunk of data.
    error_t preallocate(uint64_t size_bytes)
    {
        if (m_base_address != 0)
        {
            free((void*)m_base_address);
        }
        void* ptr = malloc(sizeof(uint8_t) * size_bytes);
        m_base_address = (uintptr_t)ptr;
        m_size_bytes = size_bytes;
        return result_ok;
    }

    // Manually release pool memory.
    error_t release()
    {
        if (m_base_address != 0)
        {
           free((void*)m_base_address);
        } 
        m_base_address = 0ULL;
        return result_ok;
    }

    uintptr_t get_base_address() const { return m_base_address; }
    uint64_t get_memory_size_bytes() const { return m_size_bytes; }
private:
    // Base address of the memory pool. This needs to be 
    // pre allocated.
    uintptr_t m_base_address;
    uint64_t m_size_bytes;
};
} // swrast