PATH := $(PATH):$(PWD)/../../../src/fc_base/thrax-subbuild/thrax-populate-prefix/bin:$(PWD)/../../../src/fc_base/openfst-subbuild/openfst-populate-prefix/bin
LD_LIBRARY_PATH := $(LD_LIBRARY_PATH):$(PWD)/../../../src/fc_base/thrax-subbuild/thrax-populate-prefix/lib:$(PWD)/../../../src/fc_base/openfst-subbuild/openfst-populate-prefix/lib

build:
	mkdir -p build

# common

common/byte.far: common/byte.grm
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

common/util.far: common/util.grm common/byte.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

# tagger

taggers/int.far: taggers/int.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/float.far: taggers/float.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/time.far: taggers/time.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/measure.far: taggers/measure.grm common/byte.far common/util.far taggers/int.far taggers/float.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/telephone.far: taggers/telephone.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/electronic.far: taggers/electronic.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/fraction.far: taggers/fraction.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/percentage.far: taggers/percentage.grm common/byte.far common/util.far taggers/int.far taggers/float.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/date.far: taggers/date.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/fraction_structured.far: taggers/fraction_structured.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/word_structured.far: taggers/word_structured.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/whitelist.far: taggers/whitelist.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/taggers.far: taggers/taggers.grm common/byte.far common/util.far taggers/int.far taggers/float.far taggers/time.far taggers/measure.far taggers/electronic.far taggers/fraction_structured.far taggers/word_structured.far taggers/whitelist.far taggers/percentage.far taggers/fraction.far taggers/date.far taggers/telephone.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

build/extract_taggerfst: build taggers/taggers.far
	farextract --filename_suffix=".fst" --filename_prefix="build/" taggers/taggers.far
	touch build/extract_taggerfst

# verbalizer

verbalizers/word_unstructured.far: verbalizers/word_unstructured.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

verbalizers/fraction_unstructured.far: verbalizers/fraction_unstructured.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

verbalizers/verbalizers.far: verbalizers/verbalizers.grm common/byte.far common/util.far verbalizers/word_unstructured.far verbalizers/fraction_unstructured.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

build/extract_verbalizerfst: build verbalizers/verbalizers.far
	farextract --filename_suffix=".fst" --filename_prefix="build/" verbalizers/verbalizers.far
	touch build/extract_verbalizerfst

move_far_to_build_dir: build
	mv common/*.far build
	mv taggers/*.far build
	mv verbalizers/*.far build

all: build/extract_taggerfst build/extract_verbalizerfst

.PHONY: clean move_far_to_build_dir

clean:
	rm -rf build common/*.far taggers/*.far verbalizers/*.far
