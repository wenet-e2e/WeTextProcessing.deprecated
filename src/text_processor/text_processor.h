// Copyright [2021-09-13] <sxc19@mails.tsinghua.edu.cn, Xingchen Song>

#ifndef SRC_TEXT_PROCESSOR_TEXT_PROCESSOR_H_
#define SRC_TEXT_PROCESSOR_TEXT_PROCESSOR_H_

#include <utility>
#include <string>
#include <memory>
#include <vector>

#include "fst/fstlib.h"
#include "utils/paths.h"

namespace wenet {

struct Token {
  std::string token_name;
  // we want to keep the insertion order thus we use vector instead of map
  std::vector<std::pair<std::string, std::string>> key_value_map;
};

class TextProcessor {
 public:
  TextProcessor(const std::string& tagger_fst_path,
                const std::string& verbalizer_fst_path);
  fst::StdVectorFst* SortInputLabels(const std::string& fst_path);
  void FormatFst(fst::StdVectorFst* fst);
  std::string ProcessInput(const std::string& input, bool verbose);
  std::string ParseAndReorder(const std::string& tagged_text);
  bool FstToString(const fst::StdVectorFst& fst,
                   std::string* text);

 private:
  std::shared_ptr<fst::StringCompiler<fst::StdArc>> str_compiler_ = nullptr;
  std::shared_ptr<fst::StdVectorFst> tagger_fst_ = nullptr;
  std::shared_ptr<fst::StdVectorFst> verbalizer_fst_ = nullptr;
};

}  // namespace wenet

#endif  // SRC_TEXT_PROCESSOR_TEXT_PROCESSOR_H_
