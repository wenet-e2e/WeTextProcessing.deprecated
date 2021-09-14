# Installation and Usage

```sh
# In Current Directory (wenet-text-processing/src)
mkdir -p build && cd build && cmake .. && cmake --build .
```

```sh
# In Current Directory (wenet-text-processing/src)
cd ../grammars/inverse_text_normalization/cn && make all -j2
```

```sh
# In Current Directory (wenet-text-processing/src)
cat ../grammars/inverse_text_normalization/cn/testcase_cn.txt | ./text_process_main ../grammars/inverse_text_normalization/cn/build/TAGGER.fst ../grammars/inverse_text_normalization/cn/build/VERBALIZER.fst 1
```


log output:

```sh
...
Start Processing Text (verbose = 1):
...

tagged_text   : token { word { name: "一共有1兆320万5000人" } }
reordered_text: token { word { name: "一共有1兆320万5000人" } }
input : 一共有一兆零三百二十万五千人
output: 一共有1兆320万5000人

...

tagged_text   : token { word { name: "明天有62%的概率降雨所以你有2.51%的可能性赢得比赛但是有-13%的人认为你有-20%的胜利可能性" } }
reordered_text: token { word { name: "明天有62%的概率降雨所以你有2.51%的可能性赢得比赛但是有-13%的人认为你有-20%的胜利可能性" } }
input : 明天有百分之六十二的概率降雨所以你有百分之二点五一的可能性赢得比赛但是有负百分之十三的人认为你有负的百分之二十的胜利可能性
output: 明天有62%的概率降雨所以你有2.51%的可能性赢得比赛但是有-13%的人认为你有-20%的胜利可能性

...

tagged_text   : token { word { name: "现场有" } } token { fraction { denominator: "17" frac: "/" numerator: "7" } } token { word { name: "的观众投出了赞成票可是最后唱票结果却是-" } } token { fraction { denominator: "12" frac: "/" numerator: "7" } }
reordered_text: token { word { name: "现场有" } } token { fraction { numerator: "7" frac: "/" denominator: "17" } } token { word { name: "的观众投出了赞成票可是最后唱票结果却是-" } } token { fraction { numerator: "7" frac: "/" denominator: "12" } }
input : 现场有十七分之七的观众投出了赞成票可是最后唱票结果却是负十二分之七
output: 现场有7/17的观众投出了赞成票可是最后唱票结果却是-7/12

...
```
