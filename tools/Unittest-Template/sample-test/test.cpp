#define CATCH_CONFIG_MAIN

#include <stdint.h>

#include "L5-Testing/catch.hpp"
#include "L5-Testing/fff.h"
#include "L5-Testing/fakeit.hpp"
#include "L5-Testing/SampleTestSubject.hpp"

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(int32_t, test_add, int32_t, int32_t);

int32_t sum(int32_t a, int32_t b)
{
	return a + b;
}

TEST_CASE( "Testing Unittest", "[test]" )
{
    SECTION( "Basic object method testing" )
	{
        SampleTestSubject subject;
		REQUIRE( subject.methodAdd(7, 1) == 8 );
	}

    SECTION( "Basic function spy testing" )
	{
        SampleTestSubject subject;
        test_add_Fake.custom_fake = [](int32_t a, int32_t b) -> int32_t {
            return a - b;
        };
		subject.injectAddFunction(test_add);
        REQUIRE( subject.useInjectedMethod(17, 7) == 10 );
        REQUIRE( test_add_Fake.call_count == 1 );
	}
    SECTION( "Summation test 0" )
	{
        SampleTestSubject subject;

        Mock<SampleTestSubject> spy(subject);
        Spy(Method(spy, methodAdd));

        SomeClass& spy_reference = spy.get();

        spy_reference.injectAddFunction(sum);

        Verify(Method(spy, injectAddFunction)).Exactly(1);
	}
}