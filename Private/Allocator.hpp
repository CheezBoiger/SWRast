//
#pragma once
#include "Memory.hpp"


namespace swrast {


class allocator_t 
{
public:
    allocator_t(uint64_t bytes_to_allocate = 0ull)  
        : memory_pool({})
    {
        if (bytes_to_allocate)
        {
            memory_pool.preallocate(bytes_to_allocate);
        } 
    }
    allocator_t(const memory_pool_t& pool)
    {
        if (memory_pool.get_base_address())
            memory_pool.release();
        memory_pool = pool;
    }

    virtual ~allocator_t() { release(); }

    virtual error_t initialize(uint64_t bytes_to_allocate) { return result_failed; }
    virtual void* allocate(uint64_t requested_size_bytes, uint16_t alignment) { return nullptr; }
    virtual void free(void* ptr) { }

    virtual void reset() { } 
    virtual void release() { }
protected:
    memory_pool_t memory_pool;
private:
    uint64_t bytes_allocated;
    uint64_t objects_allocated;
};


class malloc_allocator_t : public allocator_t
{
public:
    malloc_allocator_t()
        : allocator_t()
    { }

    void* allocate(uint64_t requested_size_bytes, uint16_t alignment) override
    {
        return malloc(requested_size_bytes + alignment);
    }

    void free(void* ptr) override
    {
        ::free(ptr);
    }
};


class linear_allocator : public allocator_t
{
public:
        
private:

};
} // swrast