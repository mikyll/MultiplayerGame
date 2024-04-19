#include "utils.h"

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