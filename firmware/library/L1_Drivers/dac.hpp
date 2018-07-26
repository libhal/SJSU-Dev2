#pragma once

#include <cstdint>
#include <cstdio>

class DacInterface
{
 public:
    virtual bool Initialize();
    virtual bool WriteDac(uint16_t dac_output);
    virtual bool SetVoltage(float voltage);
    // Sine Demonstration Function
    virtual void SineDac(uint32_t frequency_hz);
    // Sawtooth Demonstration Function
    virtual void SawtoothDac(uint32_t frequency_hz);
    // Triangle Demonstration Function
    virtual void TriangleDac(uint32_t frequency_hz);
    virtual void SetBias(bool bias_level);
};
