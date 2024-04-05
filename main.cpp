#include <4dm.h>
#include "4Dconsole.h"
#include "4DKeyBinds.h"
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


void toggleConsole(GLFWwindow* window, int action, int _mods) {
	if(action != GLFW_PRESS)
		return;
	
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


$exec {
	KeyBinds::addBind( "Toggle console", glfw::C, KeyBinds::GLOBAL, toggleConsole );
}
