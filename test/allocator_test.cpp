#include <gtest/gtest.h>
#include "allocator.h"
#include "testallocatoraccessor.h"
#include <string>
#include <map>

class TestValueType
{
public:
    TestValueType(int v, std::string s):m_number(v >= 0 ? v : 0), m_text(s){}
    ~TestValueType()
    {
        m_number = -1;
        m_text.clear();
    }

    int number() const {return m_number;}
    std::string text() const {return m_text;}

private:
    int m_number;
    std::string m_text;
};

bool operator ==(const TestValueType& lhs, const TestValueType& rhs)
{
    return lhs.number() == rhs.number() && lhs.text() == rhs.text();
}

TEST(ALLOCATOR_TEST, allocate1)
{
    allocator::Allocator<int, 10> alloc;
    bool isNull = TestAllocatorAccessor<int,10>::isNull(&alloc);
    ASSERT_TRUE(isNull);

    ASSERT_NO_THROW(alloc.allocate(10) );

    isNull = TestAllocatorAccessor<int,10>::isNull(&alloc);
    ASSERT_FALSE(isNull);

    auto states = TestAllocatorAccessor<int,10>::getMemoryMap(&alloc);
    for(int i = 0; i < 10; ++i)
        ASSERT_EQ(states[i], allocator::ALLOCATE_STATE);
}

TEST(ALLOCATOR_TEST, allocate2)
{
    allocator::Allocator<int, 10> alloc;
    ASSERT_THROW(alloc.allocate(11), std::bad_alloc);
}

TEST(ALLOCATOR_TEST, allocate3)
{
    allocator::Allocator<int, 10> alloc;

    ASSERT_NO_THROW(alloc.allocate(2) );
    ASSERT_NO_THROW(alloc.allocate(4) );
    ASSERT_THROW(alloc.allocate(8), std::bad_alloc);
}

TEST(ALLOCATOR_TEST, deallocate_test)
{
    allocator::Allocator<int, 10> alloc;

    int* mem = alloc.allocate(10);

    auto states = TestAllocatorAccessor<int,10>::getMemoryMap(&alloc);
    for(int i = 0; i < 10; ++i)
        ASSERT_EQ(states[i], allocator::ALLOCATE_STATE);

    alloc.deallocate(mem, 10);

    states = TestAllocatorAccessor<int,10>::getMemoryMap(&alloc);
    for(int i = 0; i < 10; ++i)
        ASSERT_EQ(states[i], allocator::DEALLOCATE_STATE);
}

TEST(ALLOCATOR_TEST, allocate_deallocate_test)
{
    allocator::Allocator<int, 5> alloc;

    int* mem1 = alloc.allocate(2);
    int* mem2 = alloc.allocate(1);
    int* mem3 = alloc.allocate(1);
    int* mem4 = alloc.allocate(1);

    auto states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    for(int i = 0; i < 4; ++i)
        ASSERT_EQ(states[i], allocator::ALLOCATE_STATE);

    alloc.deallocate(mem2, 1);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[1], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[2], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[3], allocator::ALLOCATE_STATE);//mem3
    ASSERT_EQ(states[4], allocator::ALLOCATE_STATE);//mem4

    alloc.deallocate(mem4, 1);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[1], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[2], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[3], allocator::ALLOCATE_STATE);//mem3
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    int* mem5 = alloc.allocate(1);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[1], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[2], allocator::ALLOCATE_STATE);//mem5
    ASSERT_EQ(states[3], allocator::ALLOCATE_STATE);//mem3
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    alloc.deallocate(mem1, 2);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[1], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[2], allocator::ALLOCATE_STATE);//mem5
    ASSERT_EQ(states[3], allocator::ALLOCATE_STATE);//mem3
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    alloc.deallocate(mem3, 1);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[1], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[2], allocator::ALLOCATE_STATE);//mem5
    ASSERT_EQ(states[3], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    alloc.deallocate(mem5, 1);

    states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[1], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[2], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[3], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    auto isnull = TestAllocatorAccessor<int, 5>::isNull(&alloc);
    ASSERT_FALSE(isnull);
}

TEST(ALLOCATOR_TEST, construct_standard_type_test)
{
    allocator::Allocator<int, 5> alloc;

    int* mem1 = alloc.allocate(2);
    int* mem2 = alloc.allocate(1);

    auto states = TestAllocatorAccessor<int, 5>::getMemoryMap(&alloc);
    ASSERT_EQ(states[0], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[1], allocator::ALLOCATE_STATE);//mem1
    ASSERT_EQ(states[2], allocator::ALLOCATE_STATE);//mem2
    ASSERT_EQ(states[3], allocator::DEALLOCATE_STATE);
    ASSERT_EQ(states[4], allocator::DEALLOCATE_STATE);

    alloc.construct(mem1, 5);
    alloc.construct(mem1+1, 2);
    alloc.construct(mem2, 38);

    auto v = TestAllocatorAccessor<int,5>::getValue(&alloc, 0);
    ASSERT_EQ(v, 5);
    v = TestAllocatorAccessor<int,5>::getValue(&alloc, 1);
    ASSERT_EQ(v, 2);
    v = TestAllocatorAccessor<int,5>::getValue(&alloc, 2);
    ASSERT_EQ(v, 38);
}

TEST(ALLOCATOR_TEST, construct_custom_type_test)
{
    allocator::Allocator<TestValueType, 3> alloc;

    TestValueType* ptr = alloc.allocate(3);
    alloc.construct(ptr, 1, "v1");
    alloc.construct(ptr+1, 2, "v2");
    alloc.construct(ptr+2, 3, "v3");

    auto v1 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 0);
    ASSERT_EQ(v1, TestValueType(1, "v1") );
    auto v2 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 1);
    ASSERT_EQ(v2, TestValueType(2, "v2") );
    auto v3 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 2);
    ASSERT_EQ(v3, TestValueType(3, "v3") );
}

TEST(ALLOCATOR_TEST, construct_destruct_test)
{
    allocator::Allocator<TestValueType, 3> alloc;

    TestValueType* ptr = alloc.allocate(3);
    alloc.construct(ptr, 1, "v1");
    alloc.construct(ptr+1, 2, "v2");
    alloc.construct(ptr+2, 3, "v3");

    alloc.destroy(ptr+1);

    auto v1 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 0);
    ASSERT_EQ(v1, TestValueType(1, "v1") );
    auto v2 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 1);
    ASSERT_EQ(v2.number(), -1);
    ASSERT_EQ(v2.text(), "");
    auto v3 = TestAllocatorAccessor<TestValueType, 3>::getValue(&alloc, 2);
    ASSERT_EQ(v3, TestValueType(3, "v3") );

    auto memMap = TestAllocatorAccessor<TestValueType, 3>::getMemoryMap(&alloc);
    for(int i = 0; i < 3; ++i)
        memMap[i] = allocator::ALLOCATE_STATE;

}

TEST(ALLOCATOR_TEST, vector_test)
{
    std::vector<int, allocator::Allocator<int, 7>> values;

    ASSERT_NO_THROW(values.push_back(0) );
    ASSERT_NO_THROW(values.push_back(1) );
    ASSERT_NO_THROW(values.push_back(2) );
    ASSERT_NO_THROW(values.push_back(3) );

    ASSERT_EQ(values.size(), 4);
    for(int i = 0; i < values.size(); ++i)
        ASSERT_EQ(values[i], i);

    ASSERT_THROW(values.push_back(4), std::bad_alloc);
}

TEST(ALLOCATOR_TEST, map_test)
{
    std::map<int, int, std::less<int>, allocator::Allocator<std::pair<const int, int>, 3>> values;
    ASSERT_NO_THROW(values.emplace(1,1) );
    ASSERT_NO_THROW(values.emplace(2,2) );
    ASSERT_NO_THROW(values.emplace(3,3) );
    ASSERT_THROW(values.emplace(4,4), std::bad_alloc);
}
