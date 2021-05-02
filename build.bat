@echo off
set project_name=win32

set args= -GR- -EHa -nologo -Zi -experimental:external -external:anglebrackets -DDEBUG
set include_path=-external:I ..\include\ 

set linker_options=-link -SUBSYSTEM:WINDOWS -LIBPATH:..\lib 
set libs=SDL2main.lib SDL2.lib SDL2_ttf.lib SDL2_image.lib Shell32.lib

pushd bin
cl %args% -Fe%project_name% %include_path% ../src/win32.cpp %linker_options% %libs%
cl %args% -Fegame %include_path% -LD ../src/game.cpp %linker_options% %libs%
echo a > game.meta
popd
echo Build completed!
