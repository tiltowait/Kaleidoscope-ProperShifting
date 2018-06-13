# Kaleidoscope-BetterShifting

The BetterShifting plugin is a trainer to help you use your shift keys more properly. If you hold down the left shift, it disables input on the left half of the keyboard. Right shift disables the right half of the keyboard.

## Using the plugin

To use the plugin, just include the header and add it to the
list of used plugins.

```c++
#include <Kaleidoscope.h>
#include <Kaleidoscope-BetterShifting.h>

KALEIDOSCOPE_INIT_PLUGINS(BetterShifting);

void setup() {
  Kaleidoscope.setup();
}
```

## Plugin Methods

The plugin provides the `BetterShifting` object, with the following methods:

### `.enable()`

> Enable the BetterShifting plugin.
>
> The default behavior is `enabled`.

### `.disable()`

> Disable the BetterShifting plugin.

### `.isActive()`

> This method returns `true` if BetterShifting is enabled and `false` if
> BetterShifting is disabled.

## Dependencies

None.
