/**
 * Locate the directory that contains this executable.
 * Windows, macOS, and Linux each have their own API. Other targets fail
 * so the caller can fall back to a relative data path.
 */
#define _POSIX_C_SOURCE 200809L

#include "exepath.h"

#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

int rb_exe_dir(char *out, size_t n)
{
    char buf[1024];
    char *slash;

    if (!out || n == 0) {
        return -1;
    }
    buf[0] = 0;

#if defined(_WIN32)
    {
        DWORD got = GetModuleFileNameA(NULL, buf, (DWORD)sizeof(buf));

        if (got == 0 || got >= (DWORD)sizeof(buf)) {
            return -1;
        }
    }
    {
        char *p;
        for (p = buf; *p; p++) {
            if (*p == '\\') {
                *p = '/';
            }
        }
    }
#elif defined(__APPLE__)
    {
        uint32_t sz = (uint32_t)sizeof(buf);

        if (_NSGetExecutablePath(buf, &sz) != 0) {
            return -1;
        }
    }
#elif defined(__linux__)
    {
        ssize_t got = readlink("/proc/self/exe", buf, sizeof(buf) - 1u);

        if (got <= 0 || (size_t)got >= sizeof(buf)) {
            return -1;
        }
        buf[got] = 0;
    }
#else
    return -1;
#endif

    slash = strrchr(buf, '/');
    if (!slash || slash == buf) {
        return -1;
    }
    *slash = 0;
    if (strlen(buf) + 1u > n) {
        return -1;
    }
    memcpy(out, buf, strlen(buf) + 1u);
    return 0;
}
