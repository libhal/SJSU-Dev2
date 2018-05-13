#include <stdint.h>

#include "SampleTestSubject.hpp"

int32_t SampleTestSubject::methodAdd(int32_t a, int32_t b) const
{
    return a + b;
}
void SampleTestSubject::injectAddFunction(int32_t (*new_function)(int32_t, int32_t))
{
    alternative_add_function = new_function;
}
int32_t SampleTestSubject::useInjectedMethod(int32_t a, int32_t b)
{
    return alternative_add_function(a, b);
}