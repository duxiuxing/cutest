#include "mock-turtle.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Return;

// 监测调用次数，指定返回值
TEST(TurtleTest, GetX) {
  MockTurtle turtle;
  EXPECT_CALL(turtle, GetX())
      .Times(5) // Times必须在WillOnce之前
      .WillOnce(Return(100))
      .WillOnce(Return(150))
      .WillRepeatedly(Return(200));

  EXPECT_EQ(turtle.GetX(), 100);
  EXPECT_EQ(turtle.GetX(), 150);
  EXPECT_EQ(turtle.GetX(), 200);
  EXPECT_EQ(turtle.GetX(), 200);
  EXPECT_EQ(turtle.GetX(), 200);
}

using ::testing::Ge;

// 监测入参
TEST(TurtleTest, Forward) {
  {
    MockTurtle turtle;
    EXPECT_CALL(turtle, Forward(100));

    turtle.Forward(100); // 如果入参不是100会报错
  }

  {
    MockTurtle turtle;
    EXPECT_CALL(turtle, Forward(100));
    EXPECT_CALL(turtle, Forward(200));

    // 如果入参不吻合，顺序不对会报错
    turtle.Forward(100);
    turtle.Forward(200);
  }

  {
    // 使用testing::Ge来监测入参
    MockTurtle turtle;
    EXPECT_CALL(turtle, Forward(Ge(100)));

    turtle.Forward(200);
  }
}
