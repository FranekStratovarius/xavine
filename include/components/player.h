#ifndef PLAYER_COMPONENTS_H
#define PLAYER_COMPONENTS_H
#include <bx/math.h>

// player tag is an empty struct
namespace xavine {
	struct Player {};

	struct Rotation {
		float pitch = 0.0f;	// rotate around x axis
		float roll = 0.0f;	// rotate around y axis
		float yaw = 0.0f;		// rotate around z axis
	};
}

#endif // PLAYER_COMPONENTS_H