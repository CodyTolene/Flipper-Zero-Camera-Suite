#include "camera_suite.h"
#include <stdlib.h>
#include <expansion/expansion.h>

bool camera_suite_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    CameraSuite* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void camera_suite_tick_event_callback(void* context) {
    furi_assert(context);
    CameraSuite* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

// Leave app if back button pressed.
bool camera_suite_navigation_event_callback(void* context) {
    furi_assert(context);
    CameraSuite* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

CameraSuite* camera_suite_app_alloc() {
    CameraSuite* app = malloc(sizeof(CameraSuite));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on.
    notification_message(app->notification, &sequence_display_backlight_on);

    // Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&camera_suite_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, camera_suite_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, camera_suite_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, camera_suite_custom_event_callback);
    app->submenu = submenu_alloc();

    // Set app default settings values.
    app->haptic = 1; // Haptic is enabled by default
    app->jpeg = 0; // Save JPEG to ESP32-CAM sd-card is disabled by default.
    app->speaker = 1; // Speaker is enabled by default
    app->led = 1; // LED is enabled by default

    // Set cam default settings values.
    app->orientation = 0; // Orientation is "portrait", zero degrees by default.
    app->dither = 0; // Dither algorithm is "Floyd Steinberg" by default.
    app->flash = 1; // Flash is enabled by default.

    // Load configs if available (overrides defaults).
    camera_suite_read_settings(app);

    view_dispatcher_add_view(
        app->view_dispatcher, CameraSuiteViewIdMenu, submenu_get_view(app->submenu));

    app->camera_suite_view_start = camera_suite_view_start_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdStartscreen,
        camera_suite_view_start_get_view(app->camera_suite_view_start));

    app->camera_suite_view_camera = camera_suite_view_camera_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdCamera,
        camera_suite_view_camera_get_view(app->camera_suite_view_camera));

    app->camera_suite_view_wifi_camera = camera_suite_view_wifi_camera_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdWiFiCamera,
        camera_suite_view_wifi_camera_get_view(app->camera_suite_view_wifi_camera));

    app->camera_suite_view_guide = camera_suite_view_guide_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdGuide,
        camera_suite_view_guide_get_view(app->camera_suite_view_guide));

    app->button_menu = button_menu_alloc();

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdAppSettings,
        variable_item_list_get_view(app->variable_item_list));
    view_dispatcher_add_view(
        app->view_dispatcher,
        CameraSuiteViewIdCamSettings,
        variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    return app;
}

void camera_suite_app_free(CameraSuite* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdStartscreen);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdCamera);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdWiFiCamera);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdGuide);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdAppSettings);
    view_dispatcher_remove_view(app->view_dispatcher, CameraSuiteViewIdCamSettings);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    // Free remaining resources
    camera_suite_view_start_free(app->camera_suite_view_start);
    camera_suite_view_camera_free(app->camera_suite_view_camera);
    camera_suite_view_wifi_camera_free(app->camera_suite_view_wifi_camera);
    camera_suite_view_guide_free(app->camera_suite_view_guide);

    button_menu_free(app->button_menu);
    variable_item_list_free(app->variable_item_list);

    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

/** Main entry point for initialization. */
int32_t camera_suite_app(void* p) {
    UNUSED(p);

    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    CameraSuite* app = camera_suite_app_alloc();
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    // Init with start scene.
    scene_manager_next_scene(app->scene_manager, CameraSuiteSceneStart);
    furi_hal_power_suppress_charge_enter();
    view_dispatcher_run(app->view_dispatcher);
    camera_suite_save_settings(app);
    furi_hal_power_suppress_charge_exit();
    camera_suite_app_free(app);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
