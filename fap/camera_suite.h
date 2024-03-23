#pragma once

#include <expansion/expansion.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdlib.h>

#include "scenes/camera_suite_scene.h"
#include "views/camera_suite_view_guide.h"
#include "views/camera_suite_view_start.h"
#include "views/camera_suite_view_camera.h"
#include "views/camera_suite_view_wifi_camera.h"
#include "helpers/camera_suite_storage.h"

#define TAG "Camera Suite"

#ifdef FW_ORIGIN_Xtreme
/**
 * Enable the following line for "Xtreme Firmware" & "Xtreme Apps" (Flipper-XFW).
 * 
 * @see https://github.com/Flipper-XFW/Xtreme-Firmware
 * @see https://github.com/Flipper-XFW/Xtreme-Apps
*/
#include <xtreme/xtreme.h>
#define UART_CH (xtreme_settings.uart_esp_channel)
#elif defined FW_ORIGIN_Momentum
/**
 * Enable the following line for "Momentum Firmware" & "Momentum Apps".
 * 
 * @see https://github.com/Next-Flip/Momentum-Firmware
 * @see https://github.com/Next-Flip/Momentum-Apps
*/
#include <momentum/momentum.h>
#define UART_CH (momentum_settings.uart_esp_channel)
#elif defined FW_ORIGIN_RM
/**
 * Enable the following line for "RogueMaster Firmware".
 * 
 * @see https://github.com/RogueMaster/flipperzero-firmware-wPlugins
*/
#include <cfw/cfw.h>
#define UART_CH (cfw_settings.uart_esp_channel)
#else
#define UART_CH (FuriHalSerialIdUsart)
#endif

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    CameraSuiteViewStart* camera_suite_view_start;
    CameraSuiteViewCamera* camera_suite_view_camera;
    CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera;
    CameraSuiteViewGuide* camera_suite_view_guide;
    uint32_t orientation;
    uint32_t dither;
    uint32_t flash;
    uint32_t haptic;
    uint32_t jpeg;
    uint32_t speaker;
    uint32_t led;
    ButtonMenu* button_menu;
} CameraSuite;

typedef enum {
    CameraSuiteViewIdStartscreen,
    CameraSuiteViewIdMenu,
    CameraSuiteViewIdCamera,
    CameraSuiteViewIdWiFiCamera,
    CameraSuiteViewIdGuide,
    CameraSuiteViewIdAppSettings,
    CameraSuiteViewIdCamSettings,
} CameraSuiteViewId;

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

#define CAMERA_WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

typedef enum {
    CameraSuiteOrientation0,
    CameraSuiteOrientation90,
    CameraSuiteOrientation180,
    CameraSuiteOrientation270,
} CameraSuiteOrientationState;

typedef enum {
    CameraSuiteDitherFloydSteinberg,
    CameraSuiteDitherStucki,
    CameraSuiteDitherJarvisJudiceNinke,
} CameraSuiteDitherState;

typedef enum {
    CameraSuiteFlashOff,
    CameraSuiteFlashOn,
} CameraSuiteFlashState;

typedef enum {
    CameraSuiteJpegOff,
    CameraSuiteJpegOn,
} CameraSuiteJpegState;

typedef enum {
    CameraSuiteHapticOff,
    CameraSuiteHapticOn,
} CameraSuiteHapticState;

typedef enum {
    CameraSuiteSpeakerOff,
    CameraSuiteSpeakerOn,
} CameraSuiteSpeakerState;

typedef enum {
    CameraSuiteLedOff,
    CameraSuiteLedOn,
} CameraSuiteLedState;
