#ifndef UI_VIEW_H_
#define UI_VIEW_H_

#include "ui_model.h"

/**
 * @brief Draws the UI and audio visualization into the graphics framebuffer.
 * @param display Formatted effect/parameter data to draw.
 * @param visualization Latest audio level and FFT data to draw.
 */
void ui_view_render(const ui_effect_display_t *display,
                    const ui_visualization_t *visualization);

#endif /* UI_VIEW_H_ */
