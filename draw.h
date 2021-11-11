#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "kiero/kiero.h"
#include <iostream>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


namespace colors {
	namespace rgb {
		const ImColor red{ 1.0f , 0.0f, 0.0f, 0.7f };
		const ImColor green{ 0.0f, 1.0f, 0.0f, 0.7f };
		const ImColor blue{ 0.0f, 0.0f, 1.0f, 0.7f };
		const ImColor gray{ 0.2f, 0.2f, 0.2f, 0.7f };
		const ImColor lightgray{ 0.7f , 0.7f, 0.7f, 0.7f };
		const ImColor yellow{ 1.0f, 1.0f, 0.0f, 0.7f };
		const ImColor orange{ 0.7f, 0.5f, 0.0f, 0.7f };
		const ImColor pink{ 0.7f, 0.5f, 0.7f, 0.7f };
		const ImColor cyan{ 0.0f, 0.7f, 0.7f, 0.7f };
		const ImColor black{ 0.0f, 0.0f, 0.0f, 0.7f };
	}
}



namespace draw {
	void StartMenu();
	void Start();
	void End();
	void StartFrame();
	void EndFrame();
}
