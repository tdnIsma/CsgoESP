#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#define ABS(x) ((x < 0) ? (-x) : (x))

struct Vector2 { float x, y; };
struct Vector3 {
	float x, y, z;
	float distancef(const Vector3& other);
};
struct Vector4 { float x, y, z, w; };



namespace hacking_math {

	bool WorldToScreenDX(Vector3 pos, Vector2& screen, float viewMatrix[16]);

};
