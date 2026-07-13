#include "pausemenu.h"

void DrawPauseMenu(int gamewidth, int gameheight, Rectangle resumeButton) {
    DrawRectangle(0, 0, gamewidth, gameheight, Fade(BLACK, 0.5f));

    const char* pausedText = "PAUSED";
    int fontSize = 60;
    int textWidth = MeasureText(pausedText, fontSize);
    DrawText(pausedText, gamewidth/2 - textWidth/2, gameheight/2 - 100, fontSize, WHITE);

    DrawRectangleRec(resumeButton, DARKGRAY);
    DrawRectangleLinesEx(resumeButton, 2, WHITE);
    const char* resumeText = "Resume";
    int rFontSize = 28;
    int rTextWidth = MeasureText(resumeText, rFontSize);
    DrawText(resumeText,
        (int)(resumeButton.x + resumeButton.width/2 - rTextWidth/2),
        (int)(resumeButton.y + resumeButton.height/2 - rFontSize/2),
        rFontSize, WHITE);
}

bool IsResumeButtonClicked(Rectangle resumeButton, float mouseX, float mouseY, float viewX, float viewY, float scale) {
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return false;
    Vector2 gameMouse = { (mouseX - viewX) / scale, (mouseY - viewY) / scale };
    return CheckCollisionPointRec(gameMouse, resumeButton);
}
