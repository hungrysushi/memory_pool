
#include "memory_pool.h"

#include <gtest/gtest.h>

namespace {

const unsigned int kPoolSize = 1024;

class MemoryPoolTest : public ::testing::Test {
public:
        MemoryPoolTest()
                : memory_pool_(kPoolSize) {}
        virtual ~MemoryPoolTest() {}

        virtual void SetUp() {

        }

        virtual void TearDown() {

        }

        MemoryPool memory_pool_;
};

class AllocObject {
public:
        // constructor is not called
        AllocObject() {
                member_ = 20;
        }

        ~AllocObject();

        void Initialize(int set_member) {
                member_ = set_member;
        }

        // member_ is not set on allocation
        int member_ = 0;
};

TEST_F(MemoryPoolTest, create_pool) {
        EXPECT_EQ(memory_pool_.size(), kPoolSize);
}

TEST_F(MemoryPoolTest, free_size) {
        EXPECT_EQ(memory_pool_.free(), kPoolSize);
}

TEST_F(MemoryPoolTest, single_allocate_deallocate) {
        // allocate
        long* num = (long*) memory_pool_.allocate(sizeof(long));
        EXPECT_EQ(memory_pool_.free(), kPoolSize - sizeof(long));

        // deallocate
        memory_pool_.deallocate(num);
        EXPECT_EQ(memory_pool_.free(), kPoolSize - sizeof(long));
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize);
}

TEST_F(MemoryPoolTest, multiple_allocate_deallocate) {
        // allocate 128 longs
        long* num[kPoolSize / sizeof(long)];
        for (int i = 0; i < kPoolSize / sizeof(long); i++) {
                num[i] = (long*) memory_pool_.allocate(sizeof(long));
                *num[i] = i;
        }
        EXPECT_EQ(memory_pool_.free(), 0);

        // verify written values and then deallocate
        for (int i = 0; i < kPoolSize / sizeof(long); i++) {
                EXPECT_EQ(*num[i], i);
                memory_pool_.deallocate(num[i]);
        }
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize);

        // reallocate chunks
        for (int i = 0; i < kPoolSize / sizeof(long); i++) {
                num[i] = (long*) memory_pool_.allocate(sizeof(long));
                *num[i] = 2 * i;
                EXPECT_EQ(memory_pool_.free_total(), kPoolSize - ((i + 1) * sizeof(long)));
        }

        // verify values
        for (int i = 0; i < kPoolSize / sizeof(long); i++) {
                EXPECT_EQ(*num[i], i * 2);
                memory_pool_.deallocate(num[i]);
        }
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize);
}

TEST_F(MemoryPoolTest, varied_size_allocate_deallocate) {
        // 128 longs, 256 ints
        int num_longs = (kPoolSize / 2) / sizeof(long);
        int num_ints = (kPoolSize / 2) / sizeof(int);

        long* num_long[num_longs];
        int* num_int[num_ints];

        for (int i = 0; i < (kPoolSize / 2) / sizeof(long); i++) {
                num_long[i] = (long*) memory_pool_.allocate(sizeof(long));
                *num_long[i] = i;

                num_int[i] = (int*) memory_pool_.allocate(sizeof(int));
                *num_int[i] = i;
                num_int[i + (num_ints / 2)] = (int*) memory_pool_.allocate(sizeof(int));
                *num_int[i + (num_ints / 2)] = i + (num_ints / 2);
        }
        EXPECT_EQ(memory_pool_.free_total(), 0);

        // verify and deallocate
        for (int i = 0; i < (kPoolSize / 2) / sizeof(long); i++) {
                EXPECT_EQ(*num_long[i], i);
                memory_pool_.deallocate(num_long[i]);

                EXPECT_EQ(*num_int[i], i);
                EXPECT_EQ(*num_int[i + (num_ints / 2)], i + (num_ints / 2));
                memory_pool_.deallocate(num_int[i]);
                memory_pool_.deallocate(num_int[i + (num_ints / 2)]);
        }
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize);
}

TEST_F(MemoryPoolTest, class_allocate_deallocate) {
        AllocObject* object = (AllocObject*) memory_pool_.allocate(sizeof(AllocObject));
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize - sizeof(AllocObject));

        object->Initialize(10);
        EXPECT_EQ(object->member_, 10);

        object->Initialize(100);
        EXPECT_EQ(object->member_, 100);

        // free up object
        memory_pool_.deallocate(object);
        EXPECT_EQ(memory_pool_.free_total(), kPoolSize);
}

TEST_F(MemoryPoolTest, clear_all) {
        long* num[kPoolSize / sizeof(long)];
        for (int i = 0; i < kPoolSize / sizeof(long); i++) {
                num[i] = (long*) memory_pool_.allocate(sizeof(long));
                EXPECT_EQ(memory_pool_.free(), kPoolSize - ((i + 1) * sizeof(long)));

                memory_pool_.deallocate(num[i]);
                EXPECT_EQ(memory_pool_.free(), kPoolSize - ((i + 1) * sizeof(long)));
                EXPECT_EQ(memory_pool_.free_total(), kPoolSize);
        }

        memory_pool_.clear();
        EXPECT_EQ(memory_pool_.free(), kPoolSize);
        EXPECT_EQ(memory_pool_.free_total, kPoolSize);
}

TEST_F(MemoryPoolTest, consolidate_free_memory) {
        // TODO
}

}
