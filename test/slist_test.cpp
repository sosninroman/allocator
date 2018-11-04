#include <gtest/gtest.h>
#include "slist.h"
#include "allocator.h"
#include <memory>

TEST(SLIST_TEST, slist_custom_allocator_test)
{
    slist::SList<int, allocator::Allocator<int,3>> list;

    ASSERT_TRUE(list.isEmpty() );
    ASSERT_NO_THROW(list.addItem(0) );
    ASSERT_NO_THROW(list.addItem(1) );
    ASSERT_NO_THROW(list.addItem(2) );

    auto itr = list.begin();
    for(int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(*itr++, i);
    }

    ASSERT_THROW(list.addItem(6), std::bad_alloc);

    slist::SList<int, allocator::Allocator<int, 3, true>> list1;

    ASSERT_TRUE(list1.isEmpty() );
    ASSERT_NO_THROW(list1.addItem(0) );
    ASSERT_NO_THROW(list1.addItem(1) );
    ASSERT_NO_THROW(list1.addItem(2) );

    itr = list1.begin();
    for(int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(*itr++, i);
    }

    ASSERT_NO_THROW(list1.addItem(6) );
}

TEST(SLIST_TEST, slist_standard_allocator_test)
{
    slist::SList<int> list;

    ASSERT_TRUE(list.isEmpty() );
    ASSERT_NO_THROW(list.addItem(0) );
    ASSERT_NO_THROW(list.addItem(1) );
    ASSERT_NO_THROW(list.addItem(2) );

    auto itr = list.begin();
    for(int i = 0; i < 3; ++i)
    {
        ASSERT_EQ(*itr++, i);
    }

    ASSERT_NO_THROW(list.addItem(6) );
}
