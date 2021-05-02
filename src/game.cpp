#include "game.h"

#include "utf8.cpp"

void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, u32 x, u32 y, SDL_Color color) {
    SDL_Surface *surf = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Rect rect = {(int)x, (int)y, surf->w, surf->h};
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

bool ConditionLengthTest(char *buffer, const char value) {
    u32 length = strlen(buffer);
    u32 nb = (u32)value;
    return(length > nb);
}

void ConditionLengthDraw(SDL_Renderer *renderer, TTF_Font *font, const u32 x, const u32 y, const char value, const bool v) {
    char string[100];
    u32 nb = (u32)value;
    sprintf(string, "- Password must be at least %d characters long.", nb);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestCaps) {
    u32 count = 0;
    for(u32 i = 0; buffer[i] != '\0'; ++i) {
        if(IsUpper(buffer[i])) {
            count++;
        }
    }
    u32 nb = (u32)value;
    return count >= nb;
}

DECL_DRAW(ConditionDrawCaps) {
    char string[100];
    sprintf(string, "- Password must have at least %d capital letters.", (u32)value);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestLowercase) {
    u32 count = 0;
    for(u32 i = 0; buffer[i] != '\0'; ++i) {
        if(!IsUpper(buffer[i])) {
            count++;
        }
    }
    u32 nb = (u32)value;
    return count >= nb;
}

DECL_DRAW(ConditionDrawLowercase) {
    char string[100];
    u32 nb = (u32)value;
    sprintf(string, "- Password must have at least %d lowercase letters.", nb);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestLengthLower) {
    u32 nb = (u32)value;
    return (strlen(buffer) < nb);
}

DECL_DRAW(ConditionDrawLengthLower) {
    char string[100];
    u32 nb = (u32)value;
    sprintf(string, "- Password must have less than %d characters.", nb);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestRepeating) {
    char previous_letter = '\0';
    for(u32 i = 0; buffer[i] != '\0'; ++i) {
        if(previous_letter == buffer[i])
            return false;
        previous_letter = buffer[i];
    }
    return true;
}

DECL_DRAW(ConditionDrawRepeating) {
    char string[100];
    sprintf(string, "- Password must not have the same characters in succession.");
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestDifferent) {
    char previous_letter = '\0';
    for(u32 i = 0; buffer[i] != '\0'; ++i) {
        for(u32 j = i+1; buffer[j] != '\0'; ++j) {
            if(buffer[j] == buffer[i]) {
                return false;
            }
        }
    }
    return true;
}

DECL_DRAW(ConditionDrawDifferent) {
    char string[100];
    sprintf(string, "- Password must have unique letters.");
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestStartsWith) {
    char c = value;
    return buffer[0] == c;
}
DECL_DRAW(ConditionDrawStartsWith) {
    char c = value;
    char string[100];
    sprintf(string, "- Password must start with %c.", c);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestEndsWith) {
    char c = value;
    u32 length = strlen(buffer);
    if(length == 0)
        return false;
    
    return buffer[length-1] == c;
}
DECL_DRAW(ConditionDrawEndsWith) {
    char c = value;
    char string[100];
    sprintf(string, "- Password must end with %c.", c);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

DECL_TEST(ConditionTestEqEveryOther) {
    char c = value;
    u32 length = strlen(buffer);
    if(length == 0)
        return false;
    
    for(u32 i = 0; i < length; i++) {
        if(i+2 >= length)
            continue;
        if(buffer[i+2] != buffer[i])
            return false;
    }
    
    return true;
}
DECL_DRAW(ConditionDrawEqEveryOther) {
    char c = value;
    char string[100];
    sprintf(string, "- Every other letter must be the same.");
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}
DECL_TEST(ConditionTestContains) {
    char c = value;
    for(u32 i = 0; buffer[i] != '\0'; ++i) {
        if(buffer[i] == c)
            return true;
    }
    return false;
}
DECL_DRAW(ConditionDrawContains) {
    char c = value;
    char string[100];
    sprintf(string, "- Password must have at least 1 %c.", c);
    DrawText(renderer, font, string, x, y, v ? GREEN : RED);
}

inline SDL_Rect GetAlignedRect(SDL_Rect area, u32 width, u32 height, AlignementFlags alignement, u32 margin) {
    SDL_Rect result = {};
    if(alignement & H_CENTERED) {
        result.x = area.x + area.w / 2 - width / 2;
    } else if(alignement & RIGHT) {
        result.x = area.x + area.w - width - margin;
    } else if(alignement & LEFT) {
        result.x = area.x + margin;
    }
    if(alignement & V_CENTERED) {
        result.y = area.y + area.h / 2 - height / 2;
    } else if(alignement & TOP) {
        result.y = area.y + margin;
    } else if(alignement & BOTTOM) {
        result.y = area.y + area.h - height - margin;
    }
    
    result.w = width;
    result.h = height;
    return result;
}

void LoadLevel(GameData *game_data) {
    free(game_data->is_condition_met);
    if(game_data->current_level + 1 <= LEVEL_COUNT) {
        game_data->level++;
    }
    game_data->is_condition_met = (bool*) calloc(game_data->level[game_data->current_level].condition_count, sizeof(bool));
    *game_data->buffer = '\0';
    game_data->caret = game_data->buffer;
}

void CheckPassword(GameData *game_data) {
    const Level *level = game_data->level;
    for(u32 i = 0; i < level->condition_count; ++i) {
        game_data->is_condition_met[i] = level->tests[i].test(game_data->buffer, level->test_values[i]);
    }
    game_data->all_conditions_met = true;
    for(u32 i = 0; i < game_data->level->condition_count; ++i){
        if(!game_data->is_condition_met[i]) {
            game_data->all_conditions_met = false;
            break;
        }
    }
}

Level *LoadAllLevels () {
    
    Level *all_levels = (Level*) calloc(LEVEL_COUNT, sizeof(Level));
    
    all_levels[0].condition_count = 1;
    all_levels[0].tests = (TestCase *) calloc(1, sizeof(TestCase));
    all_levels[0].test_values = (u32 *) calloc(1, sizeof(u32));
    all_levels[0].test_values[0] = 5;
    all_levels[0].tests[0] = ConditionLength;
    
    all_levels[1].condition_count = 2;
    all_levels[1].tests = (TestCase *) calloc(2, sizeof(TestCase));
    all_levels[1].test_values = (u32 *) calloc(2, sizeof(u32));
    all_levels[1].test_values[0] = 10;
    all_levels[1].tests[0] = ConditionLength;
    all_levels[1].test_values[1] = 1;
    all_levels[1].tests[1] = ConditionCaps;
    
    all_levels[2].condition_count = 3;
    all_levels[2].tests = (TestCase *) calloc(3, sizeof(TestCase));
    all_levels[2].test_values = (u32 *) calloc(3, sizeof(u32));
    all_levels[2].test_values[0] = 2;
    all_levels[2].tests[0] = ConditionLength;
    all_levels[2].test_values[1] = 5;
    all_levels[2].tests[1] = ConditionLengthLower;
    all_levels[2].test_values[2] = 1;
    all_levels[2].tests[2] = ConditionCaps;
    
    all_levels[3].condition_count = 3;
    all_levels[3].tests = (TestCase *) calloc(3, sizeof(TestCase));
    all_levels[3].test_values = (u32 *) calloc(3, sizeof(u32));
    all_levels[3].test_values[0] = 2;
    all_levels[3].tests[0] = ConditionLength;
    all_levels[3].test_values[1] = 0;
    all_levels[3].tests[1] = ConditionRepeating;
    all_levels[3].test_values[2] = 1;
    all_levels[3].tests[2] = ConditionCaps;
    
    all_levels[4].condition_count = 4;
    all_levels[4].tests = (TestCase *) calloc(4, sizeof(TestCase));
    all_levels[4].test_values = (u32 *) calloc(4, sizeof(u32));
    all_levels[4].test_values[0] = 2;
    all_levels[4].tests[0] = ConditionDifferent;
    all_levels[4].test_values[1] = 2;
    all_levels[4].tests[1] = ConditionLowercase;
    all_levels[4].test_values[2] = 3;
    all_levels[4].tests[2] = ConditionCaps;
    all_levels[4].test_values[3] = 6;
    all_levels[4].tests[3] = ConditionLengthLower;
    
    all_levels[5].condition_count = 5;
    all_levels[5].tests = (TestCase *) calloc(5, sizeof(TestCase));
    all_levels[5].test_values = (u32 *) calloc(5, sizeof(u32));
    all_levels[5].test_values[0] = 2;
    all_levels[5].tests[0] = ConditionDifferent;
    all_levels[5].test_values[1] = 2;
    all_levels[5].tests[1] = ConditionLowercase;
    all_levels[5].test_values[2] = 3;
    all_levels[5].tests[2] = ConditionCaps;
    all_levels[5].test_values[3] = 6;
    all_levels[5].tests[3] = ConditionLengthLower;
    all_levels[5].test_values[4] = 'C';
    all_levels[5].tests[4] = ConditionStartsWith;
    
    all_levels[6].condition_count = 6;
    all_levels[6].tests = (TestCase *) calloc(6, sizeof(TestCase));
    all_levels[6].test_values = (u32 *) calloc(6, sizeof(u32));
    all_levels[6].test_values[0] = 2;
    all_levels[6].tests[0] = ConditionDifferent;
    all_levels[6].test_values[1] = 3;
    all_levels[6].tests[1] = ConditionLowercase;
    all_levels[6].test_values[2] = 2;
    all_levels[6].tests[2] = ConditionCaps;
    all_levels[6].test_values[3] = 6;
    all_levels[6].tests[3] = ConditionLengthLower;
    all_levels[6].test_values[4] = 'C';
    all_levels[6].tests[4] = ConditionStartsWith;
    all_levels[6].test_values[5] = 'T';
    all_levels[6].tests[5] = ConditionEndsWith;
    
    all_levels[7].condition_count = 5;
    all_levels[7].tests = (TestCase *) calloc(5, sizeof(TestCase));
    all_levels[7].test_values = (u32 *) calloc(5, sizeof(u32));
    all_levels[7].test_values[0] = 2;
    all_levels[7].tests[0] = ConditionEqEveryOther;
    all_levels[7].test_values[1] = 3;
    all_levels[7].tests[1] = ConditionLowercase;
    all_levels[7].test_values[2] = 2;
    all_levels[7].tests[2] = ConditionCaps;
    all_levels[7].test_values[3] = 6;
    all_levels[7].tests[3] = ConditionLengthLower;
    all_levels[7].test_values[4] = 'a';
    all_levels[7].tests[4] = ConditionStartsWith;
    
    all_levels[8].condition_count = 5;
    all_levels[8].tests = (TestCase *) calloc(5, sizeof(TestCase));
    all_levels[8].test_values = (u32 *) calloc(5, sizeof(u32));
    all_levels[8].test_values[0] = 'H';
    all_levels[8].tests[0] = ConditionStartsWith;
    all_levels[8].test_values[1] = 'o';
    all_levels[8].tests[1] = ConditionEndsWith;
    all_levels[8].test_values[2] = 'e';
    all_levels[8].tests[2] = ConditionContains;
    all_levels[8].test_values[3] = 3;
    all_levels[8].tests[3] = ConditionLowercase;
    all_levels[8].test_values[4] = 6;
    all_levels[8].tests[4] = ConditionLengthLower;
    
    all_levels[9].condition_count = 6;
    all_levels[9].tests = (TestCase *) calloc(6, sizeof(TestCase));
    all_levels[9].test_values = (u32 *) calloc(6, sizeof(u32));
    all_levels[9].test_values[0] = 'W';
    all_levels[9].tests[0] = ConditionStartsWith;
    all_levels[9].test_values[1] = 'd';
    all_levels[9].tests[1] = ConditionEndsWith;
    all_levels[9].test_values[2] = 'r';
    all_levels[9].tests[2] = ConditionContains;
    all_levels[9].test_values[3] = 3;
    all_levels[9].tests[3] = ConditionLowercase;
    all_levels[9].test_values[4] = 6;
    all_levels[9].tests[4] = ConditionLengthLower;
    all_levels[9].test_values[5] = 6;
    all_levels[9].tests[5] = ConditionRepeating;
    
    return all_levels;
}

void DestroyAllLevels(Level *all_levels) {
    for(u32 i = 0; i < LEVEL_COUNT; ++i ) {
        free(all_levels[i].test_values);
        free(all_levels[i].tests);
    }
    
    free(all_levels);
}

dll_export void Init(GameData *game_data) {
    game_data->font = TTF_OpenFont("resources/fonts/unifont.ttf", 15);
    game_data->buffer = (char *) calloc(32, sizeof(char));
    game_data->caret = game_data->buffer;
    
    game_data->all_levels = LoadAllLevels();
    game_data->current_level = STARTING_LEVEL;
    game_data->level = &game_data->all_levels[game_data->current_level];
    game_data->is_condition_met = (bool*) calloc(game_data->level[game_data->current_level].condition_count, sizeof(bool));
    
}

dll_export void Close(GameData *game_data) {
    free(game_data->buffer);
    free(game_data->is_condition_met);
    DestroyAllLevels(game_data->all_levels);
    TTF_CloseFont(game_data->font);
}

dll_export void Input(SDL_Event *event, GameData *game_data) {
    if (event->type == SDL_TEXTINPUT) {
        strcat(game_data->buffer, event->text.text);
        game_data->caret = strchr(game_data->buffer, '\0');
        CheckPassword(game_data);
    }
    if(event->type == SDL_KEYDOWN) {
        if(event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
            if(game_data->caret > game_data->buffer) {
                game_data->caret --; // Caret is on \0 so move it back one to see
                while(!IsLeadingByte(*game_data->caret)) {
                    *game_data->caret = '\0';
                    game_data->caret--;
                }
                *game_data->caret = '\0';
                CheckPassword(game_data);
            }
        }
        if(event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
            if(game_data->all_conditions_met) {
                game_data->current_level++;
                LoadLevel(game_data);
            }
        }
#ifdef DEBUG
        if(event->key.keysym.scancode == SDL_SCANCODE_F4) {
            free(game_data->is_condition_met);
            DestroyAllLevels(game_data->all_levels);
            
            game_data->all_levels = LoadAllLevels();
            game_data->level = &game_data->all_levels[game_data->current_level];
            game_data->is_condition_met = (bool*) calloc(game_data->level[game_data->current_level].condition_count, sizeof(bool));
        }
        if(event->key.keysym.scancode == SDL_SCANCODE_KP_MINUS) {
            if(game_data->current_level-- > 0) 
                game_data->current_level--;
            LoadLevel(game_data);
        }
        if(event->key.keysym.scancode == SDL_SCANCODE_KP_PLUS) {
            game_data->current_level++;
            LoadLevel(game_data);
        }
#endif
    }
}

dll_export void Update(float delta_time, GameData *game_data) {
    game_data->caret_timer += delta_time;
    if(game_data->caret_timer > 2.0f) {
        game_data->caret_timer = 0.0f;
    }
}

void DrawWinConditions(SDL_Renderer *renderer, GameData *game_data, const u32 x, u32 y) {
    
    for(u32 i = 0; i < game_data->level->condition_count; ++i) {
        game_data->level->tests[i].draw(renderer, game_data->font, x, y, game_data->level->test_values[i], game_data->is_condition_met[i]);
        y += 15;
    }
}

dll_export void Draw(SDL_Renderer* renderer, float delta_time, GameData *game_data) {
    SDL_SetRenderDrawColor(renderer, 20,20,20,255);
    SDL_RenderFillRect(renderer, &WINDOW_RECT);
    if(game_data->current_level < LEVEL_COUNT) {
        char level[64];
        sprintf(level, "Level: %d", game_data->current_level+1);
        
        DrawText(renderer, game_data->font, level, 10, 10, {255,255,255,255});
        
        // Input bar
        SDL_SetRenderDrawColor(renderer, 250,250,250,255);
        SDL_Rect input_bar =  GetAlignedRect(WINDOW_RECT, WINDOW_RECT.w / 2, 30, V_CENTERED | H_CENTERED, 0);
        SDL_RenderDrawRect(renderer, &input_bar);
        
        DrawText(renderer, game_data->font, "Please enter a secure password:", input_bar.x, input_bar.y - 18, SDL_Color{255,255,255,255});
        
        // Input text
        SDL_Rect text_rect = input_bar;
        if(strlen(game_data->buffer) > 0) {
            SDL_Surface *surf = TTF_RenderUTF8_Solid(game_data->font, game_data->buffer, SDL_Color{255,255,255,255});
            text_rect = GetAlignedRect(input_bar, surf->w, surf->h, LEFT | V_CENTERED, 5);
            SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            SDL_RenderCopy(renderer, text, NULL, &text_rect);
        } else {
            text_rect.w = 0;
        }
        // Caret
        SDL_Rect caret = input_bar;
        caret.x = text_rect.x + text_rect.w;
        caret.y += 15/2;
        caret.w = 2;
        caret.h = 15;
        if(game_data->caret_timer <= 1.0f)
            SDL_RenderDrawRect(renderer, &caret);
        
        DrawWinConditions(renderer, game_data, input_bar.x, input_bar.y + input_bar.h + 5);
    } else {
        SDL_Rect center = GetAlignedRect(WINDOW_RECT, WINDOW_RECT.w / 2, 30, V_CENTERED|H_CENTERED, 0);
        DrawText(renderer,game_data->font, "Thank you for playing!", center.x, center.y, SDL_Color{255,255,255,255});
    }
    SDL_RenderPresent(renderer);
}

