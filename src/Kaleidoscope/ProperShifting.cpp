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
namespace plugin {

// ProperShifting

bool ProperShifting::disabled_ = false;
enum { LEFT, RIGHT, BOTH, NONE }; // Active shift key

/**
 * For our purposes, a modifier is Control, Alt, and GUI.
 * Though shift is technically a modifier, it is a special
 * case and treated separately.
 */
Key ProperShifting::modifiers_[] = {
  Key_LeftControl, Key_RightControl,
  Key_LeftAlt, Key_RightAlt,
  Key_LeftGui, Key_RightGui
};
static const int kNumModifiers = 6;

/**
 * Basic accessor methods.
 */
void ProperShifting::enable() {
  disabled_ = false;
}

void ProperShifting::disable() {
  disabled_ = true;
}

bool ProperShifting::isActive() {
  return !disabled_;
}

/**
 * The main event.
 */
EventHandlerResult ProperShifting::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
  /**
   * Holding *any* non-shift modifier negates all shifting rules, so
   * we test that first in order to succeed early, if possible.
   */
  if(disabled_ ||
     mapped_key == Key_Spacebar ||
     isKeyShift(mapped_key) ||
     isKeyModifier(mapped_key) ||
     anyModifiersActive() ||
     whichShiftActive() == BOTH) {
    return EventHandlerResult::OK;
  }

  /**
   * Shift rules only take effect when one shift is active AND
   * no other modifiers are active.
   */
  static const int kDivider = COLS / 2;

  switch(whichShiftActive()) {
  case LEFT: // Only right-side keys allowed
    if(col < kDivider) {
      return EventHandlerResult::EVENT_CONSUMED;
    }
    break;
  case RIGHT: // Only left-side keys allowed
    if(kDivider < col) {
      return EventHandlerResult::EVENT_CONSUMED;
    }
    break;
  default: // Really just case NONE, since BOTH is handled above
    break;
  }

  /**
   * Possible states:
   *  1. No shift and no modifiers held. (Print lowercase letter.)
   *  2. Shift and opposite-side key pressed. (Print uppercase letter.)
   *  All other cases should already be handled.
   */
  return EventHandlerResult::OK;
}

/**
 * The following methods test modifier states/identities.
 */

inline bool ProperShifting::isKeyModifier(Key key) {
  // If it's not a keyboard key, return false
  if(key.flags & (SYNTHETIC | RESERVED)) return false;
  if(isKeyShift(key)) return false; // Don't consider space a modifier

  return (key.keyCode >= HID_KEYBOARD_FIRST_MODIFIER &&
          key.keyCode <= HID_KEYBOARD_LAST_MODIFIER);
}

inline bool ProperShifting::anyModifiersActive() {
  static int i;
  for(i = 0; i < kNumModifiers; i++) {
    if(wasModifierKeyActive(modifiers_[i])) {
      return true;
    }
  }
  return false;
}

inline int ProperShifting::whichShiftActive() {
  if(wasModifierKeyActive(Key_LeftShift) &&
     wasModifierKeyActive(Key_RightShift)) {
    return BOTH;
  } else if(wasModifierKeyActive(Key_LeftShift)) {
    return LEFT;
  } else if(wasModifierKeyActive(Key_RightShift)) {
    return RIGHT;
  }
  return NONE;
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

} // namespace plugin
} // namespace kaleidoscope

kaleidoscope::plugin::ProperShifting ProperShifting;
