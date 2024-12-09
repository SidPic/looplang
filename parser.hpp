#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>

namespace ll {

void parse(std::vector<std::string>& script, const char* filename);

inline bool isinfix(const std::string& token);
inline char priority(const std::string& token);

}
#endif/*PARSER_HPP*/
