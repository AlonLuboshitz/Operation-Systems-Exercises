#include "Types_.h"

std::string typeToString(TYPE type) {
    switch(type) {
        case SPORTS: return "SPORTS";
        case NEWS: return "NEWS";
        case WEATHER: return "WEATHER";
        default: return "UNKNOWN";
    }
}
TYPE stringToType(const std::string& str) {
    if (str.find("SPORTS") != std::string::npos) {
        return SPORTS;
    } else if (str.find("NEWS") != std::string::npos) {
        return NEWS;
    } else if (str.find("WEATHER") != std::string::npos) {
        return WEATHER;
    } else return DONE;
    

}