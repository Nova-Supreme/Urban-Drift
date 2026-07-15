#ifndef INTRO_H
#define INTRO_H

#include <raylib.h>
#include <stdbool.h>

#define INTRO_SLIDE_COUNT 9

typedef struct {
    Texture2D slides[INTRO_SLIDE_COUNT];
    const char* texts[INTRO_SLIDE_COUNT];
    int index;
    bool over;
} Intro;

void Intro_Load(Intro* intro);
void Intro_HandleInput(Intro* intro); // ESC skips, SPACE advances; unloads textures when it ends
void Intro_Draw(Intro* intro, int windowwidth, int windowheight);

#endif
