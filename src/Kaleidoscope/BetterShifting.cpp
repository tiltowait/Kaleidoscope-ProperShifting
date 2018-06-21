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
#include <stdarg.h>

namespace kaleidoscope {

//BetterShifting
bool BetterShifting::disabled = false;
bool BetterShifting::left_shift_pressed = false;
bool BetterShifting::right_shift_pressed = false;

uint16_t * BetterShifting::ignored_keys = NULL;
int BetterShifting::num_ignored_keys = 0;

bool BetterShifting::mods_ignore_shifting = true;
int BetterShifting::num_modifiers_on = 0;
uint16_t BetterShifting::modifiers[] = {
	Key_LeftControl.raw, Key_LeftAlt.raw,
	Key_LeftGui.raw, Key_RightGui.raw,
	Key_RightAlt.raw, Key_RightControl.raw
};

void BetterShifting::enable() {
	disabled = false;
}

void BetterShifting::disable() {
	disabled = true;
}

bool BetterShifting::isActive() {
	return !disabled;
}

void BetterShifting::enableModifiersIgnoreShifting() {
	mods_ignore_shifting = true;
}

void BetterShifting::disableModifiersIgnoreShifting() {
	mods_ignore_shifting = false;
}

bool BetterShifting::modifiersIgnoreShifting() {
	return mods_ignore_shifting;
}

EventHandlerResult BetterShifting::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
	static const int DIVIDER = COLS / 2;
	static bool *shift_pressed;

	if(disabled || keyIsIgnored(mapped_key.raw)) {
		return EventHandlerResult::OK;
	}

	//Determine which shift, if any, is being held/released.
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

	//Modifier keys (alt, ctrl, etc.) override shifting rules.
	if(mods_ignore_shifting && keyIsModifier(mapped_key.raw)) {
		if(keyToggledOn(key_state)) {
			num_modifiers_on++;
		} else if(keyToggledOff(key_state)) {
			num_modifiers_on--;
		}
		return EventHandlerResult::OK;
	}

	//Allow any keypress if both shifts are held.
	//This makes typing in all-caps without capslock
	//less painful.
	if(modStatus() || (left_shift_pressed && right_shift_pressed)) {
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

//Build an array of keys (Key_*.raw) that won't have
//shift rules applied to them. Note that the argument
//type is uint16_t, and this function is NOT typesafe.
void BetterShifting::ignoreKeys(int num_keys, ...) {
	clearIgnoredKeys();

	if(num_keys < 1) {
		return;
	}

	ignored_keys = new uint16_t[num_keys];
	num_ignored_keys = num_keys;

	va_list args;
	va_start(args, num_keys);
	for(int i = 0; i < num_keys; i++) {
		ignored_keys[i] = va_arg(args, uint16_t);
	}
	va_end(args);
}

//Empty the current list of ignored keys, if there is
//one.
void BetterShifting::clearIgnoredKeys(void) {
	if(num_ignored_keys > 0) {
		delete [] ignored_keys;
	}
}

//Determine if a key should have the shift rules ignored.
inline bool BetterShifting::keyIsIgnored(uint16_t key) {
	static int i;
	for(i = 0; i < num_ignored_keys; i++) {
		if(ignored_keys[i] == key) {
			return true;
		}
	}
	return false;
}

//Return true if the passed key.raw is a modifier.
inline bool BetterShifting::keyIsModifier(uint16_t key) {
	static int i;
	for(i = 0; i < 6; i++) {
		if(key == modifiers[i]) {
			return true;
		}
	}
	return false;
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
