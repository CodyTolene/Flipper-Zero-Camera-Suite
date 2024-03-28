#pragma once

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
View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* instance);
void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance);
void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* instance,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context);
