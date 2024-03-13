#pragma once

#include <gui/modules/text_box.h>

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial_control.h>
#include <furi_hal_serial.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/modules/dialog_ex.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>

#include "../helpers/camera_suite_custom_event.h"

typedef void (*CameraSuiteViewWiFiCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewWiFiCamera {
    CameraSuiteViewWiFiCameraCallback callback;
    FuriStreamBuffer* wifi_rx_stream;
    FuriHalSerialHandle* wifi_serial_handle;
    FuriThread* wifi_worker_thread;
    NotificationApp* notification;
    View* view;
    void* context;
} CameraSuiteViewWiFiCamera;

typedef struct CameraSuiteViewWiFiCameraModel {
    FuriString* log;
} CameraSuiteViewWiFiCameraModel;

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc();
View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* camera_suite_static);
void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* camera_suite_static);
void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context);
