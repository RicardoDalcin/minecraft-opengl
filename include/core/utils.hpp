#ifndef _UTILS_H
#define _UTILS_H

#include <cstdio>

#include "core.h"

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif
