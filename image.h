#ifndef M2C_IMAGE_H
#define M2C_IMAGE_H

#include "config.h"

// image_read_png_rgba will return rgba pixel buffer, should free manually.
uint8_t* image_read_png_rgba(const std::string& p_file_path, int* p_width,
                             int* p_height);

uint8_t* image_rgba_to_rgb(const uint8_t* p_buffer, int p_width, int p_height);

uint8_t* image_rgb_to_rgb565(const uint8_t* p_buffer, int p_width,
                             int p_height);

void image_write_pixel_to_c(const std::string& p_file_path,
                            const uint8_t* p_buffer, int p_size);

#endif  // M2C_IMAGE_H
