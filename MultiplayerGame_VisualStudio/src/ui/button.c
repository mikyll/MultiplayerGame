#include "button.h"

Button createButton(
  int posX, int posY, int width, int height, SDL_Color color,
  char* text, SDL_Color textColor,
  void (*onOver)(void), void (*onOut)(void),
  void (*onPress)(void), void (*onRelease)(void));
void updateButtonText(Button* button, const char* format, ...);
void updateButtonColor(Button* button, SDL_Color background, SDL_Color text);
void drawButton(Button button);

Button createButton(
  int posX, int posY, int width, int height, SDL_Color color,
  char* text, SDL_Color textColor,
  void (*onOver)(void), void (*onOut)(void),
  void (*onPress)(void), void (*onRelease)(void))
{
  Button button = { 0 };

  button.x = posX;
  button.y = posY;
  button.w = width;
  button.h = height;
  button.c = color;
  if (text != NULL && strlen(text) > 0)
  {
    secure_strcpy(button.text, sizeof(button.text), text);
    button.textC = textColor;
  }
  button.onOver = onOver;
  button.onOut = onOut;
  button.onPress = onPress;
  button.onRelease = onRelease;

  return button;
}

void updateButtonText(Button* button, const char* format, ...)
{
  char buffer[MAX_BUTTON_TEXT_LENGTH];
  va_list args;

  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  secure_strcpy(button->text, sizeof(button->text), buffer);
}

void updateButtonColor(Button* button, SDL_Color background, SDL_Color text)
{
  button->c = background;
  button->textC = text;
}

void drawButton(Button button)
{
  blitRect(button.x, button.y, button.w, button.h, button.c);
  drawText(
    button.x + button.w / 2,
    button.y + button.h / 2 - GLYPH_HEIGHT / 2,
    button.textC.r,
    button.textC.g,
    button.textC.b,
    TEXT_CENTER,
    button.text
  );
}
