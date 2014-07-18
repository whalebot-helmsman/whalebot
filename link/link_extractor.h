#ifndef _LINK_EXTRACTOR_H_
#define _LINK_EXTRACTOR_H_

#include <iostream>
#include <hubbub/hubbub.h>

class ILinkFactory;
struct hubbub_parser;

class CLinkExtractor {
public:

    CLinkExtractor(ILinkFactory& out);

    hubbub_error init();
    hubbub_error extract(std::istream &in);
    static bool isParse(const std::string &ext);
    static hubbub_error hubbubCallback(const hubbub_token* token, void* pw);

    hubbub_error extract(const hubbub_tag& tag);

    ~CLinkExtractor();

private:
    ILinkFactory&   m_out;
    hubbub_parser*  m_pParser;
};

#endif //_LINK_EXTRACTOR_H_
