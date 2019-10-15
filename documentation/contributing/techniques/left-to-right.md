# Left to Right Code Readability Technique

Take the following as an example:

``` c++
sjsu::lpc40xx::Gpio button_pin(0, 1);
```

Now, the above is a bit verbose, but as you can tell its pretty easy to
understand what is happening. Reading from left to right, you see that
we are using an sjsu library specifically. Then we go into the scope of
`lpc40xx` peripheral drivers. Lastly we pick the Gpio peripheral as our
object type. and finally, the name `button_pin` which tells you exactly
what this variable is being used for. Keeping the code in this form
makes it easy to tell what it is you are using.

``` c++
button_pin.GetPin().SetMode(sjsu::Pin::Mode::kPullUp);
```

Reading from left to right, we see that we are taking the button pin,
acquiring the underlining pin object from within the Gpio object, then
using that object to set the pin's mode, such as pull up, pull down, etc
etc. We can even tell with the mode name what it is we are doing. In
`sjsu::Pin::Mode::kPullUp` we see that we are using sjsu's Pin library
and within that is a list (enum) of Modes. In this case we want to use
the Pull Up mode.