#include "rpn.hpp"
#include <iostream>
#include <gtest/gtest.h>
#include "logger.hpp"


// TODO Add more tests

TEST(expression, sum)
{
	yesza::logger::turnOff();
	yesza::logger::setPriority(yesza::logger::Priority::LOW);
	EXPECT_DOUBLE_EQ(yesza::count("5+10"), 15);
	EXPECT_DOUBLE_EQ(yesza::count("0+5"), 5);
	EXPECT_DOUBLE_EQ(yesza::count("10000+12314"), 22314);
	EXPECT_DOUBLE_EQ(yesza::count(""), 0);
	EXPECT_DOUBLE_EQ(yesza::count("5.5+ 1.4"), 6.9);
	EXPECT_DOUBLE_EQ(yesza::count("-5.5+ 1.4"), -4.1);
	EXPECT_DOUBLE_EQ(yesza::count("-5 * (7+3)"), -50);
	EXPECT_DOUBLE_EQ(yesza::count("-5 * (-7+3)"), 20);
	EXPECT_DOUBLE_EQ(yesza::count("-5 * (7-3)"), -20);
	EXPECT_DOUBLE_EQ(yesza::count("-5 * -(7-3)"), 20);
}
TEST(equation, sum)
{
	auto eq = yesza::make_equation("x+10");
	EXPECT_DOUBLE_EQ(eq(0), 10);
	EXPECT_DOUBLE_EQ(eq(-10), 0);
	EXPECT_DOUBLE_EQ(eq(5.5), 15.5);
	EXPECT_DOUBLE_EQ(eq(-10.5), -0.5);
	eq = yesza::make_equation("x*x");
	EXPECT_DOUBLE_EQ(eq(0), 0);
	EXPECT_DOUBLE_EQ(eq(5), 25);
	EXPECT_DOUBLE_EQ(eq(2), 4);
	EXPECT_DOUBLE_EQ(eq(10), 100);
	EXPECT_DOUBLE_EQ(eq(0.1), 0.01);
	EXPECT_DOUBLE_EQ(eq(-4), 16);
	EXPECT_DOUBLE_EQ(eq(-4 * 5), 400);
}
TEST(expression, min)
{
	EXPECT_DOUBLE_EQ(yesza::count("5-10"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("0-5"), -5);
	EXPECT_DOUBLE_EQ(yesza::count("10000-12314"), -2314);
	EXPECT_DOUBLE_EQ(yesza::count("-10-20"), -30);
	EXPECT_DOUBLE_EQ(yesza::count("5.5 - 1.4"), 4.1);
}
