# Kaleidoscope-BetterShifting

The BetterShifting plugin is a trainer to help you use your shift keys more properly. If you hold down the left shift, it disables input on the left half of the keyboard. Right shift disables the right half of the keyboard.

Holding both shift keys behaves like a shift-lock for as long as they're both held. This allows you to type IN ALL CAPS without giving your thumbs an obnoxious workout.

## Using the plugin

To use the plugin, just include the header and add it to the list of used plugins. If you want to include keys that ignore the shifting rules, you may define them in `ignoreKeys(int num_keys, ...)` as in the following example.

```c++
#include <Kaleidoscope.h>
#include <Kaleidoscope-BetterShifting.h>

KALEIDOSCOPE_INIT_PLUGINS(BetterShifting);

void setup() {
  Kaleidoscope.setup();
  BetterShifting.ignoreKeys(2, Key_A.raw, Key_B.raw);
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

### `.ignoreKeys(int num_keys, ...)`

> This method takes a variable number of arguments which will be used to
> build a list of keys for which the shift rules will not apply.
>
> `num_keys` is the number of keys in the list. The rest of the arguments
> must be of type `uint16_t`, as obtained by calling the `.raw` member of
> a Key object.

### `.clearIgnoredKeys()`

> Empties the current list of ignored keys, if one exists.

## Dependencies

None.