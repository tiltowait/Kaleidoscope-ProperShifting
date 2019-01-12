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
bool ProperShifting::allow_events_ = true;
bool ProperShifting::last_event_allowed_ = false;

enum { LEFT, RIGHT, BOTH, NONE };  // Currently active shift key(s).

// For our purposes, a modifier is Control, Alt, and GUI. Though shift
// is technically a modifier, it is a special case and is treated
// separately.
Key ProperShifting::modifiers_[] = {
  Key_LeftControl, Key_RightControl,
  Key_LeftAlt, Key_RightAlt,
  Key_LeftGui, Key_RightGui
};
static const int kNumModifiers = 6;  // Used for lopping through modifier
                                     // array.

// Basic accessor methods.
void ProperShifting::enable() {
  disabled_ = false;
}

void ProperShifting::disable() {
  disabled_ = true;
  allow_events_ = true;
}

bool ProperShifting::active() {
  return !disabled_;
}

// A fast typist trying to capitalize a letter with the wrong shift can
// sometimes wind up typing that letter in lowercase. This happens when
// the following order of events takes place:
//   
//   1. Hold shift
//   2. Press a letter
//   3. Release shift
//   4. Release the letter
//
// This is a surprisingly common occurrence. To combat this, every time
// a shift key is released, we check to see if any other letters are being
// held. If that is the case, we put the keyboard into a "no events" state
// until all keys are released.
//
// We check the keyboard state between cycles; otherwise, we wind up
// consuming desired keyswitch events.
EventHandlerResult ProperShifting::beforeEachCycle() {
  if(!allow_events_) {
    allow_events_ = noKeysPressed();
  }
  return EventHandlerResult::OK;
}

// Prevent the user from typing an uppercase letter while holding the
// shift key on the same side as that letter; however, allow it if the
// user is also holding a non-shift modifier of any kind.
//
// Also, allow a semi-capslock mode when both shifts are being held.
EventHandlerResult ProperShifting::onKeyswitchEvent(Key &mapped_key,
                                                    byte row,
                                                    byte col,
                                                    uint8_t key_state) {
  // Disabling the plugin will set allowed_events_ to true, so it's safe
  // to test for it before anything else. Otherwise, we wind up with 
  if(!allow_events_) {
    // The word, "I", can cause problems, because the cycle for typing it
    // often follows the sequence: Shift on, I, Shift off, Space. If that
    // happens, the space will be erroneously consumed.
    if(mapped_key == Key_Spacebar) {
      return EventHandlerResult::OK;
    }
    return EventHandlerResult::EVENT_CONSUMED;
  }

  // Holding *any* non-shift modifier negates all shifting rules, so
  // we test that first in order to succeed early, if possible.
  if(disabled_
     || isKeyModifier(mapped_key)
     || Layer.top() != 0  // Prevents issues with '{' and '}' on default keymap.
     || anyModifiersActive()  // One may be activated in a previous cycle.
     || whichShiftActive() == BOTH) {
    return EventHandlerResult::OK;
  }

  // Shift events are always allowed, but we need to make sure the keyboard is
  // clear every time a shift is toggled off. See above for more information.
  //
  // If the last event was allowed, however, we don't do anything. This is
  // another protection for fast typists. Without that check, the 'a' in
  // "Mariachi" (and possibly the 'r') would be consumed when typing ~100 wpm.
  if(isKeyShift(mapped_key)) {
    if(keyToggledOff(key_state) && !last_event_allowed_) {
      allow_events_ = noKeysPressed();
    }
    return EventHandlerResult::OK;
  }

  // Each half of the Model 01 has an equal number of keys, so we can
  // use the midpoint as a dividing line to determine whether to allow
  // a shift to occur.
  static const int kDivider = COLS / 2;

  // Shift rules only take effect when one shift is active AND no other
  // modifiers are active. We already tested for BOTH shifts above.
  switch(whichShiftActive()) {
  case LEFT:  // Only right-side keys allowed
    if(col < kDivider) {
      last_event_allowed_ = false;
      return EventHandlerResult::EVENT_CONSUMED;
    }
    break;
  case RIGHT:  // Only left-side keys allowed
    if(kDivider <= col) {
      last_event_allowed_ = false;
      return EventHandlerResult::EVENT_CONSUMED;
    }
    break;
  default:  // Really just case NONE, since BOTH is already handled.
    break;
  }

  // Possible states:
  //   1. No shift and no modifiers held. (Print lowercase letter.)
  //   2. Shift and opposite-side key pressed. (Print uppercase letter.)
  // All other cases should already be handled.
  last_event_allowed_ = true;

  return EventHandlerResult::OK;
}

// The following methods test modifier states/identities.

// Determine if a given key is a non-shift modifier.
inline bool ProperShifting::isKeyModifier(Key key) {
  // If it's not a keyboard key, return false
  if(key.flags & (SYNTHETIC | RESERVED)) return false;
  if(isKeyShift(key)) return false;  // Don't consider space a modifier

  return (key.keyCode >= HID_KEYBOARD_FIRST_MODIFIER
          && key.keyCode <= HID_KEYBOARD_LAST_MODIFIER);
}

// Determine if any non-shift modifiers are active.
// TODO: See if there's a more efficient way of doing this.
inline bool ProperShifting::anyModifiersActive() {
  static int i;
  for(i = 0; i < kNumModifiers; i++) {
    if(wasModifierKeyActive(modifiers_[i])) {
      return true;
    }
  }
  return false;
}

// Determine which shift key (if any, or both) is currently active.
inline int ProperShifting::whichShiftActive() {
  if(wasModifierKeyActive(Key_LeftShift)
     && wasModifierKeyActive(Key_RightShift)) {
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

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::ProperShifting ProperShifting;
