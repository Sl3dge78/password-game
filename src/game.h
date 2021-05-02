/* date = May 1st 2021 0:58 am */

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#define global static 
#define internal static 
#define dll_export extern "C" __declspec(dllexport) 

typedef uint32_t u32;

global const SDL_Rect WINDOW_RECT = {0, 0, 1280, 720};
global const SDL_Color GREEN = {20,255,20,255};
global const SDL_Color RED = {255,20,20,255};

#define DECL_TEST(name) bool name##(char *buffer, const char value)
typedef DECL_TEST(fn_ConditionTest);

#define DECL_DRAW(name) void name##(SDL_Renderer *renderer, TTF_Font *font, const u32 x, const u32 y, const char value, const bool v)
typedef DECL_DRAW(fn_ConditionDraw);

struct TestCase {
    fn_ConditionTest *test;
    fn_ConditionDraw *draw;
};

#define LEVEL_COUNT 10
#ifdef DEBUG
#define STARTING_LEVEL 0
#else
#define STARTING_LEVEL 0
#endif

struct Level {
    u32 condition_count;
    u32 *test_values;
    TestCase *tests;
};

fn_ConditionTest ConditionLengthTest;
fn_ConditionDraw ConditionLengthDraw;
const TestCase ConditionLength = {ConditionLengthTest, ConditionLengthDraw};

#define DECL_TEST_CASE(name) \
fn_ConditionTest ConditionTest##name; \
fn_ConditionDraw ConditionDraw##name; \
const TestCase Condition##name = {ConditionTest##name, ConditionDraw##name};

DECL_TEST_CASE(Caps)
DECL_TEST_CASE(LengthLower)
DECL_TEST_CASE(Repeating)
DECL_TEST_CASE(Different)
DECL_TEST_CASE(Lowercase)
DECL_TEST_CASE(StartsWith)
DECL_TEST_CASE(EndsWith)
DECL_TEST_CASE(EqEveryOther)
DECL_TEST_CASE(Contains)

struct GameData {
    float caret_timer;
    TTF_Font *font;
    
    char *buffer;
    char *caret;
    
    u32 current_level;
    bool *is_condition_met;
    bool all_conditions_met;
    Level *all_levels;
    const Level *level;
};

enum Alignement {
    V_CENTERED = 1,
    H_CENTERED = 2,
    LEFT = 4,
    RIGHT = 8,
    TOP = 16,
    BOTTOM = 32
};

typedef u32 AlignementFlags;


typedef void fn_GameInit(GameData *game_data);
fn_GameInit *pfn_GameInit;

typedef void fn_GameInput(SDL_Event *event, GameData *game_data);
fn_GameInput *pfn_GameInput;

typedef void fn_GameUpdate(float delta_time, GameData *game_data);
fn_GameUpdate *pfn_GameUpdate;

typedef void fn_GameDraw(SDL_Renderer* renderer, float delta_time, GameData *game_data);
fn_GameDraw *pfn_GameDraw;

typedef void fn_GameClose(GameData *game_data);
fn_GameClose *pfn_GameClose;

#endif //GAME_H
