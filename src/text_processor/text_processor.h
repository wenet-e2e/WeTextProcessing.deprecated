// Copyright [2021-09-13] <sxc19@mails.tsinghua.edu.cn, Xingchen Song>

#ifndef TEXT_PROCESSOR_TEXT_PROCESSOR_H_
#define TEXT_PROCESSOR_TEXT_PROCESSOR_H_

#include <utility>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>

#include "fst/fstlib.h"
#include "utils/paths.h"
#include "utils/colors.h"

namespace wenet {

const std::unordered_map<std::string, std::vector<std::string>> kReorderRules({
  {"money", {"currency:", "interger_part:", "fractional_part:"}},
  {"time", {"hour:", "minute:", "suffix:", "zone:"}},
  {"fraction", {"numerator:", "frac:", "denominator:"}}
});

struct Token {
  std::string token_name;
  // we want to keep the insertion order thus
  // we use vector + hash instead of map
  std::vector<std::string> token_members;
  std::unordered_map<std::string, std::string> member2value;
};

class TextProcessor {
 public:
  TextProcessor(const std::string& tagger_fst_path,
                const std::string& verbalizer_fst_path);
  fst::StdVectorFst* SortInputLabels(const std::string& fst_path);
  void FormatFst(fst::StdVectorFst* fst);
  std::string ProcessInput(const std::string& input, bool verbose);
  bool ParseAndReorder(const std::string& tagged_text,
                       std::string* reordered_text);
  bool FstToString(const fst::StdVectorFst& fst,
                   std::string* text);

 private:
  std::shared_ptr<fst::StringCompiler<fst::StdArc>> str_compiler_ = nullptr;
  std::shared_ptr<fst::StdVectorFst> tagger_fst_ = nullptr;
  std::shared_ptr<fst::StdVectorFst> verbalizer_fst_ = nullptr;
  std::unordered_map<std::string,
                     std::vector<std::string>> rules_ = kReorderRules;
};

}  // namespace wenet

#endif  // TEXT_PROCESSOR_TEXT_PROCESSOR_H_
