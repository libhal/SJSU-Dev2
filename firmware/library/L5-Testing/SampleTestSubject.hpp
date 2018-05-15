#ifndef __SAMPLETESTSUBJECT_HPP_
#define __SAMPLETESTSUBJECT_HPP_

#include <stdint.h>

class SampleTestSubjectInterface
{
public:
    //// Simple test of method implementation
    //// Used for spying on this
    virtual int32_t methodAdd(int32_t a, int32_t b) const = 0;
    virtual int32_t outerMethodAdd(int32_t a, int32_t b) const = 0;
    //// Simple test of function injection
    virtual void injectAddFunction(int32_t (*new_function)(int32_t, int32_t)) = 0;
    virtual int32_t useInjectedMethod(int32_t a, int32_t b) = 0;
    virtual ~SampleTestSubjectInterface() = default;
};

class SampleTestSubject : public SampleTestSubjectInterface
{
protected:
    int32_t (*alternative_add_function)(int32_t, int32_t);
public:
    int32_t methodAdd(int32_t a, int32_t b) const override;
    int32_t outerMethodAdd(int32_t a, int32_t b) const override;
    void injectAddFunction(int32_t (*new_function)(int32_t, int32_t)) override;
    int32_t useInjectedMethod(int32_t a, int32_t b) override;
};

#endif