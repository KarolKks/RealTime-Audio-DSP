#ifndef UI_MODEL_H_
#define UI_MODEL_H_

#include <stdbool.h>
#include <stdint.h>

#define UI_SPECTRUM_BAND_COUNT 16U

typedef struct
{
    float band_magnitudes[UI_SPECTRUM_BAND_COUNT];
    uint16_t peak_level_per_mille;
} ui_visualization_t;

typedef struct
{
    char effect_name[8];
    char parameter_label[8];
    char parameter_value[16];
    int32_t parameter_value_raw;
    int32_t parameter_min;
    int32_t parameter_max;
    bool muted;
} ui_effect_display_t;

#endif /* UI_MODEL_H_ */
