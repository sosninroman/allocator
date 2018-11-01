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
}

TEST(SLIST_TEST, slist_standard_allocator_test)
{
    slist::SList<int, std::allocator<int>> list;

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
