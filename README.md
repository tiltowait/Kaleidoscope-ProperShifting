# Kaleidoscope-ProperShifting

A plugin for the Keyboardio Model 01 to ensure proper shifting technique.

## Purpose

Force the user to use both hands while shifting. Typing a capital *A*, for instance, requires using the right shift. Capital *M*, meanwhile, necessitates the left shift.

## Motivation

Proper typing technique helps to reduce RSI and other discomforts while spending many hours at the keyboard.

Holding both shift keys behaves like a shift-lock for as long as they're both held. This allows you to type IN ALL CAPS without giving your thumbs an obnoxious workout.

## Extra Behavior

The plugin ignores the shifting rules if other modifiers (Control, Alt, or GUI) are also being held. Additionally, the user may enter a pseudo-shiftlock mode by holding both shifts.

## Using the plugin

To use the plugin, just include the header and add it to the list of used plugins. If you want to include keys that ignore the shifting rules, you may define them in `ignoreKeys(int num_keys, ...)` as in the following example.

```c++
#include <Kaleidoscope.h>
#include <Kaleidoscope-ProperShifting.h>

KALEIDOSCOPE_INIT_PLUGINS(ProperShifting);

void setup() {
  Kaleidoscope.setup();
  ProperShifting.ignoreKeys(2, Key_A.raw, Key_B.raw);
}
```

### Plugin Methods

The plugin provides the `ProperShifting` object, with the following methods:

#### `.enable()`

> Enable the ProperShifting plugin.
>
> The default behavior is `enabled`.

#### `.disable()`

> Disable the ProperShifting plugin.

#### `.isActive()`

> This method returns `true` if ProperShifting is enabled and `false` if
> ProperShifting is disabled.

## Dependencies

Nothing beyond the Kaleidoscope core.
