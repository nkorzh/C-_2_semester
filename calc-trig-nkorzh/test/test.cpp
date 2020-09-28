#include "calc.h"

#define _USE_MATH_DEFINES

#include <cmath>

#include <gtest/gtest.h>

class CalcFixture : public ::testing::TestWithParam<bool> {
    
};

const double eps = 1e-15;

TEST_P(CalcFixture, err)
{
    auto param = GetParam();
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "fix"));
    EXPECT_EQ("Unknown operation fix\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(11, process_line(11, param, "sqrt"));
    EXPECT_EQ("Unknown operation sqrt\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(17, process_line(17, param, "\\ 11"));
    EXPECT_EQ("Unknown operation \\ 11\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "rad"));
    EXPECT_EQ("Unknown operation rad\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "CAN"));
    EXPECT_EQ("Unknown operation CAN\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "ACON"));
    EXPECT_EQ("Unknown operation ACON\n", testing::internal::GetCapturedStderr());
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "sin"));
    EXPECT_EQ("Unknown operation sin\n", testing::internal::GetCapturedStderr());
}

TEST_P(CalcFixture, set)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "0"));
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "0000"));
    EXPECT_DOUBLE_EQ(0, process_line(101, param, "0"));
    EXPECT_DOUBLE_EQ(13, process_line(0, param, "13"));
    EXPECT_DOUBLE_EQ(5, process_line(99, param, "5."));
    EXPECT_DOUBLE_EQ(0.05625, process_line(1113, param, "0.05625"));
    EXPECT_DOUBLE_EQ(1234567890.0, process_line(1, param, "1234567890"));
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(1234567890.0, process_line(1, param, "12345678900000"));
    EXPECT_EQ("Argument isn't fully parsed, suffix left: '0000'\n", testing::internal::GetCapturedStderr());
}

TEST_P(CalcFixture, add)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(7, process_line(0, param, "+7"));
    EXPECT_DOUBLE_EQ(7, process_line(5, param, "+ 2"));
    EXPECT_DOUBLE_EQ(7, process_line(5, param, "+ \t\t   2"));
    EXPECT_DOUBLE_EQ(2.34, process_line(1.5, param, "+ 0.84"));
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(1234567899.0, process_line(9, param, "+    12345678900000"));
    EXPECT_EQ("Argument isn't fully parsed, suffix left: '0000'\n", testing::internal::GetCapturedStderr());
}

TEST_P(CalcFixture, sub)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(-11, process_line(0, param, "- 11"));
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "-0"));
    EXPECT_DOUBLE_EQ(0, process_line(3, param, "-3"));
    EXPECT_DOUBLE_EQ(-3, process_line(7, param, "-10"));
    EXPECT_DOUBLE_EQ(-12344.6789, process_line(1, param, "- 12345.67890"));
}

TEST_P(CalcFixture, mul)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "* 0"));
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "*131"));
    EXPECT_DOUBLE_EQ(0, process_line(99, param, "* 0"));
    EXPECT_DOUBLE_EQ(8, process_line(2, param, "* 4"));
    EXPECT_DOUBLE_EQ(-16, process_line(-4, param, "*4"));
}

TEST_P(CalcFixture, div)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "/ 11"));
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(11, process_line(11, param, "/ 0"));
    EXPECT_EQ("Bad right argument for division: 0\n", testing::internal::GetCapturedStderr());
    EXPECT_DOUBLE_EQ(3, process_line(6, param, "/ 2"));
    EXPECT_DOUBLE_EQ(0.7, process_line(7, param, "/ 10"));
    EXPECT_DOUBLE_EQ(0.3333333333333333, process_line(1, param, "/ 3"));
    EXPECT_DOUBLE_EQ(-0.5, process_line(-2, param, "/ 4"));
    EXPECT_DOUBLE_EQ(100, process_line(10, param, "/ 0.1"));
}

TEST_P(CalcFixture, rem)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "/ 3"));
    EXPECT_DOUBLE_EQ(0, process_line(4, param, "%4"));
    EXPECT_DOUBLE_EQ(0, process_line(-24, param, "%4"));
    EXPECT_DOUBLE_EQ(2, process_line(-13, param, "%5"));
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(-1, process_line(-1, param, "%0"));
    EXPECT_EQ("Bad right argument for remainder: 0\n", testing::internal::GetCapturedStderr());
}

TEST_P(CalcFixture, neg)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "_"));
    EXPECT_DOUBLE_EQ(1, process_line(-1, param, "_"));
}

TEST_P(CalcFixture, pow)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "^1"));
    EXPECT_DOUBLE_EQ(0, process_line(0, param, "^2"));
    EXPECT_DOUBLE_EQ(1, process_line(119, param, "^0"));
    EXPECT_DOUBLE_EQ(37, process_line(37, param, "^1"));
    EXPECT_DOUBLE_EQ(25, process_line(-5, param, "^2"));
    EXPECT_DOUBLE_EQ(-27, process_line(-3, param, "^3"));
    EXPECT_DOUBLE_EQ(5, process_line(25, param, "^0.5"));
}

TEST_P(CalcFixture, sqrt)
{
    auto param = GetParam();
    EXPECT_DOUBLE_EQ(1, process_line(1, param, "SQRT"));
    EXPECT_DOUBLE_EQ(0.7, process_line(0.49, param, "SQRT"));
    EXPECT_DOUBLE_EQ(5, process_line(25, param, "SQRT"));
    testing::internal::CaptureStderr();
    EXPECT_DOUBLE_EQ(-1, process_line(-1, param, "SQRT"));
    EXPECT_EQ("Bad argument for SQRT: -1\n", testing::internal::GetCapturedStderr());
}

INSTANTIATE_TEST_SUITE_P(Calc, CalcFixture, ::testing::Values(true, false));

const double sqrt_3 = 1.7320508075688772935274463;
const double sqrt_3_over_3 = 1.7320508075688772935274463l / 3.0;
const double half_sqrt_3 = 1.7320508075688772935274463l / 2.0;
const double half_sqrt_2 = 0.707106781186547524400844;

TEST(Calc, sin)
{
    bool rad_on = false;
    EXPECT_NEAR(0.5, process_line(30, rad_on, "SIN"), eps);
    EXPECT_NEAR(1, process_line(90, rad_on, "SIN"), eps);
    EXPECT_NEAR(-1, process_line(-90, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "SIN"), eps);
    EXPECT_NEAR(-1, process_line(270, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(180, rad_on, "SIN"), eps);
    EXPECT_NEAR(-half_sqrt_2, process_line(225, rad_on, "SIN"), eps);
    EXPECT_NEAR(-0.5, process_line(330, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(360, rad_on, "SIN"), eps);
    EXPECT_NEAR(half_sqrt_3, process_line(480, rad_on, "SIN"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_NEAR(1, process_line(M_PI_2, rad_on, "SIN"), eps);
    EXPECT_NEAR(-1, process_line(-M_PI_2, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(M_PI * 2, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(M_PI * 4, rad_on, "SIN"), eps);
    EXPECT_NEAR(0.5, process_line(M_PI * 5 / 6, rad_on, "SIN"), eps);
    EXPECT_NEAR(-half_sqrt_2, process_line(M_PI * 7 / 4, rad_on, "SIN"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_NEAR(1, process_line(90, rad_on, "SIN"), eps);
    EXPECT_NEAR(-1, process_line(-90, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(180, rad_on, "SIN"), eps);
    EXPECT_NEAR(0, process_line(360, rad_on, "SIN"), eps);
}

TEST(Calc, cos)
{
    bool rad_on = false;
    EXPECT_NEAR(0, process_line(90, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(0, rad_on, "COS"), eps);
    EXPECT_NEAR(half_sqrt_3, process_line(30, rad_on, "COS"), eps);
    EXPECT_NEAR(0.5, process_line(60, rad_on, "COS"), eps);
    EXPECT_NEAR(-1, process_line(180, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(360, rad_on, "COS"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    ASSERT_TRUE(rad_on);
    EXPECT_NEAR(0, process_line(M_PI_2, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(0, rad_on, "COS"), eps);
    EXPECT_NEAR(-1, process_line(M_PI, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(M_PI * 2, rad_on, "COS"), eps);
    EXPECT_NEAR(-half_sqrt_2, process_line(M_PI * 3 / 4, rad_on, "COS"), eps);
    EXPECT_NEAR(half_sqrt_3, process_line(M_PI * 11 / 6, rad_on, "COS"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_NEAR(0, process_line(90, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(0, rad_on, "COS"), eps);
    EXPECT_NEAR(-1, process_line(180, rad_on, "COS"), eps);
    EXPECT_NEAR(1, process_line(360, rad_on, "COS"), eps);
}

TEST(Calc, tan)
{
    const double near_inf = 1.633123935319537e+16;
    bool rad_on = false;
    EXPECT_NEAR(1, process_line(45, rad_on, "TAN"), eps);
    EXPECT_NEAR(-1, process_line(-45, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "TAN"), eps);
    EXPECT_NEAR(sqrt_3_over_3, process_line(30, rad_on, "TAN"), eps);
    EXPECT_NEAR(sqrt_3, process_line(60, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(180, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(360, rad_on, "TAN"), eps);
    EXPECT_DOUBLE_EQ(near_inf, process_line(90, rad_on, "TAN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_NEAR(1, process_line(M_PI_4, rad_on, "TAN"), eps);
    EXPECT_NEAR(-1, process_line(-M_PI_4, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(M_PI, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(M_PI * 2, rad_on, "TAN"), eps);
    EXPECT_NEAR(sqrt_3_over_3, process_line(M_PI * 7 / 6, rad_on, "TAN"), eps);
    EXPECT_DOUBLE_EQ(near_inf, process_line(M_PI_2, rad_on, "TAN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_NEAR(1, process_line(45, rad_on, "TAN"), eps);
    EXPECT_NEAR(-1, process_line(-45, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(0, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(180, rad_on, "TAN"), eps);
    EXPECT_NEAR(0, process_line(360, rad_on, "TAN"), eps);
    EXPECT_DOUBLE_EQ(near_inf, process_line(90, rad_on, "TAN"));
}

TEST(Calc, ctn)
{
    bool rad_on = false;
    EXPECT_NEAR(1, process_line(45, rad_on, "CTN"), eps);
    EXPECT_NEAR(sqrt_3, process_line(30, rad_on, "CTN"), eps);
    EXPECT_TRUE(std::isinf(process_line(0, rad_on, "CTN")));
    EXPECT_NEAR(0, process_line(90, rad_on, "CTN"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_NEAR(1, process_line(M_PI_4, rad_on, "CTN"), eps);
    EXPECT_NEAR(sqrt_3, process_line(M_PI / 6, rad_on, "CTN"), eps);
    EXPECT_TRUE(std::isinf(process_line(0, rad_on, "CTN")));
    EXPECT_NEAR(0, process_line(M_PI_2, rad_on, "CTN"), eps);
    EXPECT_NEAR(1, process_line(M_PI_4 * 5, rad_on, "CTN"), eps);
    EXPECT_NEAR(-sqrt_3_over_3, process_line(M_PI * 5 / 3, rad_on, "CTN"), eps);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_NEAR(1, process_line(45, rad_on, "CTN"), eps);
    EXPECT_NEAR(sqrt_3, process_line(30, rad_on, "CTN"), eps);
    EXPECT_NEAR(sqrt_3_over_3, process_line(60, rad_on, "CTN"), eps);
    EXPECT_TRUE(std::isinf(process_line(0, rad_on, "CTN")));
    EXPECT_NEAR(0, process_line(90, rad_on, "CTN"), eps);
}

TEST(Calc, asin)
{
    bool rad_on = false;
    EXPECT_DOUBLE_EQ(-90, process_line(-1, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(-30, process_line(-0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(30, process_line(0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(90, process_line(1, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_DOUBLE_EQ(-M_PI_2, process_line(-1, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(-M_PI / 6, process_line(-0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(M_PI / 6, process_line(0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(M_PI_2, process_line(1, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_DOUBLE_EQ(-90, process_line(-1, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(-30, process_line(-0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(30, process_line(0.5, rad_on, "ASIN"));
    EXPECT_DOUBLE_EQ(90, process_line(1, rad_on, "ASIN"));
}

TEST(Calc, acos)
{
    bool rad_on = false;
    EXPECT_DOUBLE_EQ(180, process_line(-1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(0, process_line(1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(90, process_line(0, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_DOUBLE_EQ(M_PI, process_line(-1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(0, process_line(1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(M_PI_2, process_line(0, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_DOUBLE_EQ(180, process_line(-1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(0, process_line(1, rad_on, "ACOS"));
    EXPECT_DOUBLE_EQ(90, process_line(0, rad_on, "ACOS"));
}

TEST(Calc, atan)
{
    bool rad_on = false;
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ATAN"));
    EXPECT_DOUBLE_EQ(45, process_line(1, rad_on, "ATAN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ATAN"));
    EXPECT_DOUBLE_EQ(M_PI_4, process_line(1, rad_on, "ATAN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "ATAN"));
    EXPECT_DOUBLE_EQ(45, process_line(1, rad_on, "ATAN"));
}

TEST(Calc, actn)
{
    bool rad_on = false;
    EXPECT_DOUBLE_EQ(45, process_line(1, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(30, process_line(sqrt_3, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "RAD"));
    ASSERT_TRUE(rad_on);
    EXPECT_DOUBLE_EQ(M_PI_4, process_line(1, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(M_PI / 6, process_line(sqrt_3, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(M_PI / 3, process_line(sqrt_3_over_3, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(0, process_line(0, rad_on, "DEG"));
    ASSERT_FALSE(rad_on);
    EXPECT_DOUBLE_EQ(45, process_line(1, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(30, process_line(sqrt_3, rad_on, "ACTN"));
    EXPECT_DOUBLE_EQ(120, process_line(-sqrt_3_over_3, rad_on, "ACTN"));
}
