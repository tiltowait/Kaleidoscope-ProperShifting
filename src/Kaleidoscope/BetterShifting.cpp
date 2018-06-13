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
bool BetterShifting::leftHalfDisabled = false;
bool BetterShifting::rightHalfDisabled = false;

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

	if(disabled) {
		return EventHandlerResult::OK;
	}

	if(mapped_key.raw == Key_LeftShift.raw) {
		if(keyIsPressed(key_state)) {
			leftHalfDisabled = true;
		} else {
			leftHalfDisabled = false;
		}
	} else if(mapped_key.raw == Key_RightShift.raw) {
		if(keyIsPressed(key_state)) {
			rightHalfDisabled = true;
		} else {
			rightHalfDisabled = false;
		}
	} else {
		if(col < DIVIDER && leftHalfDisabled) {
			return EventHandlerResult::EVENT_CONSUMED;
		} else if(col > DIVIDER && rightHalfDisabled) {
			return EventHandlerResult::EVENT_CONSUMED;
		}
	}

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
