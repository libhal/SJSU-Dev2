#include "L2_Utilities/utility.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Random Functions", "[random]")
{
    SECTION("SetRandomSeed Consistency")
    {
        utility::SetRandomSeed();
        int32_t randomtester1 = std::rand() % 10000;
        utility::SetRandomSeed();
        int32_t randomtester2 = std::rand() % 10000;
        CHECK(randomtester1 == randomtester2);
        randomtester2         = std::rand() % 10000;
        int32_t randomtester3 = std::rand() % 10000;
        CHECK(randomtester1 != randomtester2);
        CHECK(randomtester2 != randomtester3);
    }

    SECTION("SetRandomSeed Non-default seed")
    {
        utility::SetRandomSeed();
        int32_t defaulttester[10000] = { 0 };
        int32_t newseedtester[10000] = { 0 };
        uint32_t count               = 0;
        for (int i = 0; i < 10000; i++)
        {
            defaulttester[i] = std::rand() % 10000;
        }
        utility::SetRandomSeed(0xFFFF0000);
        for (int i = 0; i < 10000; i++)
        {
            newseedtester[i] = std::rand() % 10000;
        }
        for (int i = 0; i < 10000; i++)
        {
            if (defaulttester[i] == newseedtester[i])
            {
                count++;
            }
        }
        CHECK(10000 > count);
    }

    SECTION("Random Bounds")
    {
        utility::SetRandomSeed();
        bool norandomoob = true;
        for (uint32_t i = 0; i < 1000; i++)
        {
            int32_t randomtester = utility::Random(0b1, 0b1111);
            if (0b1 > randomtester || 0b1111 < randomtester)
            {
                norandomoob = false;
                break;
            }
        }
        CHECK(true == norandomoob);
    }

    SECTION("Random")
    {
        utility::SetRandomSeed();
        int32_t randomtester1 = std::rand() % 100;
        utility::SetRandomSeed();
        int32_t randomtester2 = utility::Random(0, 99);
        CHECK(randomtester1 == randomtester2);
    }
}

TEST_CASE("Testing Bit Manipulations", "[bit manipulation]")
{
    SECTION("BitExtract")
    {
        CHECK(0b111 == utility::BitExtract(0b1111, 2, 3));
        CHECK(0b111 == utility::BitExtract(0b1111, 3, 3));
        CHECK(0b1111 == utility::BitExtract(0b1111, 31, 32));
        CHECK(0b1 == utility::BitExtract(0b1111, 0, 1));
        CHECK(0b0 == utility::BitExtract(0b1111, 31, 28));
        CHECK(0xFFFF == utility::BitExtract(0xFFFFFFFF, 15, 16));
        CHECK(0xFFFF == utility::BitExtract(0xFFFFFFFF, 31, 16));
    }

    SECTION("BitPlace")
    {
        CHECK(0b1111 == utility::BitPlace(0b1111, 2, 7, 3));
        CHECK(0b11111 == utility::BitPlace(0b1111, 4, true, 1));
        CHECK(0b111 == utility::BitPlace(0b1111, 3, false, 1));
        CHECK(0xF0F0F0F == utility::BitPlace(0xF0F0F0F0, 31, 0x0F0F0F0F, 32));
    }

    SECTION("BitSet")
    {
        CHECK(0b11111 == utility::BitSet(0b1111, 4));
        CHECK(0xFFFFFFFF == utility::BitSet(0x7FFFFFFF, 31));
        CHECK(0b1 == utility::BitSet(0, 0));
        CHECK(0XFFFFFFFF == utility::BitSet(0xF0F0F0F0, 31, 32));
    }

    SECTION("BitClear")
    {
        CHECK(0b111 == utility::BitClear(0b1111, 3));
        CHECK(0xFFFFFFFE == utility::BitClear(0xFFFFFFFF, 0));
        CHECK(0x7FFFFFFF == utility::BitClear(0XFFFFFFFF, 31));
        CHECK(0xF0F00000 == utility::BitClear(0xF0F0F0F0, 15, 16));
    }

    SECTION("BitRead")
    {
        CHECK(true == utility::BitRead(0b1111, 3));
        CHECK(false == utility::BitRead(0b01111, 4));
        CHECK(true == utility::BitRead(0b1, 0));
        CHECK(false == utility::BitRead(0b0, 0));
        CHECK(true == utility::BitRead(0x80000000, 31));
        CHECK(false == utility::BitRead(0x0000, 31));
    }
}

TEST_CASE("Testing Min and Max methods", "[min-max]")
{
    SECTION("Min")
    {
        CHECK(0b0 == utility::Min<uint32_t>(0xFFFFFFFF, 0));
        CHECK(0xFFFFFFFF == utility::Min<uint32_t>(0xFFFFFFFF, 0xFFFFFFFF));
    }

    SECTION("Max")
    {
        CHECK(0xFFFFFFFF == utility::Max<uint32_t>(0xFFFFFFFF, 0));
        CHECK(0 == utility::Max<uint32_t>(0, 0));
    }
}

TEST_CASE("Testing Map method", "[map]")
{
    SECTION("Map")
    {
        CHECK(1 == utility::Map(1, 1, 4, 1, 256));
        CHECK(86 == utility::Map(2, 1, 4, 1, 256));
        CHECK(1 == utility::Map(85, 1, 256, 1, 4));
        CHECK(2 == utility::Map(86, 1, 256, 1, 4));
        CHECK(4 == utility::Map(1, 1, 4, 4, 1));
        CHECK(1 == utility::Map(4, 1, 4, 4, 1));

        CHECK(0.0000000000001 >
              static_cast<float>(2.5) - utility::Map<float>(5, 0, 10, 0, 5));
        CHECK(1 == utility::Map<uint32_t>(4, 1, 4, 4, 1));
        CHECK(-12 == utility::Map<int>(5, 0, 10, 0, -24));
    }
}

TEST_CASE("Testing FloatCompare method", "[float-compare]")
{
	SECTION("FloatCompare")
	{
		CHECK(true == utility::FloatCompare<double>(0.01, 0.01));
		CHECK(false == utility::FloatCompare<double>(3, 3.1));
	}
}

