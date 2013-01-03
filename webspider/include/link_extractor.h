#ifndef _LINK_EXTRACTOR_H_
#define _LINK_EXTRACTOR_H_

#include <iterator>
#include <iostream>
#include <string>

#include <htmlcxx/html/ParserSax.h>
#include <htmlcxx/html/utils.h>

#include <prefix.h>
#include <link.h>


template <class T>
class CLinkExtractor : public htmlcxx::HTML::ParserSax {
public:

    CLinkExtractor(T &out)
    :m_out(out) {}

    void extract(std::istream &in) {
        std::string buff("");
        std::istreambuf_iterator<char> walker(in),
                stop;
        while (walker != stop) {
            buff += *walker;
            ++walker;
        }

        parse(buff);
    }

    static bool isParse(const std::string &ext) {
        return ext == "html";
    }
protected:

    void foundTag(htmlcxx::HTML::Node node, bool isEnd) {
        if (node.tagName() == "a") {
            node.parseAttributes();
            std::pair<bool, std::string > res(node.attribute("href"));
            if (res.first) {
                m_out.pushLink(htmlcxx::HTML::decode_entities(res.second));
            }
        }
    }
private:
    T &m_out;
};

#endif //_LINK_EXTRACTOR_H_
