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

using namespace kaleidoscope::hid;

namespace kaleidoscope {

// ProperShifting

bool ProperShifting::disabled = false;

/**
 * For our purposes, a modifier is Control, Alt, and GUI.
 * Though shift is technically a modifier, it is a special
 * case and treated separately.
 */
Key ProperShifting::modifiers[] = {
  Key_LeftControl, Key_RightControl,
  Key_LeftAlt, Key_RightAlt,
  Key_LeftGui, Key_RightGui
};
#define NUM_MODIFIERS 6

/**
 * Basic accessor methods.
 */
void ProperShifting::enable() {
  disabled = false;
}

void ProperShifting::disable() {
  disabled = true;
}

bool ProperShifting::isActive() {
  return !disabled;
}

/**
 * The main event.
 */
EventHandlerResult ProperShifting::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
  static const int DIVIDER = COLS / 2;

  if(disabled) {
    return EventHandlerResult::OK;
  }

  /**
   * Holding *any* non-shift modifier negates all shifting rules, so
   * we test that first in order to succeed early, if possible.
   */
  if(mapped_key == Key_Spacebar ||
     keyIsShift(mapped_key) ||
     anyModifiersActive()) {
    return EventHandlerResult::OK;
  }

  /**
   * Shift rules only take effect when one shift is active AND
   * no other modifiers are active.
   */
  if(!keyIsModifier(mapped_key)) {
    if(!bothShiftsActive()) {
      // Left shift -> only right-side keys allowed
      if(isModifierKeyActive(Key_LeftShift) && col < DIVIDER) {
        return EventHandlerResult::EVENT_CONSUMED;
      }

      // Right shift -> only left-side keys allowed
      if(isModifierKeyActive(Key_RightShift) && DIVIDER < col) {
        return EventHandlerResult::EVENT_CONSUMED;
      }
    }
  }

  /**
   * Possible states:
   *  1. No shift and no modifiers held. (Print lowercase letter.)
   *  2. Modifier being released.
   *  3. Shift and opposite-side key pressed. (Print uppercase letter.)
   */
  return EventHandlerResult::OK;
}

/**
 * The following methods test modifier states/identities.
 */

inline bool ProperShifting::keyIsModifier(Key key) {
  static int i;
  for(i = 0; i < NUM_MODIFIERS; i++) {
    if(key == modifiers[i]) {
      return true;
    }
  }
  return false;
}

inline bool ProperShifting::anyModifiersActive() {
  static int i;
  for(i = 0; i < NUM_MODIFIERS; i++) {
    if(isModifierKeyActive(modifiers[i])) {
      return true;
    }
  }
  return false;
}

inline bool ProperShifting::bothShiftsActive() {
  return isModifierKeyActive(Key_LeftShift) &&
         isModifierKeyActive(Key_RightShift);
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
