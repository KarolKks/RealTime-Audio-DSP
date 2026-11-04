#ifndef UI_WIDGETS_H_
#define UI_WIDGETS_H_

#include <stdint.h>

/**
 * @brief  Draws a formatted progress bar in the graphics frame buffer.
 * @param  x: X coordinate of top-left corner.
 * @param  y: Y coordinate of top-left corner.
 * @param  width: Width of progress bar in pixels.
 * @param  height: Height of progress bar in pixels.
 * @param  val: Current parameter value.
 * @param  min: Minimum parameter value.
 * @param  max: Maximum parameter value.
 */
void ui_widget_progress_bar(int16_t x, int16_t y, int16_t width, int16_t height, 
                           int32_t val, int32_t min, int32_t max);

#endif /* UI_WIDGETS_H_ */

