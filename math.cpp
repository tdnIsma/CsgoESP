#include "math.h"


bool hacking_math::WorldToScreenDX(Vector3 pos, Vector2& screen, float viewMatrix[16]) {
	ImVec2 screenSize{ ImGui::GetIO().DisplaySize };
	glm::vec4 clipCoords;
	clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
	clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
	clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
	clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	glm::vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (screenSize.x / 2 * NDC.x) + (NDC.x + screenSize.x / 2);
	screen.y = -(screenSize.y / 2 * NDC.y) + (NDC.y + screenSize.y / 2);
	return true;
}

float Vector3::distancef(const Vector3& other) {
	return sqrtf((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y) + (this->z - other.z) * (this->z - other.z));
}