// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;

bool bAimbot = false;
bool bESP = false;
bool bSnapLines = false;
static bool init = false;
bool show_menu = false;
bool bExit = true;
bool bFov = false;
int fov = 90;

uintptr_t clientModule;
Entity* entList;
Player* player;
static float* W2S_Matrix;

Vector3 GetBonePos(EntListObj ent, int bone) {
	uintptr_t bonePtr = entList->entListObjs->ent->BoneMatrix;
	Vector3 bonePos;
	bonePos.x = *(float*)(bonePtr + 0x30 * bone + 0x0C);
	bonePos.y = *(float*)(bonePtr + 0x30 * bone + 0x1C);
	bonePos.z = *(float*)(bonePtr + 0x30 * bone + 0x2C);
	return bonePos;
}

void DrawFilledRect(LPDIRECT3DDEVICE9 pDevice, Vector2 var, Vector2 weh, D3DCOLOR color) {
	D3DRECT rect = { var.x,var.y,var.x + weh.x,var.y + weh.y };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void rect(const ImVec2& upperLeft, const ImVec2& lowerRight, float width, const ImColor& color) noexcept {

	auto draw = ImGui::GetBackgroundDrawList();
	draw->AddRect(upperLeft, lowerRight, colors::rgb::black, 0.0f, 15, width + 2);
	draw->AddRect(upperLeft, lowerRight, color, 0.0f, 15, width);
	
}

void StatusHealth(EntListObj ent, const Vector2 lowerRight)
{
	auto draw = ImGui::GetBackgroundDrawList();
	std::stringstream h;
	h << ent.ent->Health;
	std::string hT = "Health: " + h.str();

	char* HealthText = (char*)hT.c_str();

	draw->AddText(ImVec2(lowerRight.x - 30, lowerRight.y - 0), colors::rgb::yellow, HealthText, 0);
}

void Circle(const ImVec2& pos, float radius, const ImColor& color) noexcept {
	auto draw = ImGui::GetBackgroundDrawList();
	draw->AddCircle(pos, radius, colors::rgb::black, 0, 3.0f);
	draw->AddCircle(pos, radius, color);
}

void DrawLine(LPDIRECT3DDEVICE9 pDevice, int x1, int y1, int x2, int y2, int thickness, const ImColor& color) {
	ID3DXLine* LineL;
	D3DXCreateLine(pDevice, &LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->Draw(Line, 2, color);
	LineL->Release();
}

void DrawLine2(LPDIRECT3DDEVICE9 pDevice, Vector2 src, Vector2 dst, int thickness, D3DCOLOR color) {
	DrawLine(pDevice,src.x, src.y, dst.x, dst.y, thickness, color);
}

void DrawEspBox2D(Vector2 pos, float characterHeight, float distance,int thickness, ImColor& color) {
	float lineWidth = 1.0f;

	ImVec2 display{ ImGui::GetIO().DisplaySize };

	float height = (display.y / distance) * characterHeight;
	float width = (display.x / distance) * characterHeight / 3;


	rect({ pos.x - width / 2, pos.y - height }, { pos.x + width / 2, pos.y }, lineWidth, color);

}

void SnapLines(LPDIRECT3DDEVICE9 pDevice) {

	ImVec2 screenSize{ ImGui::GetIO().DisplaySize };

	for (auto currPlayer : entList->entListObjs) {



		if (!player) continue;
		if (!currPlayer.ent) continue;
		if (currPlayer.ent == player) continue;
		if (currPlayer.ent->isDormant) continue;


		if (currPlayer.ent->Health > 0 && currPlayer.ent->Health < 200) {

			ImColor color;
			if (player->Team == currPlayer.ent->Team) {

				color = colors::rgb::black;

			}
			else {
				color = colors::rgb::black;
			}

			Vector2 entPos2D;//var para segurar os valores
			if (hacking_math::WorldToScreenDX(currPlayer.ent->vecOrigin, entPos2D, W2S_Matrix)) {
				DrawLine(pDevice, entPos2D.x, entPos2D.y, screenSize.x / 2, screenSize.y, 2, color);
			}


		}

	}
}

void ESP() {

	ImVec2 screenSize{ ImGui::GetIO().DisplaySize };

	for (auto currPlayer : entList->entListObjs) {


		if (!player) continue;
		if (!currPlayer.ent) continue;
		if (currPlayer.ent == player) continue;
		if (currPlayer.ent->isDormant) continue;


		if (currPlayer.ent->Health > 0 && currPlayer.ent->Health < 200) {

			ImColor color;
			if (player->Team == currPlayer.ent->Team) {

				color = colors::rgb::blue;

			}
			else {
				color = colors::rgb::red;
			}


			Vector3 headPos = GetBonePos(currPlayer, 8);
			headPos.z += 8;

			Vector3 myPos = player->vecOrigin;

			float distance = myPos.distancef(currPlayer.ent->vecOrigin);

			Vector2 entPos2D, entHead2D;
			if (hacking_math::WorldToScreenDX(currPlayer.ent->vecOrigin, entPos2D, W2S_Matrix)) {

				DrawEspBox2D(entPos2D, 50.0f, distance, 2, color);
				StatusHealth(currPlayer, entPos2D);

			}


		}

	}
}

//degrees pitch -89, 89  yaw -180 , 180
void Aimbot() {

	ImVec2 display{ ImGui::GetIO().DisplaySize };
	float centerY = display.y / 2.0f;
	float centerX = display.x / 2.0f;

	Circle(ImVec2{ centerX, centerY }, 50, colors::rgb::yellow);

}



void StartMenu() {

	ImGui::Begin("Menu");

	ImGui::Text("Functions:");
	ImGui::Checkbox("Aimbot", &bAimbot);
	ImGui::Checkbox("ESP", &bESP);
	ImGui::Checkbox("SnapLines", &bSnapLines);
	ImGui::SliderInt("Fov", &fov, -180, 180);
	ImGui::Text("");
	ImGui::Text("[HOME] to close menu");

	ImGui::End();


}




void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!init)
	{
		InitImGui(pDevice);
		init = true;
	}

	if (GetAsyncKeyState(VK_END)) {
		kiero::shutdown();
		Sleep(100);
		(WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)oWndProc);
		Sleep(100);
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		bExit = false;
		return 0;
	}


	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		show_menu = !show_menu;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (show_menu)StartMenu();

	if (bESP) {
		draw::Start();
		ESP();
		draw::End();
	}

	if (bSnapLines)SnapLines(pDevice);

	if (bAimbot) {
		draw::Start();
		Aimbot();
		draw::End();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}



DWORD WINAPI InternalMain(HMODULE hMod)
{
	clientModule = (uintptr_t)GetModuleHandle("client.dll");
	entList = (Entity*)(clientModule + offsets::dwEntityList);
	player = *(Player**)(clientModule + offsets::dwLocalPlayer);
	W2S_Matrix = reinterpret_cast<float*>(clientModule + offsets::dwViewMatrix);

	bool attached = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			kiero::bind(42, (void**)&oEndScene, hkEndScene);
			do
				window = GetProcessWindow();
			while (window == NULL);
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);

	while (bExit) {

	}

	Sleep(100);
	FreeLibraryAndExitThread(hMod, 0);
	return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		//start a new thread for our cheat to run in so this thread can return to the injector
		HANDLE tHndl = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InternalMain, hModule, 0, 0);
		if (tHndl) CloseHandle(tHndl); //if we where able to create a thread, close the handle to it
		else return FALSE; //if we where not able to create a thread, return false
		break;
	}
	return TRUE; //everything went fine
}
