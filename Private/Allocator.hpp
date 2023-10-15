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
        resize_pool(bytes_to_allocate);
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

    void resize_pool(uint64_t bytes_to_allocate)
    {
        if (bytes_to_allocate)
        {
            memory_pool.preallocate(bytes_to_allocate);
        }
    }

    uint64_t get_memory_pool_size_bytes() const 
    {
        return memory_pool.get_memory_size_bytes();
    }
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


class linear_allocator_t : public allocator_t
{
public:
 
    void* allocate(uint64_t requested_size_bytes, uint16_t alignment) override
    {
        const uintptr_t base_address = memory_pool.get_base_address();
        const uint64_t max_size = memory_pool.get_memory_size_bytes();
        const uint64_t used_size = top - base_address;
        const uint64_t new_size = used_size + requested_size_bytes;
        if (new_size < max_size)
        {
            uintptr_t allocation = top;
            top += requested_size_bytes;
            return (void*)allocation;
        }
        return nullptr;
    }
        

    void free(void* ptr) override
    {   
    }

    void reset() 
    {
        top = memory_pool.get_base_address();
    }
    
private:
    uintptr_t top = 0;
};
} // swrast