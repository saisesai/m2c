#include "image.h"

uint8_t* image_read_png_rgba(const std::string& p_file_path, int* p_width,
                             int* p_height) {
  FILE* fp = fopen(p_file_path.c_str(), "rb");
  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png) {
    std::cerr << "failed to create png read struct!" << std::endl;
    exit(-1);
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    std::cerr << "failed to create png info struct";
    exit(-1);
  }
  if (setjmp(png_jmpbuf(png))) {
    std::cerr << "failed to setjmp for png_jmpbuf" << std::endl;
    exit(-1);
  }
  png_init_io(png, fp);
  png_read_info(png, info);
  png_uint_32 width = png_get_image_width(png, info);
  png_uint_32 height = png_get_image_height(png, info);
  *p_width = static_cast<int>(width);
  *p_height = static_cast<int>(height);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);
  // read any color type to rgba
  if (bit_depth == 16) {
    png_set_strip_16(png);
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }
  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png);
  }
  if (png_get_valid(png, info, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png);
  }
  // These color_type don't have an alpha channel then fill it with 0xff.
  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }
  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }
  png_read_update_info(png, info);
  auto row_pointers =
      static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
  for (int y = 0; y < height; y++) {
    row_pointers[y] =
        static_cast<png_bytep>(malloc(png_get_rowbytes(png, info)));
  }
  png_read_image(png, row_pointers);
  fclose(fp);
  png_destroy_read_struct(&png, &info, nullptr);
  auto buffer = static_cast<uint8_t*>(malloc(width * height * 4));
  for (int y = 0; y < height; y++) {
    memcpy(buffer + y * width * 4, row_pointers[y], width * 4);
    free(row_pointers[y]);
  }
  free(row_pointers);
  return buffer;
}

uint8_t* image_rgba_to_rgb(const uint8_t* p_buffer, int p_width, int p_height) {
  auto buffer = static_cast<uint8_t*>(malloc(p_width * p_height * 3));
  for (int i = 0; i < p_width * p_height; i++) {
    buffer[i * 3 + 0] = p_buffer[i * 4 + 0];
    buffer[i * 3 + 1] = p_buffer[i * 4 + 1];
    buffer[i * 3 + 2] = p_buffer[i * 4 + 2];
  }
  return buffer;
}

uint8_t* image_rgb_to_rgb565(const uint8_t* p_buffer, int p_width,
                             int p_height) {
  auto buffer = static_cast<uint8_t*>(malloc(p_width * p_height * 2));
  for (int i = 0; i < p_width * p_height; i++) {
    uint8_t r = p_buffer[i * 3 + 0];
    uint8_t g = p_buffer[i * 3 + 1];
    uint8_t b = p_buffer[i * 3 + 2];
    uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    buffer[i * 2 + 0] = rgb565 >> 8 & 0xFF;
    buffer[i * 2 + 1] = rgb565 & 0xFF;
  }
  return buffer;
}

void image_write_pixel_to_c(const std::string& p_file_path,
                            const uint8_t* p_buffer, int p_size) {
  std::stringstream ss;
  ss << "#pragma once" << std::endl << std::endl;
  ss << "#include <stdint.h>" << std::endl << std::endl;
  ss << "const uint8_t out[" << p_size << "] = {" << std::endl;
  const int npl = 12;
  int n = 0;
  for (int i = 0; i < p_size; i++) {
    if (n == npl) {
      n = 0;
      ss << "\n";
    }
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(p_buffer[i]);
    if (i != p_size - 1) {
      ss << ", ";
    }
    n++;
  }
  ss << "};" << std::endl;
  std::ofstream file(p_file_path);
  if (!file) {
    std::cerr << "failed to open output file: " << p_file_path << std::endl;
  }
  file << ss.str();
  file.close();
}
