#include "utils.h"

int isValidIPAddress(const char* ipAddress)
{
  int segments = 0;
  char* segment;
  char* copy = strdup(ipAddress); // Make a copy of the input string
  char* saveptr = NULL; // Pointer to maintain the position between calls

  // Tokenize the string by dots
  segment = secure_strtok(copy, ".", &saveptr);
  while (segment != NULL)
  {
    segments++;
    // Check if the segment is a valid integer in the range [0, 255]
    for (int i = 0; i < strlen(segment); i++)
    {
      if (!isdigit(segment[i]))
      {
        free(copy); // Free the memory allocated for the copy
        return 0; // Not a valid IP address
      }
    }
    int value = atoi(segment);
    if (value < 0 || value > 255)
    {
      free(copy); // Free the memory allocated for the copy
      return 0; // Not a valid IP address
    }
    segment = secure_strtok(NULL, ".", &saveptr);
  }

  free(copy); // Free the memory allocated for the copy

  // Ensure there are exactly 4 segments
  if (segments != 4)
  {
    return 0; // Not a valid IP address
  }

  return 1; // Valid IP address
}

int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...)
{
  if (buffer == NULL || bufferSize == 0 || format == NULL)
  {
    // Invalid parameters
    return -1;
  }

  va_list args;
  va_start(args, format);

  // Print to a temporary buffer to determine the required size
  char tempBuffer[1024]; // Choose a suitable size for the temporary buffer
  int result = vsnprintf(tempBuffer, sizeof(tempBuffer), format, args);

  va_end(args);

  if (result < 0)
  {
    // Error occurred
    return result;
  }

  if ((size_t)result >= bufferSize)
  {
    // Output buffer is too small
    return -1;
  }

  // Copy the formatted string to the output buffer
  va_start(args, format);
  vsnprintf(buffer, bufferSize, format, args);
  va_end(args);

  return result;
}

void secure_strcpy(char* dest, size_t destSize, const char* src)
{
  if (dest == NULL || src == NULL || destSize == 0)
  {
    // Invalid parameters
    return;
  }

  size_t i = 0;
  // Copy characters from src to dest until either the null terminator is reached
  // or the maximum size of dest is reached
  while (i < destSize - 1 && src[i] != '\0')
  {
    dest[i] = src[i];
    i++;
  }

  // Ensure null termination of dest
  dest[i] = '\0';
}

void secure_strncpy(char* dest, size_t destSize, const char* src, size_t count)
{
  if (dest == NULL || src == NULL || destSize == 0)
  {
    // Invalid parameters
    return;
  }

  size_t i = 0;
  // Copy characters from src to dest until either the null terminator is reached
  // or the maximum size of dest is reached, or count characters are copied
  while (i < destSize - 1 && src[i] != '\0' && i < count)
  {
    dest[i] = src[i];
    i++;
  }

  // Pad the destination buffer with null characters if necessary
  while (i < destSize - 1 && i < count)
  {
    dest[i] = '\0';
    i++;
  }

  // Ensure null termination of dest
  dest[i] = '\0';
}

char* secure_strtok(char* str, const char* delim, char** saveptr)
{
  if (str == NULL && *saveptr == NULL)
  {
    return NULL;
  }

  if (str != NULL)
  {
    *saveptr = str;
  }

  // Skip leading delimiters
  while (**saveptr && strchr(delim, **saveptr))
  {
    (*saveptr)++;
  }

  if (**saveptr == '\0')
  {
    *saveptr = NULL;
    return NULL;
  }

  char* token_start = *saveptr;

  // Find the end of the token
  while (**saveptr && !strchr(delim, **saveptr))
  {
    (*saveptr)++;
  }

  if (**saveptr != '\0')
  {
    **saveptr = '\0';
    (*saveptr)++;
  }
  else
  {
    *saveptr = NULL;
  }

  return token_start;
}

int isLight(SDL_Color c) {
  double r = 0.299f * (c.r * c.r);
  double g = 0.587f * (c.g * c.g);
  double b = 0.114f * (c.b * c.b);

  double hsp = sqrt(r + g + b);

  return hsp > 127.5f;
}

unsigned int hexFromRGB(int r, int g, int b)
{
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

SDL_Color getRandomColor()
{
  SDL_Color color;

  color.r = rand() % 255;
  color.g = rand() % 255;
  color.b = rand() % 255,
  color.a = 255;

  return color;
}

