#pragma once //tells the compiler to include this header only once
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <memory>

//gets the address where a module of our game got loaded to:
uintptr_t GetModuleBaseAddress(const wchar_t* modName);

//find dynamic address a multi level pointer is pointing to:
uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets);

void PatchInternal(BYTE* dst, BYTE* src, size_t size); //write new code to memory

class NopInternal { //class for replacing code with code that does nothing
private:
	BYTE* dst; //address of the code to patch
	size_t size; //size of the patch
	BYTE* originalCode; //backup of the original code
	BYTE* nopCode; //code that does nothing
public:
	NopInternal(BYTE* dst, size_t size); //constructor
	~NopInternal(); //destructor
	void enable(); //enable the patch
	void disable(); //disable the patch
};

class ManagedPatch {
	BYTE* dst; //address of the code to patch
	size_t size; //size of the patch
	BYTE* originalCode; //backup of the original code
	BYTE* newCode; //new functionality
public:
	ManagedPatch(BYTE* dst, BYTE* src, size_t size); //constructor
	~ManagedPatch(); //destructor
	void enable(); //enable the patch
	void disable(); //disable the patch
};

class Hook { //class for our Hook
    void* tToHook; //pointer to where to place the hook
    std::unique_ptr<char[]> oldOpcodes; //save old opodes here
    int tLen; //length of the overwritten instructions(s)
	bool enabled; //is the Hook enabled
public:
    Hook(void* toHook, void* ourFunct, int len); //constructor creates the hook
	~Hook(); //Destructor restores the original code
	void enable(); //enable the hook
	void disable(); //disable the hook
	bool isEnabled(); //returns if the hook is enabled
};

class TrampHook { //easy to use class for hooking functions
	void* gateWay;
	Hook* managedHook;
public:
	TrampHook(void* toHook, void* ourFunct, int len);
	~TrampHook(); //restores original code
	void enable();
	void disable();
	bool isEnabled();
	void* getGateway();
};

