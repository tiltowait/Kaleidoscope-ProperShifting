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

#include "ProperShifting.h"

namespace kaleidoscope {

//ProperShifting

//Basic shifting ivars.
bool ProperShifting::disabled = false;
bool ProperShifting::left_shift_on = false;
bool ProperShifting::right_shift_on = false;
bool ProperShifting::allow_events = true;

//ivars for special modifier key rules
int ProperShifting::active_modifiers = 0;
uint16_t ProperShifting::modifiers[] = {
  Key_LeftControl.raw, Key_LeftAlt.raw,
  Key_LeftGui.raw, Key_RightGui.raw,
  Key_RightAlt.raw, Key_RightControl.raw
};
#define NUM_MODIFIER_KEYS 6

//basic accessor methods
void ProperShifting::enable() {
  disabled = false;
}

void ProperShifting::disable() {
  disabled = true;
}

bool ProperShifting::isActive() {
  return !disabled;
}

EventHandlerResult ProperShifting::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
  static const int DIVIDER = COLS / 2;

  if(disabled) {
    return EventHandlerResult::OK;
  }

  if(keyIsShift(mapped_key)) {
    if(keyToggledOn(key_state)) {
      if(mapped_key == Key_LeftShift) {
        left_shift_on = true;
      } else {
        right_shift_on = true;
      }
    } else if(keyToggledOff(key_state)) {
      if(mapped_key == Key_LeftShift) {
        left_shift_on = false;
      } else {
        right_shift_on = false;
      }

      if(shiftsIdentical()) { //Are both shifts OFF?
        allow_events = keysCleared();
      }
    }
    return EventHandlerResult::OK;
  } else if(keyIsModifier(mapped_key.raw)) {
    if(keyToggledOn(key_state)) {
      active_modifiers++;
    } else if(keyToggledOff(key_state)) {
      active_modifiers--;
    }
    return EventHandlerResult::OK;
  }

  /* Start looking at normal keys */

  /**
   * We want to avoid a weird situation where spurious lowercase letters
   * appear. This can happen if you hold Shift, then a letter, then release
   * that letter (all on same side of keyboard). To avoid this, we require
   * all keys to be released whenever a shift is toggled off.
   */
  if(!allow_events) {
    allow_events = keysCleared();
    return EventHandlerResult::EVENT_CONSUMED;
  } else {
    active_modifiers = 0; //kludge to fix issue with Qukeys
  }

  if(shiftsIdentical() || modifiersPressed()) {
    return EventHandlerResult::OK;
  }

  //left side
  if(left_shift_on && col < DIVIDER) {
    return EventHandlerResult::EVENT_CONSUMED;
  }
  //right side
  if(right_shift_on && DIVIDER < col) {
    return EventHandlerResult::EVENT_CONSUMED;
  }
  return EventHandlerResult::OK;
}

//Return true if the passed key.raw is a modifier.
inline bool ProperShifting::keyIsModifier(uint16_t key) {
  static int i;
  for(i = 0; i < NUM_MODIFIER_KEYS; i++) {
    if(key == modifiers[i]) {
      return true;
    }
  }
  return false;
}

// Legacy V1 API
#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
void ProperShifting::begin() {
  Kaleidoscope.useEventHandlerHook(legacyEventHandler);
}

Key ProperShifting::legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState) {
  EventHandlerResult r = ::ProperShifting.onKeyswitchEvent(mapped_key, row, col, keyState);
  if (r == EventHandlerResult::OK)
    return mapped_key;
  return Key_NoKey;
}
#endif

}

kaleidoscope::ProperShifting ProperShifting;
