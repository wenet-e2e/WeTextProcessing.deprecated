// Copyright [2021-09-13] <sxc19@mails.tsinghua.edu.cn, Xingchen Song>

#include "text_processor/text_processor.h"

namespace wenet {

TextProcessor::TextProcessor(const std::string& tagger_fst_path,
                             const std::string& verbalizer_fst_path) {
  if (!tagger_fst_path.empty()) {
    tagger_fst_.reset(SortInputLabels(tagger_fst_path));
  }
  if (!verbalizer_fst_path.empty()) {
    verbalizer_fst_.reset(SortInputLabels(verbalizer_fst_path));
  }
  str_compiler_ = std::make_shared<fst::StringCompiler<fst::StdArc>>(
      fst::StringTokenType::BYTE);
}

fst::StdVectorFst* TextProcessor::SortInputLabels(const std::string& fst_path) {
  fst::StdVectorFst* sorted_fst = fst::StdVectorFst::Read(fst_path);
  std::cout << "Updating FST " << static_cast<const void*>(sorted_fst)
            << " with input label sorted version." << std::endl;
  fst::ArcSort(sorted_fst, fst::ILabelCompare<fst::StdArc>());
  return sorted_fst;
}

void TextProcessor::FormatFst(fst::StdVectorFst *vfst) {
  for (fst::StateIterator<fst::StdVectorFst> state_iter(*vfst);
       !state_iter.Done(); state_iter.Next()) {
    int state_id = state_iter.Value();
    for (fst::MutableArcIterator<fst::StdVectorFst> arc_iter(vfst, state_id);
         !arc_iter.Done(); arc_iter.Next()) {
       const fst::StdArc arc = arc_iter.Value();
       fst::StdArc new_arc(arc.ilabel & 0xff, arc.olabel & 0xff,
                           arc.weight, arc.nextstate);
       arc_iter.SetValue(new_arc);
    }
  }
}

std::string TextProcessor::ProcessInput(const std::string& input,
                                        bool verbose) {
  if (tagger_fst_ == nullptr || verbalizer_fst_ == nullptr) {
    std::cout << "tagger_fst_ == nullptr OR verbalizer_fst_ == nullptr, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  // tagger
  fst::StdVectorFst input_fst, tagged_lattice;
  if (!str_compiler_->operator()(input, &input_fst)) {
    std::cout << "compile input to input_fst failed, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  FormatFst(&input_fst);
  fst::ComposeOptions opts(true, fst::ALT_SEQUENCE_FILTER);
  fst::Compose(input_fst, *tagger_fst_, &tagged_lattice, opts);
  std::string tagged_text;
  if (!FstToString(tagged_lattice, &tagged_text)) {
    std::cout << "convert tagged_lattice to tagged_text failed, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  if (verbose) std::cout << "tagged_text   : " << tagged_text << std::endl;

  // parse and reorder
  std::string reordered_text = ParseAndReorder(tagged_text);
  if (verbose) std::cout << "reordered_text: " << reordered_text << std::endl;

  // verbalizer
  fst::StdVectorFst str_fst, verbalized_lattice;
  if (!str_compiler_->operator()(reordered_text, &str_fst)) {
    std::cout << "compile reordered_text to str_fst failed, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  FormatFst(&str_fst);
  fst::Compose(str_fst, *verbalizer_fst_, &verbalized_lattice, opts);
  std::string final_text;
  if (!FstToString(verbalized_lattice, &final_text)) {
    std::cout << "convert verbalized_lattice to final_text failed, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  return final_text;
}

std::string TextProcessor::ParseAndReorder(const std::string& structured_text) {
  if (structured_text.empty()) return structured_text;
  // parse and reorder structured_text
  std::stringstream ss(structured_text);
  std::vector<Token> tokens;
  std::string tmp, open_brace, close_brace, key, value;
  while (ss >> tmp) {
    assert(tmp == "token");
    Token t;
    ss >> open_brace;  // parse '{'
    ss >> t.token_name;
    ss >> open_brace;  // parse '{'
    while (ss >> key && key != "}") {
      ss >> value;
      t.key_value_map.emplace_back(make_pair(key, value));
    }
    ss >> close_brace;  // parse '}'
    std::reverse(t.key_value_map.begin(), t.key_value_map.end());  // reorder
    tokens.emplace_back(t);
  }
  // reconstruct reordered_text
  std::string reordered_text;
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (i != 0) reordered_text += " ";
    reordered_text += ("token { " + tokens[i].token_name + " { ");
    for (const auto& k_v : tokens[i].key_value_map) {
      reordered_text += (k_v.first + " " + k_v.second + " ");
    }
    reordered_text += "} }";
  }
  return reordered_text;
}

bool TextProcessor::FstToString(const fst::StdVectorFst& fst,
                                std::string* text) {
  fst::StdVectorFst shortest_path;
  fst::ShortestPath(fst, &shortest_path, 1);
  if (shortest_path.Start() == fst::kNoStateId) return false;
  fst::PathIterator<fst::StdArc> iter(shortest_path, false);
  std::string path;
  for (const auto label : iter.OLabels()) {
    if (label != 0) {
      path.push_back(label);
    }
  }
  *text = std::move(path);
  return true;
}

}  // namespace wenet
