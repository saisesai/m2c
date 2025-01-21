#include "font.h"

std::set<utf8_int32_t> font_read_codepoints_from_file(
    const std::string& p_input_path) {
  std::set<utf8_int32_t> codepoints;
  // read string from file
  std::ifstream input_file(p_input_path);
  if (!input_file) {
    std::cerr << "failed to open input file!" << std::endl;
    exit(-1);
  }
  std::stringstream ss;
  ss << input_file.rdbuf();
  std::string input_str = ss.str();
  input_str.erase(
      std::remove_if(input_str.begin(), input_str.end(),
                     [](const unsigned char x) { return std::isspace(x); }),
      input_str.end());
  const char* codepoint_now = input_str.c_str();
  for (int i = 0; i < utf8len(input_str.c_str()); i++) {
    utf8_int32_t codepoint;
    codepoint_now = utf8codepoint(codepoint_now, &codepoint);
    codepoints.insert(codepoint);
  }
  return codepoints;
}