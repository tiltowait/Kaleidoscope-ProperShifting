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

    static Key modifiers[];
    inline bool isKeyModifier(Key key);
    inline bool anyModifiersActive();
    inline bool bothShiftsActive();
    inline bool isKeyBlessed(Key key);

#define isKeyShift(key) (key == Key_LeftShift || key == Key_RightShift)

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
 protected:
  void begin();
  static Key legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState);
#endif

};
}

extern kaleidoscope::ProperShifting ProperShifting;
