#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	char c;
	std::cin >> c;
	return result;
}
