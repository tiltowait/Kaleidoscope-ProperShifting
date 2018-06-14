# Kaleidoscope-BetterShifting

The BetterShifting plugin is a trainer to help you use your shift keys more properly. If you hold down the left shift, it disables input on the left half of the keyboard. Right shift disables the right half of the keyboard.

Holding both shift keys behaves like a shift-lock for as long as they're both held. This allows you to type IN ALL CAPS without giving your thumbs an obnoxious workout.

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

## Known Issues

In some instances, rapidly hitting shift, then a key on the same side, then releasing both keys, will send the lower-case version of that key instead of no key at all. I suspect (but haven't verified) that this is due to a timing detail in Kaleidoscope rather than a bug with the plugin.

In either case, the bug is obscure enough that I'm not sure how to fix it without overcomplicating the plugin.