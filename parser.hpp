#ifndef LOOPLANG_PARSER_HPP
#define LOOPLANG_PARSER_HPP

#include <vector>
#include <string>

namespace lpl {

void parse(std::vector<std::string>& script, const char* filename);

inline bool isinfix(const std::string& token);
inline char priority(const std::string& token);

}
#endif /* LOOPLANG_PARSER_HPP */
