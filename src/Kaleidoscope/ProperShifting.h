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
namespace plugin {

class ProperShifting : public kaleidoscope::Plugin {
  public:
    // Methods relating to plugin state.
    static void enable(void);
    static void disable(void);
    static bool active(void);

    // Event-handler methods.
    EventHandlerResult beforeEachCycle();
    EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row,
                                        byte col, uint8_t key_state);

  private:
    static bool disabled_;
    static bool allow_events_;
    static Key modifiers_[];  // Modifier keys, such as Contol or Alt.

    // Key introspection methods.
    inline bool isKeyModifier(Key key);
    inline bool anyModifiersActive();
    inline int whichShiftActive();

    // Determine if key `key` is a shift.
    inline bool isKeyShift(Key key) {
      return key == Key_LeftShift
             || key == Key_RightShift;
    }

    // Determine if ANY key is being pressed.
    inline bool noKeysPressed() {
      return KeyboardHardware.pressedKeyswitchCount() == 0;
    }

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
 protected:
  void begin();
  static Key legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState);
#endif

};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::ProperShifting ProperShifting;
