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

build/extract_taggerfst: build taggers/taggers.far
	farextract --filename_suffix=".fst" --filename_prefix="build/" taggers/taggers.far
	touch build/extract_taggerfst

taggers/taggers.far: taggers/taggers.grm common/byte.far common/util.far taggers/int.far taggers/float.far taggers/punctuation.far taggers/word.far taggers/electronic.far taggers/telephone.far taggers/time.far taggers/date.far taggers/money.far taggers/measure.far taggers/whitelist.far taggers/fraction.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/punctuation.far: taggers/punctuation.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/whitelist.far: taggers/whitelist.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/word.far: taggers/word.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/fraction.far: taggers/fraction.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/electronic.far: taggers/electronic.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/telephone.far: taggers/telephone.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/time.far: taggers/time.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/date.far: taggers/date.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/money.far: taggers/money.grm common/byte.far common/util.far taggers/int.far taggers/float.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/measure.far: taggers/measure.grm common/byte.far common/util.far taggers/int.far taggers/float.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/float.far: taggers/float.grm common/byte.far common/util.far taggers/int.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

taggers/int.far: taggers/int.grm common/byte.far common/util.far
	thraxcompiler --save_symbols --input_grammar=$< --output_far=$@

move_far_to_build_dir: build
	mv common/*.far build
	mv taggers/*.far build
	mv verbalizers/*.far build

all: build/extract_taggerfst

.PHONY: clean move_far_to_build_dir

clean:
	rm -rf build common/*.far taggers/*.far verbalizers/*.far
