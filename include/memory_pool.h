#pragma once

#include <vector>
#include <map>

class MemoryPool {
public:
        MemoryPool(const unsigned int bytes);
        ~MemoryPool();

        void* allocate(const unsigned int size);
        void deallocate(void* pointer);

        void clear();
        void consolidate();

        unsigned int size();
        unsigned int free();
        unsigned int free_total();

protected:
        std::map<unsigned int, std::vector<void*>> freed_blocks_;
        std::map<unsigned int, std::vector<void*>> in_use_blocks_;

        const int size_;
        char* pool_ = nullptr;

        char* free_head_;
};
