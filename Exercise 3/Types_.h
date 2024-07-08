#ifndef TYPES__H
#define TYPES__H
#include <string>
enum TYPE { SPORTS, NEWS, WEATHER, DONE};
std::string typeToString(TYPE type);
TYPE stringToType(const std::string& str);
#endif