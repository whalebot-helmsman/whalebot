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

CLinkExtractor::CLinkExtractor(CUrlNormalizer& out)
: m_out(out)
, m_pParser(NULL)
{
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

hubbub_string   makeHubbubString(const char* str)
{
    hubbub_string   ret;
    ret.ptr =   reinterpret_cast<const uint8_t*>(str);
    ret.len =   strlen(str);
    return ret;
}

struct TExtractTask {
public:
    hubbub_string   TagName;
    hubbub_string   AttrName;
};

bool hubbubStrNCaseCmp(const hubbub_string& l, const hubbub_string& r)
{
    if (l.len != r.len) {
        return false;
    }

    return 0 == strncasecmp( reinterpret_cast<const char*>(l.ptr)
                           , reinterpret_cast<const char*>(r.ptr)
                           , r.len );
}

struct TExtractTaskMatcher {
public:
    TExtractTaskMatcher(const hubbub_string& tagName)
    : TagName(tagName)
    {}

    bool operator() (const TExtractTask& task)
    {
        return hubbubStrNCaseCmp(task.TagName, TagName);
    }

private:
    const hubbub_string&    TagName;
};

struct TAttributeMatcher {
public:
    TAttributeMatcher(const hubbub_string& attrName)
    : AttrName(attrName)
    {}

    bool operator() (const hubbub_attribute& attr)
    {
        return hubbubStrNCaseCmp(AttrName, attr.name);
    }

private:
    const hubbub_string&    AttrName;
};

hubbub_error CLinkExtractor::extract(const hubbub_tag& tag)
{
    static const TExtractTask   kTasks[]    =   {{makeHubbubString("a"), makeHubbubString("href")}};
    static const unsigned int   kTasksSize  =   sizeof(kTasks) / sizeof(kTasks[0]);
    static const TExtractTask*  kTasksBegin =   kTasks;
    static const TExtractTask*  kTasksEnd   =   kTasksBegin + kTasksSize;

    const TExtractTask* task    =   std::find_if(kTasksBegin, kTasksEnd, TExtractTaskMatcher(tag.name));
    if (kTasksEnd == task) {
        return HUBBUB_OK;
    }

    hubbub_attribute*   attrBegin   =   tag.attributes;
    hubbub_attribute*   attrEnd     =   attrBegin + tag.n_attributes;
    hubbub_attribute*   attribute   =   std::find_if(attrBegin, attrEnd, TAttributeMatcher(task->AttrName));

    if (attrEnd == attribute) {
        return HUBBUB_OK;
    }
    std::string link(reinterpret_cast<const char*>(attribute->value.ptr), attribute->value.len);
    m_out.pushLink(link);

    return HUBBUB_OK;
}

