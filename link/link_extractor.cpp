#include "link_extractor.h"
#include "url_normalizer.hpp"

#include "prefix.h"

#include <hubbub/parser.h>

#include <vector>
#include <string>
#include <algorithm>

#include <cstring>

static void *myrealloc( void *ptr, size_t len, void* /*pw*/ )
{
    return realloc(ptr, len);
}

CLinkExtractor::CLinkExtractor(CUrlNormalizer& out, const CHtmlExtractorOptions& options)
: m_out(out)
, m_pParser(NULL)
{
    if (true == options.IsUseDefaultExtractor) {
        CTagExtractorOptions    anchorExtractor;
        anchorExtractor.Tag                     =   "a";
        anchorExtractor.TargetAttribute         =   "href";
        anchorExtractor.ControlAttribute        =   "";
        anchorExtractor.ControlAttributeValue   =   "";

        Extractors.push_back(anchorExtractor);
    }

    if (true == options.IsExtractImages) {
        CTagExtractorOptions    imagesExtractor;
        imagesExtractor.Tag                     =   "img";
        imagesExtractor.TargetAttribute         =   "src";
        imagesExtractor.ControlAttribute        =   "";
        imagesExtractor.ControlAttributeValue   =   "";

        Extractors.push_back(imagesExtractor);
    }

    if (true == options.IsExtractCss) {
        CTagExtractorOptions    cssExtractor;
        cssExtractor.Tag                    =   "link";
        cssExtractor.TargetAttribute        =   "href";
        cssExtractor.ControlAttribute       =   "rel";
        cssExtractor.ControlAttributeValue  =   "stylesheet";

        Extractors.push_back(cssExtractor);
    }

    if (true == options.IsExtractStructure) {
        static const char*     kStructureRelations[]    =   { "search"
                                                            , "up"
                                                            , "index"
                                                            , "first"
                                                            , "prev"
                                                            , "next"
                                                            , "last"
                                                            , "alternate"
                                                            , "help" };
        static const char**    kStructureRelationsBegin =   kStructureRelations;
        static const int       kStructureRelationsSize  =   sizeof(kStructureRelations) / sizeof(kStructureRelations[0]);
        static const char**    kStructureRelationsEnd   =   kStructureRelationsBegin + kStructureRelationsSize;

        for (const char** structureRelation = kStructureRelationsBegin; structureRelation != kStructureRelationsEnd; ++structureRelation) {
            CTagExtractorOptions    structureExtractor;
            structureExtractor.Tag                      =   "link";
            structureExtractor.TargetAttribute          =   "href";
            structureExtractor.ControlAttribute         =   "rel";
            structureExtractor.ControlAttributeValue    =   *structureRelation;

            Extractors.push_back(structureExtractor);
        }

    }

    if (true == options.IsExtractFavicons) {
        static const char*     kIconRelations[]    =   { "icon"
                                                       , "apple-touch-icon"
                                                       , "apple-touch-startup-image"
                                                       , "apple-touch-icon-precomposed"
                                                       , "shortcut icon"
                                                       , "msapplication-TileImage" };
        static const char**    kIconRelationsBegin =   kIconRelations;
        static const int       kIconRelationsSize  =   sizeof(kIconRelations) / sizeof(kIconRelations[0]);
        static const char**    kIconRelationsEnd   =   kIconRelationsBegin + kIconRelationsSize;

        for (const char** iconRelation = kIconRelationsBegin; iconRelation != kIconRelationsEnd; ++iconRelation) {
            CTagExtractorOptions    favicoExtractor;
            favicoExtractor.Tag                     =   "link";
            favicoExtractor.TargetAttribute         =   "href";
            favicoExtractor.ControlAttribute        =   "rel";
            favicoExtractor.ControlAttributeValue   =   *iconRelation;

            Extractors.push_back(favicoExtractor);
        }

    }

    Extractors.insert( Extractors.end()
                     , options.CustomTagExtractors.begin()
                     , options.CustomTagExtractors.end() );
}

hubbub_error CLinkExtractor::init()
{
    hubbub_error    ret =   HUBBUB_OK;

    ret =   hubbub_parser_create(NULL, false, myrealloc, NULL, &m_pParser);

    if (HUBBUB_OK != ret) {
        return ret;
    }

    if (NULL == m_pParser) {
        return HUBBUB_NOMEM;
    }

    hubbub_parser_optparams token_handler_opt;
    token_handler_opt.token_handler.pw      =   static_cast<void*>(this);
    token_handler_opt.token_handler.handler =   CLinkExtractor::hubbubCallback;
    ret =   hubbub_parser_setopt( m_pParser
                                , HUBBUB_PARSER_TOKEN_HANDLER
                                , &token_handler_opt );

    return ret;
}


CLinkExtractor::~CLinkExtractor()
{
    hubbub_parser_destroy(m_pParser);
}

bool CLinkExtractor::isParse(const std::string &ext)
{
    return ext == "html";
}

hubbub_error CLinkExtractor::extract(std::istream &in)
{
    static const std::streamsize    kMaxReadBufferSize =   1024;
    char            buffer[kMaxReadBufferSize];
    std::streamsize read    =   1;
    hubbub_error    result  =   HUBBUB_OK;

    while ((0 != read) && (in) && (HUBBUB_OK == result)) {
        read    =   in.readsome(buffer, kMaxReadBufferSize);
        result  =   hubbub_parser_parse_chunk( m_pParser
                                             , reinterpret_cast<const uint8_t*>(buffer)
                                             , read);
    }

    if (HUBBUB_OK != result) {
        return result;
    }

    result  =   hubbub_parser_completed(m_pParser);
    return result;
}

hubbub_error CLinkExtractor::hubbubCallback(const hubbub_token* token, void* pw)
{
    if (HUBBUB_TOKEN_START_TAG != token->type) {
        return HUBBUB_OK;
    }

    CLinkExtractor* self    =   static_cast<CLinkExtractor*>(pw);
    return self->extract(token->data.tag);
}

bool isStringEqual(const std::string& l, const hubbub_string& r)
{
    return (  (l.size() == r.len)
           && (0 == strncasecmp( l.c_str()
                               , reinterpret_cast<const char*>(r.ptr)
                               , r.len ) ) );
}

struct CAttributeMatcher {
public:
    CAttributeMatcher(const std::string& attrName)
    : AttrName(attrName)
    {}

    bool operator() (const hubbub_attribute& attr)
    {
        return isStringEqual(AttrName, attr.name);
    }
private:
    const std::string&  AttrName;
};

static bool isTagMatched(const hubbub_tag& tag, const CTagExtractorOptions& extractor)
{
    if (false == isStringEqual(extractor.Tag, tag.name)) {
        return false;
    }

    if (true == extractor.ControlAttribute.empty()) {
        return true;
    }

    hubbub_attribute*   attrBegin   =   tag.attributes;
    hubbub_attribute*   attrEnd     =   attrBegin + tag.n_attributes;
    hubbub_attribute*   attrPos     =   std::find_if( attrBegin
                                                    , attrEnd
                                                    , CAttributeMatcher(extractor.ControlAttribute) );

    if (attrEnd == attrPos) {
        return false;
    }

    if (true == extractor.ControlAttributeValue.empty()) {
        return true;
    }

    return isStringEqual(extractor.ControlAttributeValue, attrPos->value);
}

hubbub_error CLinkExtractor::extract(const hubbub_tag& tag)
{
    std::string result;

    for (unsigned int extractorNum = 0; extractorNum != Extractors.size(); ++extractorNum) {

        const CTagExtractorOptions& extractor(Extractors[extractorNum]);
        bool                        isMatched   =   isTagMatched(tag, extractor);

        if (false == isMatched) {
            continue;
        }

        hubbub_attribute*   attrBegin   =   tag.attributes;
        hubbub_attribute*   attrEnd     =   attrBegin + tag.n_attributes;
        hubbub_attribute*   attrPos     =   std::find_if( attrBegin
                                                        , attrEnd
                                                        , CAttributeMatcher(extractor.TargetAttribute) );

        if (attrEnd == attrPos) {
            continue;
        }

        hubbub_string   resultStr   =   attrPos->value;
        result.assign(reinterpret_cast<const char*>(resultStr.ptr), resultStr.len);
        m_out.pushLink(result);
    }
    return HUBBUB_OK;
}

