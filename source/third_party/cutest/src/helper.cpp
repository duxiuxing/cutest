﻿#include "cutest/helper.h"
#include "gtest/gtest-message.h"

#include <stack>

const char* CUTEST_NS::Version() {
    static std::string version;

    if (version.empty()) {
        if (0 == BUILD_NUMBER) {
            // 本地编译会进入这个分支，这个时候取当前时刻作为版本号。
            version = __TIME__;
            version.append(" ");
            version.append(__DATE__);
        } else {
            // 持续集成会进入这个分支，这个时候按照实际版本信息来拼接版本号。
            int major   = MAJOR_VERSION;
            int minor   = MINOR_VERSION;
            int fix     = FIX_VERSION;
            int build   = BUILD_NUMBER;
            version = testing::internal::StreamableToString(major) + "."
                      + testing::internal::StreamableToString(minor) + "."
                      + testing::internal::StreamableToString(fix) + "."
                      + testing::internal::StreamableToString(build);
        }
    }

    return version.c_str();
}

bool CUTEST_NS::IsOnMainThread() {
    return (CUTEST_NS::MainThreadId() == CUTEST_NS::CurrentThreadId());
}

std::string CUTEST_NS::MakeFilePathShorter(std::string path) {
    std::string str = path;
    std::stack<std::string> dirStack;
    size_t index = str.find('/');
    while (index != -1) {
        size_t length = str.length();
        if (0 != index) {
            // 把'/'左边的截断出来放到strlist
            std::string dir = str.substr(0, index);
            if (dir == ".") {
                if (dirStack.empty()) {
                    dirStack.push(dir);
                }
            } else if (dir == "..") {
                if (dirStack.empty()) {
                    dirStack.push(dir);
                } else if (dirStack.top() == "..") {
                    dirStack.push(dir);
                } else {
                    dirStack.pop();
                }
            } else {
                dirStack.push(dir);
            }
        } else {
            // 如果第一个字符就是'/'，则跳过
        }

        // 计算'/'右边的字符串长度
        size_t start = index + 1;
        length -= start;

        // 然后把'/'右边的字符串赋给str
        if (length) {
            str = str.substr(start, length);
        } else {
            str = "";
        }

        index = str.find('/');
    }

    if (!str.empty()) {
        dirStack.push(str);
    }

    str.clear();
    while (!dirStack.empty()) {
        if (!str.empty()) {
            str = '/' + str;
        }

        str = dirStack.top() + str;
        dirStack.pop();
    }

    return str;
}
