#include "config.h"

#include "font.h"
#include "image.h"

void handle_image_request(const argparse::ArgumentParser& p_program);

void handle_font_request(const argparse::ArgumentParser& p_program);

int main(int argc, const char* const argv[]) {
  // setup arg parser
  argparse::ArgumentParser program("m2c");
  program.add_argument("-m", "--mode")
      .help("working mode: [image, font]")
      .required();
  program.add_argument("-i", "--input").default_value("").help("input path");
  program.add_argument("-o", "--output").default_value("").help("output path");
  program.add_argument("-f", "--format")
      .default_value("")
      .help("output pixel format");
  program.add_argument("--font").default_value("").help("font input path");
  program.add_argument("--font-size")
      .default_value("")
      .help("font output size");
  // parse arg
  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& exp) {
    std::cerr << exp.what() << std::endl;
    exit(-1);
  }
  // handle request
  auto mode = program.get<std::string>("--mode");
  if (program.get<std::string>("--input").empty()) {
    std::cerr << "arg --input is required" << std::endl;
    exit(-1);
  }
  if (program.get<std::string>("--output").empty()) {
    std::cerr << "arg --output is required" << std::endl;
    exit(-1);
  }
  if (program.get<std::string>("--format").empty()) {
    std::cerr << "arg --format is required" << std::endl;
    exit(-1);
  }
  if (mode == "image") {
    handle_image_request(program);
  } else if (mode == "font") {
    if (program.get<std::string>("--font").empty()) {
      std::cerr << "arg --font is required in font mode" << std::endl;
      exit(-1);
    }
    handle_font_request(program);
  } else {
    std::cerr << "unknown mode: " << mode << std::endl;
    std::cerr << program << std::endl;
    exit(-1);
  }
  return 0;
}

void handle_image_request(const argparse::ArgumentParser& p_program) {
  auto cwd = std::filesystem::current_path().string();
  auto input = cwd + "/" + p_program.get<std::string>("--input");
  auto output = cwd + "/" + p_program.get<std::string>("--output");
  auto format = p_program.get<std::string>("--format");
  std::cout << "mode: image" << std::endl;
  std::cout << "input: " << input << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "format: " << format << std::endl;
  int width, height;
  uint8_t* rgba_bytes = image_read_png_rgba(input, &width, &height);
  std::cout << "width: " << width << " height: " << height << std::endl;
  uint8_t* rgb_bytes = image_rgba_to_rgb(rgba_bytes, width, height);
  uint8_t* rgb565_bytes = image_rgb_to_rgb565(rgb_bytes, width, height);
  std::cout << "converting....";
  if (format == "rgba") {
    image_write_pixel_to_c(output, rgba_bytes, width * height * 4);
  } else if (format == "rgb") {
    image_write_pixel_to_c(output, rgb_bytes, width * height * 3);
  } else if (format == "rgb565") {
    image_write_pixel_to_c(output, rgb565_bytes, width * height * 2);
  } else {
    std::cerr << "unknown pixel format: " << format << std::endl;
    exit(-1);
  }
  free(rgb565_bytes);
  free(rgb_bytes);
  free(rgba_bytes);
  std::cout << "done!" << std::endl;
}

void handle_font_request(const argparse::ArgumentParser& p_program) {
  auto cwd = std::filesystem::current_path().string();
  auto input = cwd + "/" + p_program.get<std::string>("--input");
  auto output = cwd + "/" + p_program.get<std::string>("--output");
  auto format = p_program.get<std::string>("--format");
  auto font = cwd + "/" + p_program.get<std::string>("--font");
  auto font_size = p_program.get<std::string>("--font-size");
  std::cout << "mode: font" << std::endl;
  std::cout << "input: " << input << std::endl;
  std::cout << "font: " << font << std::endl;
  std::cout << "font size: " << font_size << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "format: " << format << std::endl;
  auto codepints = font_read_codepoints_from_file(input);
}
