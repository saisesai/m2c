#ifndef M2C_FONT_H
#define M2C_FONT_H

#include "config.h"

std::set<utf8_int32_t> font_read_codepoints_from_file(const std::string& p_input_path);

#endif // M2C_FONT_H
