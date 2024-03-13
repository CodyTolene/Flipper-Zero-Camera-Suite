#include "../camera_suite.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_led.h"

static void camera_suite_view_wifi_camera_draw(Canvas* canvas, void* model) {
    furi_assert(canvas);
    furi_assert(model);

    CameraSuiteViewWiFiCameraModel* instance = model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);

    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, "Feature coming soon!");

    // Draw log from camera.
    canvas_draw_str_aligned(
        canvas, 3, 13, AlignLeft, AlignTop, furi_string_get_cstr(instance->log));
}

static bool camera_suite_view_wifi_camera_input(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    CameraSuiteViewWiFiCamera* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        default:
            with_view_model(
                instance->view,
                CameraSuiteViewWiFiCameraModel * model,
                {
                    UNUSED(model);
                    // Stop all sounds, reset the LED.
                    camera_suite_play_bad_bump(instance->context);
                    camera_suite_stop_all_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 0);
                },
                true);
            break;
        }
    } else if(event->type == InputTypePress) {
        switch(event->key) {
        case InputKeyBack: {
            with_view_model(
                instance->view,
                CameraSuiteViewWiFiCameraModel * model,
                {
                    UNUSED(model);

                    // Stop camera WiFi stream.
                    furi_hal_serial_tx(instance->wifi_serial_handle, (uint8_t[]){'w'}, 1);
                    furi_delay_ms(50);

                    // Go back to the main menu.
                    instance->callback(CameraSuiteCustomEventSceneCameraBack, instance->context);
                },
                true);
            break;
        }
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
        case InputKeyMAX:
        default: {
            break;
        }
        }
    }

    return false;
}

static void camera_suite_view_wifi_camera_exit(void* context) {
    furi_assert(context);
}

static void camera_suite_view_wifi_camera_model_init(
    CameraSuiteViewWiFiCameraModel* const model,
    CameraSuite* instance_context) {
    furi_assert(model);
    furi_assert(instance_context);

    model->log = furi_string_alloc();
    furi_string_reserve(model->log, 4096);
}

static void camera_suite_view_wifi_camera_enter(void* context) {
    furi_assert(context);

    // Get the camera suite instance context.
    CameraSuiteViewWiFiCamera* instance = (CameraSuiteViewWiFiCamera*)context;

    // Get the camera suite instance context.
    CameraSuite* instance_context = instance->context;

    // Start wifi camera stream.
    furi_hal_serial_tx(instance->wifi_serial_handle, (uint8_t[]){'W'}, 1);
    furi_delay_ms(50);

    with_view_model(
        instance->view,
        CameraSuiteViewWiFiCameraModel * model,
        { camera_suite_view_wifi_camera_model_init(model, instance_context); },
        true);
}

static void
    wifi_on_irq_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    furi_assert(handle);
    furi_assert(context);

    // Cast `context` to `CameraSuiteViewWiFiCamera*` and store it in `instance`.
    CameraSuiteViewWiFiCamera* instance = context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(instance->wifi_rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(instance->wifi_worker_thread), WorkerEventRx);
    }
}

static int32_t camera_suite_wifi_camera_worker(void* context) {
    furi_assert(context);

    CameraSuiteViewWiFiCamera* instance = context;

    while(1) {
        // Wait for any event on the worker thread.
        uint32_t events =
            furi_thread_flags_wait(CAMERA_WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);

        // Check if an error occurred.
        furi_check((events & FuriFlagError) == 0);

        // Check if the thread should stop.
        if(events & WorkerEventStop) {
            break;
        } else if(events & WorkerEventRx) {
            size_t length = 0;
            // Read all available data from the stream buffer.
            do {
                // Read up to 64 bytes from the stream buffer.
                size_t buffer_size = 64;
                // Allocate a buffer for the data.
                uint8_t data[buffer_size];
                // Read the data from the stream buffer.
                length =
                    furi_stream_buffer_receive(instance->wifi_rx_stream, data, buffer_size, 0);
                if(length > 0) {
                    with_view_model(
                        instance->view,
                        CameraSuiteViewWiFiCameraModel * model,
                        {
                            UNUSED(model);
                            // Process the data.
                            // for(size_t i = 0; i < length; i++) {
                            // model    // Do something with model.
                            // data[i]  // Do something with data.
                            //}
                        },
                        false);
                }
            } while(length > 0);

            with_view_model(
                instance->view, CameraSuiteViewWiFiCameraModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc() {
    // Allocate memory for the instance
    CameraSuiteViewWiFiCamera* instance = malloc(sizeof(CameraSuiteViewWiFiCamera));

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate a stream buffer
    instance->wifi_rx_stream = furi_stream_buffer_alloc(2048, 1);

    // Allocate model
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(CameraSuiteViewWiFiCameraModel));

    // Set context for the view (furi_assert crashes in events without this)
    view_set_context(instance->view, instance);

    // Set draw callback
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_wifi_camera_draw);

    // Set input callback
    view_set_input_callback(instance->view, camera_suite_view_wifi_camera_input);

    // Set enter callback
    view_set_enter_callback(instance->view, camera_suite_view_wifi_camera_enter);

    // Set exit callback
    view_set_exit_callback(instance->view, camera_suite_view_wifi_camera_exit);

    // Allocate a thread for this camera to run on.
    FuriThread* thread = furi_thread_alloc_ex(
        "Camera_Suite_WiFi_Rx_Thread", 2048, camera_suite_wifi_camera_worker, instance);
    instance->wifi_worker_thread = thread;
    furi_thread_start(instance->wifi_worker_thread);

    // Allocate the serial handle for the camera.
    instance->wifi_serial_handle = furi_hal_serial_control_acquire(UART_CH);
    furi_check(instance->wifi_serial_handle);
    furi_hal_serial_init(instance->wifi_serial_handle, 230400);

    // Start the asynchronous receive.
    furi_hal_serial_async_rx_start(instance->wifi_serial_handle, wifi_on_irq_cb, instance, false);

    return instance;
}

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);

    // Free the worker thread.
    furi_thread_flags_set(furi_thread_get_id(instance->wifi_worker_thread), WorkerEventStop);
    furi_thread_join(instance->wifi_worker_thread);
    furi_thread_free(instance->wifi_worker_thread);

    // Free the allocated stream buffer.
    furi_stream_buffer_free(instance->wifi_rx_stream);

    // Deinitialize the serial handle and release the control.
    furi_hal_serial_deinit(instance->wifi_serial_handle);
    furi_hal_serial_control_release(instance->wifi_serial_handle);

    with_view_model(
        instance->view,
        CameraSuiteViewWiFiCameraModel * model,
        { furi_string_free(model->log); },
        true);
    view_free(instance->view);
    free(instance);
}

View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);
    return instance->view;
}

void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* instance,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
