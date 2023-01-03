#include "pch.h"

#include <random>

#include "../../Stack.h"

class StackTest : public ::testing::Test
{
public:
	AStack<int>		StackInt;
	AStack<float>	StackFloat;
};

static thread_local std::mt19937 generator((unsigned int)time(nullptr));
// Random number generator
int IntRand(const int& min, const int& max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}
float FloatRand(const int& min, const int& max) {
	std::uniform_int_distribution<int> distribution(min, max);
	return static_cast<float>(distribution(generator));
}

TEST_F(StackTest, test_int_zero)
{
	EXPECT_EQ(StackInt.Size(), 0);
	EXPECT_EQ(StackFloat.Size(), 0);
}

TEST_F(StackTest, test_adding_values_size)
{
	const int test_count = IntRand(1, 20);
	for (int i = 0; i < test_count; i++) {
		StackInt.Add(IntRand(1, 20));
	}
	EXPECT_EQ(StackInt.Size(), test_count);

	const int test_count2 = IntRand(1, 20);
	for (int i = 0; i < test_count2; i++) {
		StackFloat.Add(FloatRand(1, 20));
	}
	EXPECT_EQ(StackFloat.Size(), test_count2);
}

TEST_F(StackTest, test_get_values_size)
{
	const int stack_int_size = StackInt.Size();
	for (int i = 0; i < stack_int_size; i++) {
		StackInt.Get();
	}
	EXPECT_EQ(StackInt.Size(), 0);

	const int stack_float_size = StackFloat.Size();
	for (int i = 0; i < stack_float_size; i++) {
		StackFloat.Get();
	}
	EXPECT_EQ(StackFloat.Size(), 0);
}

TEST_F(StackTest, test_push_1024_values)
{
	for (int i = 0; i < 1024; i++) {
		StackInt.Add(IntRand(1, 20));
	}
	EXPECT_EQ(StackInt.Size(), 1024);

	for (int i = 0; i < 1024; i++) {
		StackFloat.Add(FloatRand(1, 20));
	}
	EXPECT_EQ((float)StackFloat.Size(), 1024);
}

TEST_F(StackTest, test_erase_stack)
{
	StackInt.Erase();
	StackFloat.Erase();

	EXPECT_EQ(StackInt.IsEmpty(), true);
	EXPECT_EQ(StackFloat.IsEmpty(), true);

	EXPECT_EQ(StackInt.Size(), 0);
	EXPECT_EQ(StackFloat.Size(), 0);
}