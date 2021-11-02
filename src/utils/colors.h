// Copyright [2021-11-02] <sxc19@mails.tsinghua.edu.cn, Xingchen Song>

#ifndef UTILS_COLORS_H_
#define UTILS_COLORS_H_

namespace wenet {

#define WENET_HEADER "[" << __TIME__ << " " << __FILE__ \
                     << ":" << __LINE__ << "] "
#define WENET_COLOR(a, b) "\033[" #b "m" << a << "\033[0m"
#define WENET_GREEN(a) WENET_COLOR(a, 32)
#define WENET_RED(a) WENET_COLOR(a, 31)
#define WENET_PINK(a) WENET_COLOR(a, 35)
#define WENET_YELLOW(a) WENET_COLOR(a, 33)
#define WENET_BLUE(a) WENET_COLOR(a, 34)

}  // namespace wenet

#endif  // UTILS_COLORS_H_
