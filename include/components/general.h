#ifndef GENERAL_COMPONENTS_H
#define GENERAL_COMPONENTS_H
#include <bx/bx.h>
#include <bx/math.h>

namespace xavine {
	struct Position {
		bx::Vec3 position_vec;
	};
	
	// input datas
	struct KeyState {
		//bool pressed = false;
		//bool released = false;
		bool state = false;
	};

	struct MouseCoord {
		double x = 0;
		double y = 0;
	};

	struct MouseState {
		KeyState mouse_key_state[8];	// glfw can get 8 mouse buttons
		MouseCoord position;
		MouseCoord relative;
	};

	struct Input {
		MouseState mouse_state;
		KeyState key_state[128];	// some keyboards can have up to 110 keys
		double mouse_sensitivity = 0.01;
	};
}

#endif // GENERAL_COMPONENTS_H