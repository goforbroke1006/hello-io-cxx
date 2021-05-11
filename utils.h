//
// Created by goforbroke on 04.05.2021.
//

#ifndef SERVER_SAMPLES_UTILS_H
#define SERVER_SAMPLES_UTILS_H

std::string getAppName(char **argv) {
    std::string tmp(argv[0]);
    size_t found = tmp.find_last_of("/");
    return tmp.substr(found + 1);
}

const char *ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string &rtrim(std::string &s, const char *t = ws) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string &ltrim(std::string &s, const char *t = ws) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string &trim(std::string &s, const char *t = ws) {
    return ltrim(rtrim(s, t), t);
}

#endif //SERVER_SAMPLES_UTILS_H
