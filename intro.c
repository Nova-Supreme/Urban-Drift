#include "intro.h"
#include <math.h>

static void UnloadAllSlides(Intro* intro)
{
    for(int i=0;i<INTRO_SLIDE_COUNT;i++) UnloadTexture(intro->slides[i]);
}

void Intro_Load(Intro* intro)
{
    intro->slides[0]=LoadTexture("assets/story0.png");
    intro->slides[1]=LoadTexture("assets/story1.png");
    intro->slides[2]=LoadTexture("assets/story2.png");
    intro->slides[3]=LoadTexture("assets/story3.png");
    intro->slides[4]=LoadTexture("assets/story4.png");
    intro->slides[5]=LoadTexture("assets/story5.png");
    intro->slides[6]=LoadTexture("assets/story6.png");
    intro->slides[7]=LoadTexture("assets/story7.png");
    intro->slides[8]=LoadTexture("assets/story8.png");

    intro->texts[0]="He started with nothing but a rickshaw and a dream.";
    intro->texts[1]="But for that, he needed money...\n...lots of it.";
    intro->texts[2]="His ultimate desire was to have a sweet ride of his own";
    intro->texts[3]="The dream may one day be realized:\ncruising down the beach with Handsome Squidward by his side.";
    intro->texts[4]="A fateful encounter begins with a simple request,\nwith many more such encounters to come.";
    intro->texts[5]="Channelling the speed of a legend, he takes off";
    intro->texts[6]="Moving at impossible speeds, the streets ignite,\nleaving onlookers to whisper, \"Wallahi\".";
    intro->texts[7]="They arrive at the destination, leaving the passenger completely frozen.";
    intro->texts[8]="The fare is secured, and a relentless hunger awakens: \"I need more!\"";

    intro->index = 0;
    intro->over = false;
}

void Intro_HandleInput(Intro* intro)
{
    if(IsKeyPressed(KEY_ESCAPE)){
        intro->over = true;
        UnloadAllSlides(intro);
        return;
    }
    if(IsKeyPressed(KEY_SPACE)){
        intro->index++;
        if(intro->index >= INTRO_SLIDE_COUNT){
            intro->over = true;
            UnloadAllSlides(intro);
        }
    }
}

void Intro_Draw(Intro* intro, int windowwidth, int windowheight)
{
    if(intro->over) return;

    Texture2D currentslide = intro->slides[intro->index];
    float imageregionheight = windowheight*0.8f;
    float scale = fminf(windowwidth/(float)currentslide.width, imageregionheight/(float)currentslide.height);
    float imgw = currentslide.width*scale;
    float imgh = currentslide.height*scale;
    float imgx = (windowwidth-imgw)/2.0f;
    float imgy = 20.0f;

    DrawTexturePro(
        currentslide,
        (Rectangle){0,0,(float)currentslide.width,(float)currentslide.height},
        (Rectangle){imgx,imgy,imgw,imgh},
        (Vector2){0,0},
        0.0f,
        WHITE);

    int textwidth = MeasureText(intro->texts[intro->index],20);
    DrawText(intro->texts[intro->index],(windowwidth-textwidth)/2,windowheight*0.88f,20,WHITE);
    DrawText("Press SPACE to continue or ESC to skip", 20, windowheight-30, 16, GRAY);
}
