#ifndef RUDEBREAKER_PLAYER_H
#define RUDEBREAKER_PLAYER_H

#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void player_reset(RbPlayer *p);
void player_update(RbPlayer *p, const RbInput *in);
void player_render(const RbPlayer *p);

#ifdef __cplusplus
}
#endif

#endif /* RUDEBREAKER_PLAYER_H */
