// Copyright [2021-09-14] <sxc19@mails.tsinghua.edu.cn, Xingchen Song>

#include "text_processor/text_processor.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cout << WENET_RED("[Usage]: ./text_process_main")
              << WENET_RED(" tagger.fst verbalizer.fst 1")
              << std::endl;
    return 0;
  }
  std::string tagger_fst_path = argv[1];
  std::string verbalizer_fst_path = argv[2];
  bool verbose = std::stoi(argv[3]);
  wenet::TextProcessor text_processor(tagger_fst_path,
                                      verbalizer_fst_path);
  std::string input;
  std::cout << "Start Processing Text (verbose = "
            << verbose << "):" << std::endl << std::endl;
  while (std::getline(std::cin, input)) {
    std::string output = text_processor.ProcessInput(input, verbose);
    std::cout << "input : " << WENET_GREEN(input) << std::endl
              << "output: " << WENET_BLUE(output) << std::endl << std::endl;
  }
  return 0;
}
