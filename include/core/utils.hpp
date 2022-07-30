#ifndef _UTILS_H
#define _UTILS_H

#include <cstdio>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif // _UTILS_H
