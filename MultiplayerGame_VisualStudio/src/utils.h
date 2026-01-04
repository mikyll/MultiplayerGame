#pragma once

#include "common.h"

int isValidIPAddress(const char* ipAddress);
int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);
void secure_strcpy(char* dest, size_t destSize, const char* src);
void secure_strncpy(char* dest, size_t destSize, const char* src, size_t count);
char* secure_strtok(char* str, const char* delim, char** saveptr);
