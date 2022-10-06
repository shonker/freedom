#pragma once

typedef void (*HR_UPDATE_UI)(void);
typedef void (*HR_SET_IMGUI_CONTEXT)(void *ctx);

HR_UPDATE_UI hr_update_ui;
HR_SET_IMGUI_CONTEXT hr_set_imgui_context;
