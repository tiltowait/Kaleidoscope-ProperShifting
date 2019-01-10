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

#include "Kaleidoscope-ProperShifting.h"

namespace kaleidoscope {

//ProperShifting

//Basic shifting ivars.
bool ProperShifting::disabled = false;
bool ProperShifting::left_shift_pressed = false;
bool ProperShifting::right_shift_pressed = false;

//ivars for special modifier key rules
int ProperShifting::num_modifiers_on = 0;
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
	static bool *shift_pressed;

	if(disabled) {
		return EventHandlerResult::OK;
	}

	//If a shift's key_state has been altered, store the change and exit
	if(keyIsShift(mapped_key)) {
		if(mapped_key == Key_LeftShift) {
			shift_pressed = &left_shift_pressed;
		} else {
			shift_pressed = &right_shift_pressed;
		}

		if(keyToggledOn(key_state)) {
			*shift_pressed = true;
		} else if(keyToggledOff(key_state)) {
			*shift_pressed = false;
		}
		return EventHandlerResult::OK;
	}

	//Modifier keys (alt, ctrl, etc.) override shifting rules.
	if(keyIsModifier(mapped_key.raw)) {
		if(keyToggledOn(key_state)) {
			num_modifiers_on++;
		} else if(keyToggledOff(key_state)) {
			num_modifiers_on--;
		}
		return EventHandlerResult::OK;
	}

	//Allow any keypress if modifiers are being held or if both shifts
	//are the same (both off or both on).
	if(modifiersPressed() || shiftsIdentical()) {
		return EventHandlerResult::OK;
	}

	//Don't allow left half + left shift, nor right half + right shift
	if(left_shift_pressed && col < DIVIDER) {
		return EventHandlerResult::EVENT_CONSUMED;
	} else if(right_shift_pressed && DIVIDER < col) {
		return EventHandlerResult::EVENT_CONSUMED;
	}

	//mapped_key is on the opposite side from the held shift, which is okay
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
