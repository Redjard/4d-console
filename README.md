# 4D UUID Obuscator

This mod allows opening (and closing) a console with c.
Other mods can send information about their shortcuts to this mod, where they are displayed in a unified way.
They can also add custom text to the console, and print into it during the duration of the game.

If a mod uses the console mod, but it is not installed, then nothing will happen, it is not a strict dependency.

If you are a mod developer, you can get the 4Dconsole.h from this repo, and be up and running in two lines:

```cpp
#include "4Dconsole.h"
main() {
	registerConsoleKeyinfo({{"key","description"}});
}
```

you can add free text before and after the shortcut printout as follows:
```cpp
#include "4Dconsole.h"

void init(){
	printf("This will get printed to the console before the shortcut list\n");
}
void info(){
	printf("This will get printed to the console after the shortcut list\n");
}

main() {
	registerConsoleCallback(init,ConsoleEvent::init);
	registerConsoleCallback(info,ConsoleEvent::info);
}
```