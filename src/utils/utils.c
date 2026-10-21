#include "utils.h"

int isValidIPAddress(const char* ipAddress)
{
	const char* c = ipAddress;

	if (ipAddress == NULL)
		return 0;

	// Expect exactly 4 segments of 1-3 digits in [0, 255], separated by single dots
	for (int segment = 0; segment < 4; segment++)
	{
		int digits = 0, value = 0;

		if (segment > 0)
		{
			if (*c != '.')
				return 0;
			c++;
		}

		while (digits < 3 && isdigit((unsigned char)*c))
		{
			value = value * 10 + (*c - '0');
			digits++;
			c++;
		}

		// Empty segment (e.g. "1..2.3.4") or value out of range
		if (digits == 0 || value > 255)
			return 0;
	}

	// Nothing may follow the last segment (e.g. "1.2.3.4." or "1.2.3.4567")
	return *c == '\0';
}

int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...)
{
	va_list args;
	int result;

	va_start(args, format);
	result = secure_vsprintf(buffer, bufferSize, format, args);
	va_end(args);

	return result;
}

int secure_vsprintf(char* buffer, size_t bufferSize, const char* format, va_list args)
{
	va_list argsCopy;
	int result;

	if (buffer == NULL || bufferSize == 0 || format == NULL)
	{
		// Invalid parameters
		return -1;
	}

	// Determine the required size without writing to the output buffer
	va_copy(argsCopy, args);
	result = vsnprintf(NULL, 0, format, argsCopy);
	va_end(argsCopy);

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

	// Write the formatted string to the output buffer
	return vsnprintf(buffer, bufferSize, format, args);
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

