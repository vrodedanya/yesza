#include "rpn.hpp"
#include <iostream>
#include <gtest/gtest.h>

// TODO Add more tests

TEST(expression, baseOperators)
{
	EXPECT_DOUBLE_EQ(yesza::count("5+10"), 15);
	EXPECT_DOUBLE_EQ(yesza::count("5-10"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("5*10"), 50);
	EXPECT_DOUBLE_EQ(yesza::count("5/10"), 0.5);
	EXPECT_DOUBLE_EQ(yesza::count("2^5"), 32);
	EXPECT_DOUBLE_EQ(yesza::count("2^10"), 1024);

	EXPECT_THROW(yesza::count("5/0"), std::runtime_error);

	EXPECT_DOUBLE_EQ(yesza::count("-5+5"), 0);
	EXPECT_DOUBLE_EQ(yesza::count("-5.5+5"), -0.5);
	EXPECT_DOUBLE_EQ(yesza::count("-5.5+(-5)"), -10.5);
	EXPECT_THROW(yesza::count(""), std::runtime_error);
	EXPECT_DOUBLE_EQ(yesza::count("5.5+ 1.4"), 6.9);
	EXPECT_DOUBLE_EQ(yesza::count("-5.5+ 1.4"), -4.1);
	EXPECT_DOUBLE_EQ(yesza::count("-5 + (7+3)"), 5);
	EXPECT_DOUBLE_EQ(yesza::count("-5 + (-7+3)"), -9);
	EXPECT_DOUBLE_EQ(yesza::count("-5 + (7-3)"), -1);
	EXPECT_DOUBLE_EQ(yesza::count("-5 + -(7-3)"), -9);
	EXPECT_DOUBLE_EQ(yesza::count("5-10"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("0-5"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("10000-12314"), -2314);
	EXPECT_DOUBLE_EQ(yesza::count("-10-20"), -30);
	EXPECT_DOUBLE_EQ(yesza::count("5.5 - 1.4"), 4.1);
	EXPECT_DOUBLE_EQ(yesza::count("2^10"), 1024);
	EXPECT_DOUBLE_EQ(yesza::count("2^10"), 10);

}
TEST(equation, sum)
{
	auto eq = yesza::make_equation("x+10");
	EXPECT_DOUBLE_EQ(eq(0), 10);
	EXPECT_DOUBLE_EQ(eq(-10), 0);
	EXPECT_DOUBLE_EQ(eq(5.5), 15.5);
	EXPECT_DOUBLE_EQ(eq(-10.5), -0.5);
	EXPECT_DOUBLE_EQ(eq(10), 20);
	eq = yesza::make_equation("x*x");
	EXPECT_DOUBLE_EQ(eq(0), 0);
	EXPECT_DOUBLE_EQ(eq(5), 25);
	EXPECT_DOUBLE_EQ(eq(2), 4);
	EXPECT_DOUBLE_EQ(eq(10), 100);
	EXPECT_DOUBLE_EQ(eq(0.1), 0.01);
	EXPECT_DOUBLE_EQ(eq(-4), 16);
	EXPECT_DOUBLE_EQ(eq(-4 * 5), 400);
	
	eq = yesza::make_equation("2*x^2+2*x+4");
	EXPECT_DOUBLE_EQ(eq(2), 16);
	EXPECT_DOUBLE_EQ(eq(2.4), 20.32);
	EXPECT_DOUBLE_EQ(eq(1.5), 11.5);
	EXPECT_DOUBLE_EQ(eq(4), 44);
	EXPECT_DOUBLE_EQ(eq(5), 64);
	EXPECT_DOUBLE_EQ(eq(6), 88);
	EXPECT_DOUBLE_EQ(eq(34), 2384);
	EXPECT_DOUBLE_EQ(eq(56), 6388);

}
TEST(expression, sub)
{
	EXPECT_DOUBLE_EQ(yesza::count("(-(-(5+5)))"),10);
	EXPECT_DOUBLE_EQ(yesza::count("(-(-(5-5+5)+5-5))"), 5);
}
