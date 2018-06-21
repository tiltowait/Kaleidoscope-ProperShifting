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

#pragma once

#include <Kaleidoscope.h>

namespace kaleidoscope {

class BetterShifting : public kaleidoscope::Plugin {
	public:
		BetterShifting(void) {}
		~BetterShifting(void) {
			clearIgnoredKeys();
		}

		static void enable(void);
		static void disable(void);
		static bool isActive(void);

		static void enableModifierRules(void);
		static void disableModifierRules(void);
		static bool modifiersIgnoreShifting(void);

		EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);

		static void ignoreKeys(int num_keys, ...);
		static void clearIgnoredKeys();

	private:
		static bool disabled;

		// Shift state
		static bool left_shift_pressed;
		static bool right_shift_pressed;

		// Key ignores
		static uint16_t *ignored_keys;
		static int num_ignored_keys;
		inline bool keyIsIgnored(uint16_t key);

		static bool mods_ignore_shifting;
		static uint16_t modifiers[];
		static int num_modifiers_on;
		inline bool keyIsModifier(uint16_t key);

		#define MOD_OVERRIDE() (mods_ignore_shifting && num_modifiers_on > 0)

#if KALEIDOSCOPE_ENABLE_V1_PLUGIN_API
 protected:
  void begin();
  static Key legacyEventHandler(Key mapped_key, byte row, byte col, uint8_t keyState);
#endif

};
}

extern kaleidoscope::BetterShifting BetterShifting;
