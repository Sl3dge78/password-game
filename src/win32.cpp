#include <windows.h>

#include "game.h"

typedef struct Module {
    char *meta_path;
    FILETIME last_write_time;
    HMODULE dll;
} Module;

void GameLoadFunctions(Module *dll) {
    pfn_GameInit = (fn_GameInit *)  GetProcAddress(dll->dll, "Init");
    pfn_GameInput = (fn_GameInput *)  GetProcAddress(dll->dll, "Input");
    pfn_GameUpdate = (fn_GameUpdate *)  GetProcAddress(dll->dll, "Update");
    pfn_GameDraw = (fn_GameDraw *)  GetProcAddress(dll->dll, "Draw");
    pfn_GameClose = (fn_GameClose *)  GetProcAddress(dll->dll, "Close");
}

internal inline FILETIME Win32GetLastWriteTime(char *file_name) {
    
    FILETIME last_write_time = {};
    
    WIN32_FIND_DATA data;
    HANDLE handle = FindFirstFile(file_name, &data);
    if(handle != INVALID_HANDLE_VALUE) {
        last_write_time = data.ftLastWriteTime;
        FindClose(handle);
    }
    
    return last_write_time;
}

internal bool Win32ShouldReloadModule(Module *module) {
    FILETIME current_time = Win32GetLastWriteTime(module->meta_path);
    if(CompareFileTime(& module->last_write_time, &current_time)) { // Has the file changed ?
        module->last_write_time = current_time;
        return true;
    }
    return false;
}

internal void Win32LoadModule(Module *module, const char* name) {
    
    SDL_Log("Loading module %s", name);
    
    u32 name_length = strlen(name);
    const char *path = "bin\\";
    const u32 path_length = strlen(path); 
    
    module->meta_path = (char *)calloc(path_length + name_length + strlen(".meta") + 1, sizeof(char));
    strcat(module->meta_path, path);
    strcat(module->meta_path, name);
    strcat(module->meta_path, ".meta");
    
    char *orig_dll = (char *)calloc(path_length + name_length + strlen(".dll") + 1, sizeof(char));
    strcat(orig_dll, path);
    strcat(orig_dll, name);
    strcat(orig_dll, ".dll");
    char *tmp_dll = (char *)calloc(path_length + name_length + strlen("_temp.dll") + 1, sizeof(char));
    strcat(tmp_dll, path);
    strcat(tmp_dll, name);
    strcat(tmp_dll, "_temp.dll");
    CopyFile(orig_dll, tmp_dll, FALSE);
    module->dll = LoadLibrary(tmp_dll);
    
    if(!module->dll) {
        SDL_LogError(0, "Unable to load module %s", name);
    }
    
    free(orig_dll);
    free(tmp_dll);
    
    module->last_write_time = Win32GetLastWriteTime(module->meta_path);
    
    SDL_Log("%s module loaded", name);
}

internal void Win32CloseModule(Module *module) {
    FreeLibrary(module->dll);
    free(module->meta_path);
}

int main (int argc, char *argv[]) {
    
    AllocConsole();
    
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    
    SDL_Window *window = SDL_CreateWindow("Password",  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_RECT.w, WINDOW_RECT.h, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    Module game_module = {};
    Win32LoadModule(&game_module, "game");
    GameLoadFunctions(&game_module);
    
    GameData game_data = {};
    
    bool running = true;
    SDL_Event event;
    float delta_time = 0;
    int last_time = SDL_GetTicks();
    
    pfn_GameInit(&game_data);
    
    while(running) {
        
        int time = SDL_GetTicks();
        delta_time = (float)(time - last_time) / 1000.f;
        last_time = time;
        
        if(Win32ShouldReloadModule(&game_module)) {
            Win32CloseModule(&game_module);
            Win32LoadModule(&game_module, "game");
            GameLoadFunctions(&game_module);
        }
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Log("Quit!");
                SDL_HideWindow(window);
                running = false;
            }
            pfn_GameInput(&event, &game_data);
        }
        pfn_GameUpdate(delta_time, &game_data);
        pfn_GameDraw(renderer, delta_time, &game_data);
    }
    
    pfn_GameClose(&game_data);
    
    Win32CloseModule(&game_module);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
    
}