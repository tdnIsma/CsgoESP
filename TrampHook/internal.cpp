#include <mutex>
#include "internal.h"

uintptr_t GetModuleBaseAddress(const wchar_t* modName) {
	return (uintptr_t)GetModuleHandleW(modName);
}

uintptr_t FindDynamicAddress(uintptr_t ptr, std::vector<unsigned int> offsets) {
	for (unsigned int i = 0; i < offsets.size(); ++i) { //loop over the offsets
		ptr = *(BYTE*)ptr; //dereference pointer
		ptr += offsets[i]; //add the current offset to addr
	}
	return ptr; //return the address the multi level pointer is pointing at currently
}

void PatchInternal(BYTE* dst, BYTE* src, size_t size) {
	DWORD oldprotect; //variable to hold a backup of our old protection
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect); //make memory writeable and save the old protection in oldprotect
	memcpy(dst, src, size); //write the new opcodes to the target location
	VirtualProtect(dst, size, oldprotect, &oldprotect); //restore our old protection
}

NopInternal::NopInternal(BYTE* dst, size_t size)
	: dst{ dst }, size{ size }, originalCode{ new BYTE[size] }, nopCode{ new BYTE[size] } {
	memset(nopCode, 0x90, size); //initialize our nopCode
	memcpy(originalCode, dst, size); //backup our original Code
}

void NopInternal::enable() { //enable our patch
	//write the code that does nothing to memory:
	PatchInternal(dst, nopCode, size);
}

void NopInternal::disable() { //disable our patch
	//write the original code back to memory:
	PatchInternal(dst, originalCode, size);
}

NopInternal::~NopInternal() { //clean up our patch object
	delete[] this->originalCode;
	delete[] this->nopCode;
}

ManagedPatch::ManagedPatch(BYTE* dst, BYTE* src, size_t size)
    : dst{ dst }, size{ size }, originalCode{ new BYTE[size] }, newCode{ new BYTE[size] } {
    memcpy(newCode, src, size); //initialize our newCode
    memcpy(originalCode, dst, size); //backup our original Code
}

void ManagedPatch::enable() { //enable our patch
    //write the code that does nothing to memory:
    PatchInternal(dst, newCode, size);
}

void ManagedPatch::disable() { //disable our patch
    //write the original code back to memory:
    PatchInternal(dst, originalCode, size);
}

ManagedPatch::~ManagedPatch() { //clean up our patch object
    delete[] this->originalCode;
    delete[] this->newCode;
}

Hook::Hook(void* toHook, void* ourFunct, int len) : tToHook{ toHook }, oldOpcodes{ nullptr }, tLen{ len }, enabled{ false } {
    /*a jmp instruction is 5 bytes in size. 
    The place we overwrite needs to be at least of that size*/
    if (len < 5) {
        return;
    }

    DWORD curProtection; //stores the old memory prtection
    //make the memory with the code we want to overwrite writeable 
    VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

    //save the current opcodes byte by byte into a char array
    oldOpcodes = std::make_unique<char[]>(len);
    if (oldOpcodes != nullptr) {
        for (int i = 0; i < len; ++i) {
            oldOpcodes[i] = ((char*)toHook)[i];
        }
    }

    //Overwrite the place we want to hook with nop instructions
    memset(toHook, 0x90, len);

    //claculate the relative address of where to jump to
    DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;

    *(BYTE*)toHook = 0xE9; //place the opcode for the jmp instruction
    //place the address of where to jump to
    *(DWORD*)((DWORD)toHook + 1) = relativeAddress;

    //restore the old code protection
    VirtualProtect(toHook, len, curProtection, &curProtection);
}

Hook::~Hook() {
    if (oldOpcodes != nullptr) {
        DWORD curProtection;
        //make memory writeable
        VirtualProtect(tToHook, tLen, PAGE_EXECUTE_READWRITE, &curProtection);
        for (int i = 0; i < tLen; ++i) { //write the old opcodes back to the hooked location
            ((char*)tToHook)[i] = Hook::oldOpcodes[i];
        }
        //restore old memory protection
        VirtualProtect(tToHook, tLen, curProtection, &curProtection);
    }
}

void Hook::enable() {
    this->enabled = true;
}

void Hook::disable() {
    this->enabled = false;
}

bool Hook::isEnabled() {
    return enabled;
}

TrampHook::TrampHook(void* toHook, void* ourFunct, int len): 
    gateWay{ nullptr }, managedHook{ nullptr } {

    if (len < 5) return; //jmp is 5 bytes in size

    //allocate memory for our gateway:
    gateWay = VirtualAlloc(0, len+5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    //write the bytes we will overwrite to the gateway
    memcpy_s(gateWay, len, toHook, len);

    //get the return address:
    uintptr_t returnAddress = (BYTE*)toHook - (BYTE*)gateWay - 5;

    //place a jmp opcode to the end of the gateway:
    *(BYTE*)((uintptr_t)gateWay + len) = 0xE9;

    //place the return Address after the jmp:
    *(uintptr_t*)((uintptr_t)gateWay + len + 1) = returnAddress;

    //create the Hook:
    managedHook = new Hook(toHook, ourFunct, len);
}

TrampHook::~TrampHook() {
    managedHook->disable();
    delete managedHook;
    VirtualFree(gateWay, 0, MEM_RELEASE);
}

void TrampHook::enable() {
    managedHook->enable();
}

void TrampHook::disable() {
    managedHook->disable();
}

bool TrampHook::isEnabled() {
    return managedHook->isEnabled();
}

void* TrampHook::getGateway() {
    return gateWay;
}