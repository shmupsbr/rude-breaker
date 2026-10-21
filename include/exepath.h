/**
 * Directory of the running executable. Shared by asset lookup and the
 * optional YM2608 rhythm ROM search.
 */
#ifndef RUDEBREAKER_EXEPATH_H
#define RUDEBREAKER_EXEPATH_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Writes the directory with no trailing slash. Returns 0 on success. */
int rb_exe_dir(char *out, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_EXEPATH_H */
