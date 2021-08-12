#include "rpn.hpp"
#include <iostream>
#include <gtest/gtest.h>

TEST(expression, sum)
{
	ASSERT_DOUBLE_EQ(yezha::count("5+10"), 15);
	ASSERT_DOUBLE_EQ(yezha::count("0+5"), 5);
	ASSERT_DOUBLE_EQ(yezha::count("10000+12314"), 22314);
	ASSERT_DOUBLE_EQ(yezha::count(""), 0);
	ASSERT_DOUBLE_EQ(yezha::count("5.5+ 1.4"), 6.9);
	ASSERT_DOUBLE_EQ(yezha::count("-5.5+ 1.4"), -4.1);
}
TEST(expression, min)
{
	ASSERT_DOUBLE_EQ(yezha::count("5-10"), -5);
	ASSERT_DOUBLE_EQ(yezha::count("0-5"), -5);
	ASSERT_DOUBLE_EQ(yezha::count("10000-12314"), -2314);
	ASSERT_DOUBLE_EQ(yezha::count("-10-20"), -30);
	ASSERT_DOUBLE_EQ(yezha::count("5.5 - 1.4"), 4.1);
}
