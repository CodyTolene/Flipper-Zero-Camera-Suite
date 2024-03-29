#pragma once

#include <gui/view.h>
#include "../helpers/camera_suite_custom_event.h"

typedef void (*CameraSuiteViewStartCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewStart {
    View* view;
    CameraSuiteViewStartCallback callback;
    void* context;
} CameraSuiteViewStart;

typedef struct {
    int some_value;
} CameraSuiteViewStartModel;

void camera_suite_view_start_set_callback(
    CameraSuiteViewStart* camera_suite_view_start,
    CameraSuiteViewStartCallback callback,
    void* context);

View* camera_suite_view_start_get_view(CameraSuiteViewStart* camera_suite_static);

CameraSuiteViewStart* camera_suite_view_start_alloc();

void camera_suite_view_start_free(CameraSuiteViewStart* camera_suite_static);
