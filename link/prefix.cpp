#include "prefix.h"
namespace prefix {

    static bool containInPos(std::string const &where, std::string const &what, size_t pos){
        return where.compare(pos, what.size(), what) == 0;
    }

    bool isMail(std::string const &link) {
        return containInPos(link, mailto, 0);
    }

    bool isHttp(std::string const &link) {
        return containInPos(link, http, 0);
    }

    bool isHttps(std::string const &link) {
        return containInPos(link, https, 0);
    }

    bool isJavaScript(std::string const &link) {
        return containInPos(link, javascript, 0);
    }
}
