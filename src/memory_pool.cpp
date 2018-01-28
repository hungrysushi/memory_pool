
#include "memory_pool.h"

#include <algorithm>

MemoryPool::MemoryPool(const unsigned int bytes)
        : size_(bytes),
          pool_(new char[bytes]) {

        // set up head to point to beginning of pool
        free_head_ = pool_;
}

MemoryPool::~MemoryPool() {
        delete[] pool_;
}

void* MemoryPool::allocate(const unsigned int allocate_size) {
        void* alloc_pointer = nullptr;  // return this if there's no space

        // see if there is enough space in the pool
        if (free() >= allocate_size) {

                // add this block to the in-use pool
                in_use_blocks_[allocate_size].emplace_back(free_head_);

                alloc_pointer = free_head_;

                free_head_ += allocate_size;  // update the free pool head
        }
        // check if any of the previously freed chunks are the correct size
        else if (freed_blocks_[allocate_size].size() > 0) {

                // take the pointer from the back and push it onto in-use
                std::vector<void*>::iterator block = freed_blocks_[allocate_size].end() - 1;

                in_use_blocks_[allocate_size].push_back(*block);
                freed_blocks_[allocate_size].erase(block);

                alloc_pointer = *block;
        }

        return alloc_pointer;
}

void MemoryPool::deallocate(void* pointer) {
        // find the block with this pointer
        for (auto it = in_use_blocks_.begin(); it != in_use_blocks_.end(); it++) {
                std::vector<void*>::iterator exists = std::find(it->second.begin(), it->second.end(), pointer);

                if (exists != it->second.end()) {
                        // move in-use block to free pool and delete from in-use
                        freed_blocks_[it->first].push_back(*exists);
                        it->second.erase(exists);
                }
        }
}

void MemoryPool::clear() {
        // reset head to beginning of pool and remove all map contents
        free_head_ = pool_;
        in_use_blocks_.clear();
        freed_blocks_.clear();
}

void MemoryPool::consolidate() {
        // TODO
}

unsigned int MemoryPool::size() {
        return size_;
}

unsigned int MemoryPool::free() {
        // return free bytes in contiguous pool
        return size_ - (free_head_ - pool_);
}

unsigned int MemoryPool::free_total() {
        // add up the total free bytes across unallocated and freed space
        unsigned int free_bytes = free();

        for (auto it = freed_blocks_.begin(); it != freed_blocks_.end(); it++) {
                // multiply size (key) by the number of freed blocks
                free_bytes += (it->first * it->second.size());
        }

        return free_bytes;
}

