/* -*- mode: c++ -*-
 * Kaleidoscope-ProperShifting -- Force proper shift usage
 * Copyright (C) 2018 Jared Lindsay
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Kaleidoscope.h"

namespace kaleidoscope {

class ProperShifting : public kaleidoscope::Plugin {
  public:
    ProperShifting(void) {}
    ~ProperShifting(void) {}

    static void enable(void);
    static void disable(void);
    static bool isActive(void);

    EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);

  private:
    static bool disabled;

    // Shift state
    static bool left_shift_on;
    static bool right_shift_on;
    static bool allow_events;

    static uint16_t modifiers[];
    static int active_modifiers;
    inline bool keyIsModifier(uint16_t key);

#define keyIsShift(key) (key == Key_LeftShift || key == Key_RightShift)
#define modifiersPressed() (active_modifiers > 0)
#define shiftsIdentical() (left_shift_on == right_shift_on)
#define keysCleared() (KeyboardHardware.pressedKeyswitchCount() == 0)

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
 protected:
  void begin();
  static Key legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState);
#endif

};
}

extern kaleidoscope::ProperShifting ProperShifting;
