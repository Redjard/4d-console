
#define MOD_NAME "4D Console"
#define MOD_VER "1.1"

#include <4dm.h>
#include "4Dconsole.h"
using namespace fdm;


using console_mod_interface::wrapper_console_keyinfos;

std::vector<ConsoleKeyinfo> keyinfos;

using console_mod_interface::consoleCallbackFunct;
struct {
	std::vector<consoleCallbackFunct> init;
	std::vector<consoleCallbackFunct> info;
} callbacks;

extern "C" __declspec(dllexport) bool registerKeyinfo( wrapper_console_keyinfos new_keyinfos ) {
	
	int i = 0;
	do {  // insert keyinfo by keyinfo sorted
		keyinfos.insert( std::upper_bound( keyinfos.begin(), keyinfos.end(), new_keyinfos[i], [](auto a, auto b){return
			a.key < b.key;  // sort by key ascending
		} ), new_keyinfos[i] );
	} while (new_keyinfos[++i].key);
	
	return true;
}
extern "C" __declspec(dllexport) bool registerCallback( consoleCallbackFunct callback, ConsoleEvent event ) {
	if (event==ConsoleEvent::init)
		callbacks.init.push_back(callback);
	else if (event==ConsoleEvent::info)
		callbacks.info.push_back(callback);
	else
		return false;
	
	return true;
}


void toggleConsole(){
	static bool consoleOpen = false;
	
	if (!consoleOpen) {
		AllocConsole();
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		std::cout.clear();
		
		printf("\n");
		
		for ( auto callback : callbacks.init )
			callback();
		
		if (callbacks.init.size())
			printf("\n");
		
		printf("Keybinds:\n");
		for ( ConsoleKeyinfo keyinfo : keyinfos )
			std::cout << std::setfill(' ') << std::setw(8) << keyinfo.key << "    " << keyinfo.description << std::endl;
		
		printf("\n");
		
		for ( auto callback : callbacks.info )
			callback();
		
	} else {
		FreeConsole();
	}
	
	consoleOpen = !consoleOpen;
}




bool isInTextInput = false;

typedef bool(__thiscall* KeyInputFunct)(void* self, StateManager& s, int key, int scancode, int action, int mods);
std::vector<KeyInputFunct> originals;
template<auto i> bool generic_keyinput(void* self, StateManager& s, int key, int scancode, int action, int mods ) {
	
	isInTextInput = false;
	
	// this will call gui_textinput_keyInput_H() if a TextInput is focused
	auto result = originals[i](self, s, key, scancode, action, mods);
	
	// if gui_textinput_keyInput_H() was called, we don't do shortcuts
	if (isInTextInput)
		return result;
	
	if(action == GLFW_PRESS && key == GLFW_KEY_C)
		toggleConsole();
	
	return result;
}

KeyInputFunct gui_textinput_keyInput;
bool gui_textinput_keyInput_H(void* self, StateManager& s, int key, int scancode, int action, int mods) {
	isInTextInput = true;
	return gui_textinput_keyInput(self, s, key, scancode, action, mods);
}


auto addrs = std::array{
	FUNC_STATETUTORIAL_KEYINPUT,
	FUNC_STATESKINCHOOSER_KEYINPUT,
	FUNC_STATESETTINGS_KEYINPUT,
	FUNC_STATECREDITS_KEYINPUT,
	FUNC_STATECREATEWORLD_KEYINPUT,  // everything is createnewworld
	FUNC_STATEGAME_KEYINPUT,
	FUNC_STATEDEATHSCREEN_KEYINPUT,
	FUNC_STATETITLESCREEN_KEYINPUT,
	FUNC_STATEMULTIPLAYER_KEYINPUT,
};

DWORD WINAPI Main_Thread(void* hModule) {
	
	registerConsoleKeyinfo({{"c","Toggle this console"}});
	
	// don't even try touching this
	// this hooks all addresses in addrs[] with generic_keyinput()
	([]<auto... i>(std::index_sequence<i...>){(
		Hook(addrs[i], generic_keyinput<i>, &originals.emplace_back())
	, ...);})(std::make_index_sequence<addrs.size()>());
	
	Hook( FUNC_GUI_TEXTINPUT_KEYINPUT, &gui_textinput_keyInput_H, &gui_textinput_keyInput );  // for checking we aren't typing into one
	
	EnableHook(nullptr);
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD _reason, LPVOID lpReserved) {
	if (_reason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main_Thread, hModule, 0, NULL);
	return true;
}
