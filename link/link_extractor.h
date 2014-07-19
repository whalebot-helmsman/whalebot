#ifndef _LINK_EXTRACTOR_H_
#define _LINK_EXTRACTOR_H_

#include <options/options.hpp>

#include <hubbub/hubbub.h>

#include <iostream>

class CUrlNormalizer;
struct hubbub_parser;

class CLinkExtractor {
public:

    CLinkExtractor(CUrlNormalizer& out, const CHtmlExtractorOptions& options);

    hubbub_error init();
    hubbub_error extract(std::istream &in);
    static bool isParse(const std::string &ext);
    static hubbub_error hubbubCallback(const hubbub_token* token, void* pw);

    hubbub_error extract(const hubbub_tag& tag);

    ~CLinkExtractor();

private:
    CUrlNormalizer&                         m_out;
    hubbub_parser*                          m_pParser;
    CHtmlExtractorOptions::CTagExtractors   Extractors;
};

#endif //_LINK_EXTRACTOR_H_
