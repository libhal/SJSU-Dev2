#define CATCH_CONFIG_MAIN

#include <stdint.h>

#include "third-party/Testing/catch.hpp"
#include "third-party/Testing/fff.h"
#include "third-party/Testing/fakeit.hpp"
#include "L5-Testing/SampleTestSubject.hpp"

DEFINE_FFF_GLOBALS

using namespace fakeit;

FAKE_VALUE_FUNC(int32_t, test_add, int32_t, int32_t);

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
        //// Defining custom function definition
        test_add_fake.custom_fake = [](int32_t a, int32_t b) -> int32_t
        {
            return a - b;
        };
        //// Inject function callback into test subject
		subject.injectAddFunction(test_add);
        //// Use test subject 'useInjectedMethod' to call spied function
        REQUIRE( subject.useInjectedMethod(17, 7) == 10 );
        //// Redefining custom function spy
        test_add_fake.custom_fake = [](int32_t a, int32_t b) -> int32_t
        {
            return a * b;
        };
        REQUIRE( subject.useInjectedMethod(8, 4) == 32 );
        REQUIRE( test_add_fake.call_count == 2 );
	}
    SECTION( "Basic Object method spying" )
	{
        SampleTestSubject subject;

        Mock<SampleTestSubject> spy(subject);
        Spy(Method(spy, methodAdd));
        Spy(Method(spy, outerMethodAdd));
        Spy(Method(spy, useInjectedMethod));

        SampleTestSubject & spy_reference = spy.get();

        REQUIRE( spy_reference.outerMethodAdd(1000, 234) == 1234 );

        Verify(Method(spy, methodAdd)).Exactly(1);
        Verify(Method(spy, outerMethodAdd)).Exactly(1);
        Verify(Method(spy, useInjectedMethod)).Exactly(0);
	}
}