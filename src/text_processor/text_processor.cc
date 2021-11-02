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

void TextProcessor::FormatFst(fst::StdVectorFst* vfst) {
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
  std::string tagged_text, reordered_text;
  if (!FstToString(tagged_lattice, &tagged_text)) {
    std::cout << "convert tagged_lattice to tagged_text failed, "
              << "will do nothing for input." << std::endl;
    return input;
  }
  if (verbose) std::cout << "tagged_text   : " << tagged_text << std::endl;

  // parse and reorder
  if (!ParseAndReorder(tagged_text, &reordered_text)) {
    return input;
  }
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

bool TextProcessor::ParseAndReorder(const std::string& tagged_text,
                                    std::string* reordered_text) {
  // i.e. tagged_text =
  //          token { fraction { denominator: "13" frac: "/" numerator: "12" } }
  //      OR
  //          token { word { name: "哈哈" } }
  if (tagged_text.empty()) return false;
  std::stringstream ss(tagged_text);
  std::vector<Token> tokens;
  std::string tmp, tmp2, open_brace, close_brace, key, value;

  // stage-1 parse tagged_text and reorder
  // i.e. Token.token_name = fraction
  //      Token.token_members =
  //          {denominator:, frac:, numerator:}
  //      Reorder(Token.token_members) =
  //          {numerator:, frac:, denominator:}
  //      Token.member2value =
  //          {{numerator:, "12"}, {frac:, "/"}, {denominator:, "13"}}
  //     OR
  //      Token.token_name = word
  //      Token.token_members =
  //          {name:}
  //      Reorder(Token.token_members) =
  //          {name:}
  //      Token.member2value =
  //          {{name:, "哈哈"}}
  while (ss >> tmp) {
    // stage-1.1 parse/separate tagged_text using spaces
    if (tmp != "token") return false;
    Token t;
    ss >> open_brace;  // parse '{'
    ss >> t.token_name;
    ss >> open_brace;  // parse '{'
    while (ss >> key && key != "}") {
      // parse value
      ss >> value;
      if (value[0] != '"') return false;
      // value may contain spaces, i.e., "2.3 millions"
      if (value[value.size() - 1] != '"') {
        while (ss >> tmp2 && tmp2[tmp2.size() - 1] != '"') {
          value += (" " + tmp2);
        }
        value += (" " + tmp2);
      }
      if (value[value.size() - 1] != '"') return false;
      t.token_members.emplace_back(key);
      t.member2value[key] = value;
    }
    ss >> close_brace;  // parse '}'
    // stage-1.2 reorder token according to predefined rules
    if (t.token_members.size() > 1
        && rules_.find(t.token_name) != rules_.end()) {
      // check consistance
      for (const auto& m : rules_[t.token_name]) {
        if (t.member2value.find(m) == t.member2value.end()) return false;
      }
      // actually do the reordering
      t.token_members = rules_[t.token_name];
    }
    tokens.emplace_back(t);
  }

  // stage-2: construct reordered_text
  // i.e. reordered_text =
  //          token { fraction { numerator: "12" frac: "/" denominator: "13" } }
  //      OR
  //          token { word { name: "哈哈" } }
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (i != 0) reordered_text->insert(reordered_text->size(), " ");
    reordered_text->insert(reordered_text->size(),
                           "token { " + tokens[i].token_name + " { ");
    for (const auto& m : tokens[i].token_members) {
      reordered_text->insert(reordered_text->size(),
                             m + " " + tokens[i].member2value[m] + " ");
    }
    reordered_text->insert(reordered_text->size(), "} }");
  }
  return true;
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
