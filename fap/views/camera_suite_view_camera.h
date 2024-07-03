#pragma once

#define BITMAP_HEADER_LENGTH 62
#define FRAME_BIT_DEPTH 1
#define FRAME_BUFFER_LENGTH 1024
#define FRAME_HEIGHT 64
#define FRAME_WIDTH 128
#define HEADER_LENGTH 3 // 'Y', ':', and row identifier
#define LAST_ROW_INDEX 1008
#define RING_BUFFER_LENGTH 19
#define ROW_BUFFER_LENGTH 16

static const unsigned char bitmap_header[BITMAP_HEADER_LENGTH] = {
    0x42, 0x4D, 0x3E, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00};

typedef void (*CameraSuiteViewCameraCallback)(CameraSuiteCustomEvent event, void* context);

typedef struct CameraSuiteViewCamera {
    CameraSuiteViewCameraCallback callback;
    FuriStreamBuffer* camera_rx_stream;
    FuriHalSerialHandle* camera_serial_handle;
    FuriThread* camera_worker_thread;
    NotificationApp* notification;
    View* view;
    void* context;
} CameraSuiteViewCamera;

typedef struct CameraSuiteViewCameraModel {
    bool is_dithering_enabled;
    bool is_initialized;
    bool is_inverted;
    uint32_t orientation;
    uint8_t pixels[FRAME_BUFFER_LENGTH];
    uint8_t ringbuffer_index;
    uint8_t row_identifier;
    uint8_t row_ringbuffer[RING_BUFFER_LENGTH];
} CameraSuiteViewCameraModel;

CameraSuiteViewCamera* camera_suite_view_camera_alloc();
View* camera_suite_view_camera_get_view(CameraSuiteViewCamera* instance);
void camera_suite_view_camera_free(CameraSuiteViewCamera* instance);
void camera_suite_view_camera_set_callback(
    CameraSuiteViewCamera* instance,
    CameraSuiteViewCameraCallback callback,
    void* context);
