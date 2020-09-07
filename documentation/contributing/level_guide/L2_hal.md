# Level 2 Hardware Abstraction Layer (HAL) Guidelines

## Adding a device driver (HAL)

First, you will need to figure out which folder to put it in. Lets take the
[Sharp GP2Y0A21YK0F Analog Distance Sensor]
(http://www.sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf)
for example. For the sharp distance sensor it should be placed in the
`library/L2_HAL/sensors/distance/`. If there doesn't seem to be a proper
place for your sensor, make a new folder for it.

Second, you will need to figure out which L1 Peripherals you will need.
In the case of the Sharp distance sensor, you will need ADC.

Third, if the device has category interface file, make sure to inherit
the interface so that the device driver can be used as the interface.
For example, there may exist a `Distance1D` interface for 1-dimensional
distance sensors, and if that is the case, `SharpGP2Y0A21` should
inherit it.

Finally, you can get started with creating your new device driver. L1
Peripherals must be taken as const Object references (`const Adc & adc`)
as their parameters for their constructors. The HAL driver can take more
parameters than then just the L1 drivers.

## Adding a device category interface

If there is a device category that does not currently exist, it can be
added, but must be added in the following way. Lets take temperature as
an example:

**Category interface:** `L2_HAL/sensors/temperature.hpp`
**Implementation file:** `L2_HAL/sensors/temperature/si7060.hpp`

If the category is temperature, then there should be a temperature
folder. In this case, this is apart of the sensors category. The
implementations of the category shall reside in that folder.

The interface file should be outside of the folder of implementations.

## Testing L2

Generally, HAL drivers are tested by dependency injecting mock version
of the L1 peripherals that the driver takes. With the sharp distance
example, sharp a mock ADC would be created using the
[FakeIt](https://github.com/eranpeer/FakeIt) mocking library.

``` c++
Mock<sjsu::Adc> mock_adc;
// We only need to fake the methods that our class uses
Fake(Method(mock_adc, Initialize));
// Method mock.Read will return 512 once.
When(Method(mock_adc, Read)).Return(512);
// Pass mock ADC reference to sharp test object
const sjsu::Adc & mock_adc_reference = mock_adc.get();
SharpGP2Y0A21 test_subject(mock_adc_reference);
```

Then, for each test section, test that the appropriate methods were
called with the appropriate inputs. In the Sharp case it would be
helpful to change what the `mock_adc::Read()` returns in order to change
the behavior of the Sharp distance method. You can then check that the
returned distance values agree with the mocked return value of the ADC
function.
