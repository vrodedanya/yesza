#include "rpn.hpp"
#include <iostream>
#include <gtest/gtest.h>

// TODO Add more tests

TEST(expression, sum)
{
	EXPECT_DOUBLE_EQ(yesza::count("5+10"), 15);
	EXPECT_DOUBLE_EQ(yesza::count("0+5"), 5);
	EXPECT_DOUBLE_EQ(yesza::count("10000+12314"), 22314);
	EXPECT_DOUBLE_EQ(yesza::count(""), 0);
	EXPECT_DOUBLE_EQ(yesza::count("5.5+ 1.4"), 6.9);
	EXPECT_DOUBLE_EQ(yesza::count("-5.5+ 1.4"), -4.1);
}
TEST(equation, sum)
{
	auto eq = yesza::make_equation("x+10");
	std::cout << "\t\t"<<eq(0) << std::endl;
	std::cout << "\t\t"<<eq(-10) << std::endl;
	std::cout << "\t\t"<<eq(5.5) << std::endl;
	std::cout << "\t\t"<<eq(-10.5) << std::endl;
	EXPECT_DOUBLE_EQ(eq(0), 10);
	EXPECT_DOUBLE_EQ(eq(-10), 0);
	EXPECT_DOUBLE_EQ(eq(5.5), 15.5);
	EXPECT_DOUBLE_EQ(eq(-10.5), -0.5);
}
TEST(expression, min)
{
	EXPECT_DOUBLE_EQ(yesza::count("5-10"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("0-5"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("10000-12314"), -2314);
	EXPECT_DOUBLE_EQ(yesza::count("-10-20"), -30);
	EXPECT_DOUBLE_EQ(yesza::count("5.5 - 1.4"), 4.1);
}
