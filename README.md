# Minecraft in C
This is a boredom project for me where I'm rewriting Minecraft true to its original form into pure C. Each branch represents a different version of Minecraft. There is a good chance I might return back to old versions if I find that there is an inconsistency with the original and mine, even if I am already working on a newer version.

## Version Classic 0.30 (WIP)
This is the first version I worked on, therefore it is hard to give an actual changelog.
 - Does not support file saving or loading
 - Does not support multiplayer
 - Small various things I am working on

## How to Build
I only make this project on WSL, so it must be built on linux unless you rewrite the makefile for building on Windows. However, the primary binary that I create here is only for Windows, which by the way, is completely statically linked.

Before the project can be built, you must convert all the assets into header files. This can be easily done with the following command: ```make assets```

Building for Windows:
```make windows -j```

Building for Linux:
```make -j```

If you want to switch survival or creative mode, you must open up the makefile yourself and make the following changes:

To add survival mode:

LINE 6: ```CFLAGS = -I./include -I./res -Wall -std=c11 -fms-extensions -g -O1 -flarge-source-files -DSURVIVAL_MODE```

LINE 14-15: ```WINDOWS_CFLAGS = -I./include -I./res -Wall -std=c11 -fms-extensions -Dmain=main \
    -isystem ./lib -DSURVIVAL_MODE```

All you must do is add -DSURVIVAL_MODE to the C compiler flags. To get creative mode, you need to remove these flags.

## Credits
I handwrote all of the code, however I must give credit to johnpayne-dev since I used his code as a big reference in early development while working on creative mode. Also, it is worth mentioning this project is not affiliated whatsoever with Mojang Studios or Minecraft at all.