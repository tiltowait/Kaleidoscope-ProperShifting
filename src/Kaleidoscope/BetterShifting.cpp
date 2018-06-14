/* -*- mode: c++ -*-
 * Kaleidoscope-BetterShifting -- Force proper shift usage
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

#include <Kaleidoscope-BetterShifting.h>

namespace kaleidoscope {

//BetterShifting
bool BetterShifting::disabled = false;
bool BetterShifting::left_shift_pressed = false;
bool BetterShifting::right_shift_pressed = false;

void BetterShifting::enable() {
	disabled = false;
}

void BetterShifting::disable() {
	disabled = true;
}

bool BetterShifting::isActive() {
	return !disabled;
}

EventHandlerResult BetterShifting::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
	static const int DIVIDER = COLS / 2;
	static bool *shift_pressed;

	if(disabled) {
		return EventHandlerResult::OK;
	}

	if(mapped_key == Key_LeftShift || mapped_key == Key_RightShift) {
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

	//Allow any keypress if both shifts are held.
	//This makes typing in all-caps without capslock
	//less painful.
	if(left_shift_pressed && right_shift_pressed) {
		return EventHandlerResult::OK;
	}

	//Don't allow left half + left shift, nor
	//right half + right shift
	if(left_shift_pressed && col < DIVIDER) {
		return EventHandlerResult::EVENT_CONSUMED;
	} else if(right_shift_pressed && DIVIDER < col) {
		return EventHandlerResult::EVENT_CONSUMED;
	}

	//No shifts are held, or we're on the opposite side
	//of the held shift key.
	return EventHandlerResult::OK;
}

// Legacy V1 API
#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
void BetterShifting::begin() {
  Kaleidoscope.useEventHandlerHook(legacyEventHandler);
}

Key BetterShifting::legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState) {
  EventHandlerResult r = ::BetterShifting.onKeyswitchEvent(mapped_key, row, col, keyState);
  if (r == EventHandlerResult::OK)
    return mapped_key;
  return Key_NoKey;
}
#endif

}

kaleidoscope::BetterShifting BetterShifting;
