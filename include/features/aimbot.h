#pragma once

#include "vector.h"
#include "config.h"

void aimbot_on_beatmap_load();
void update_aimbot(Circle &circle, const int32_t audio_time);
void aimbot_on_advance_hit_object();
