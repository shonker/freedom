#include "hitobject.h"

BeatmapData current_beatmap;
Scene current_scene = Scene::MAIN_MENU;

bool start_parse_beatmap = false;
bool target_first_circle = true;

float fraction_modifier = 0.04f;

void process_hitobject()
{
    if (start_parse_beatmap)
    {
        parse_beatmap(osu_manager_ptr, current_beatmap);
        target_first_circle = true;
        start_parse_beatmap = false;
    }

    static double keydown_time = 0.0;
    static double keyup_delay = 0.0;
    static float fraction_of_the_distance = 0.0f;
    static Vector2 direction(0.0f, 0.0f);
    static Vector2 mouse_position(0.0f, 0.0f);
    if ((cfg_relax_lock || cfg_aimbot_lock) && current_scene == Scene::GAMIN && current_beatmap.ready)
    {
        double current_time = ImGui::GetTime();
        int32_t audio_time = *(int32_t *)audio_time_ptr;
        Circle& circle = current_beatmap.current_circle();
        if (cfg_aimbot_lock)
        {
            if (fraction_of_the_distance)
            {
                if (fraction_of_the_distance > 1.0f)
                {
                    fraction_of_the_distance = 0.0f;
                }
                else
                {
                    Vector2 next_mouse_position = mouse_position + direction * fraction_of_the_distance;
                    move_mouse_to(next_mouse_position.x, next_mouse_position.y);
                    fraction_of_the_distance += fraction_modifier;
                }
            }
            if (target_first_circle)
            {
                direction = prepare_hitcircle_target(osu_manager_ptr, circle.position, mouse_position);
                fraction_of_the_distance = fraction_modifier;
                target_first_circle = false;
            }
        }
        if (audio_time >= circle.start_time)
        {
            if (cfg_aimbot_lock)
            {
                if (circle.type == HitObjectType::Slider)
                {
                    if (circle.curves.size() == 2)
                    {
                        direction = prepare_hitcircle_target(osu_manager_ptr, circle.curves[1], mouse_position);
                        fraction_of_the_distance = fraction_modifier;
                    }
                    else
                    {
                        static int32_t prev_audio_time = audio_time;
                        int32_t circle_time = circle.end_time - circle.start_time;
                        if ((audio_time - prev_audio_time) >= (circle_time / circle.curves.size()))
                        {
                            if (circle.curve_idx < circle.curves.size())
                            {
                                direction = prepare_hitcircle_target(osu_manager_ptr, circle.curves[circle.curve_idx++], mouse_position);
                                fraction_of_the_distance = fraction_modifier;
                                prev_audio_time = audio_time;
                            }
                        }
                    }
                }
            }
            if (cfg_relax_lock && !circle.clicked)
            {
                send_keyboard_input(left_click[0], 0);
                FR_INFO_FMT("hit %d!, %d %d", current_beatmap.hit_object_idx, circle.start_time, circle.end_time);
                keyup_delay = circle.end_time ? circle.end_time - circle.start_time : 0.5;
                if (circle.type == HitObjectType::Slider || circle.type == HitObjectType::Spinner)
                {
                    if (current_beatmap.mods & Mods::DoubleTime)
                        keyup_delay /= 1.5;
                    else if (current_beatmap.mods & Mods::HalfTime)
                        keyup_delay /= 0.75;
                }
                keydown_time = ImGui::GetTime();
                circle.clicked = true;
            }
        }
        if (audio_time >= circle.end_time)
        {
            current_beatmap.hit_object_idx++;
            if (current_beatmap.hit_object_idx >= current_beatmap.hit_objects.size())
            {
                current_beatmap.ready = false;
            }
            else if (cfg_aimbot_lock)
            {
                Circle& next_circle = current_beatmap.current_circle();
                if (next_circle.type == HitObjectType::Circle)
                {
                    direction = prepare_hitcircle_target(osu_manager_ptr, next_circle.position, mouse_position);
                    fraction_of_the_distance = fraction_modifier;
                }
                if (next_circle.type == HitObjectType::Slider)
                {
                    direction = prepare_hitcircle_target(osu_manager_ptr, next_circle.position, mouse_position);
                    fraction_of_the_distance = fraction_modifier;
                }
            }
        }
    }
    if (cfg_relax_lock && keydown_time && ((ImGui::GetTime() - keydown_time) * 1000.0 > keyup_delay))
    {
        keydown_time = 0.0;
        send_keyboard_input(left_click[0], KEYEVENTF_KEYUP);
    }
}