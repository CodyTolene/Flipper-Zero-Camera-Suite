#ifndef CAMERA_MODEL_H
#define CAMERA_MODEL_H

#include <stdint.h>

/** The dithering algorithms available. */
typedef enum DitheringAlgorithm {
    FLOYD_STEINBERG,
    JARVIS_JUDICE_NINKE,
    STUCKI,
} DitheringAlgorithm;

typedef enum CameraFunction {
    CAMERA_FUNCTION_SERIAL,
    CAMERA_FUNCTION_WIFI,
} CameraFunction;

typedef struct CameraModel {
    /** Flag to enable or disable dithering. */
    bool isDitheringEnabled;
    /** Flag to represent the flash state when saving pictures to the Flipper Zero. */
    bool isFlashEnabled;
    /** Flag to invert pixel colors. */
    bool isInvertEnabled;
    /** Flag to stop or start the stream to the Flipper Zero. */
    bool isStreamToSerialEnabled;
    /** Flag to stop or start the stream to WiFi. */
    bool isStreamToWiFiEnabled;
    /** Holds the currently selected dithering algorithm. */
    DitheringAlgorithm ditherAlgorithm;
} CameraModel;

/** The camera model. */
extern CameraModel camera_model;

/** Set the camera model to the default values depending on the camera use. */
void set_camera_model_defaults(CameraFunction camera_function);

#endif
