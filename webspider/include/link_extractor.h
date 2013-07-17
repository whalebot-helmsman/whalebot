#ifndef _LINK_EXTRACTOR_H_
#define _LINK_EXTRACTOR_H_

#include <hubbub/hubbub.h>
#include <hubbub/parser.h>

#include <vector>
#include <iostream>
#include <string>

#include <prefix.h>
#include <link.h>


template <class T>
class CLinkExtractor {
public:

    CLinkExtractor(T &out);
    void extract(std::istream &in);
    static bool isParse(const std::string &ext);
    static hubbub_error hubbubCallback(const hubbub_token* token, void* pw);

    hubbub_error extract(const hubbub_tag& tag);

    ~CLinkExtractor();

private:
    T               &m_out;
    hubbub_parser*  m_pParser;
};

static void *myrealloc( void *ptr, size_t len, void* /*pw*/ )
{
    return realloc(ptr, len);
}

template<class T>
CLinkExtractor<T>::CLinkExtractor(T &out)
: m_out(out)
, m_pParser(NULL)
{
    hubbub_parser_create(NULL, false, myrealloc, NULL, &m_pParser);
    hubbub_parser_optparams token_handler_opt;
    token_handler_opt.token_handler.pw      =   static_cast<void*>(this);
    token_handler_opt.token_handler.handler =   CLinkExtractor<T>::hubbubCallback;
    hubbub_parser_setopt( m_pParser
                        , HUBBUB_PARSER_TOKEN_HANDLER
                        , &token_handler_opt );
}

template<class T>
CLinkExtractor<T>::~CLinkExtractor()
{
    hubbub_parser_destroy(m_pParser);
}

template<class T>
bool CLinkExtractor<T>::isParse(const std::string &ext)
{
    return ext == "html";
}

template<class T>
void CLinkExtractor<T>::extract(std::istream &in)
{
    static const std::streamsize    kMaxReadBufferSize =   1024;
    char                buffer[kMaxReadBufferSize];

    std::streamsize read    =   1;
    while ((0 != read) && (in)) {
        read    =   in.readsome(buffer, kMaxReadBufferSize);
        hubbub_parser_parse_chunk( m_pParser
                                 , reinterpret_cast<const uint8_t*>(buffer)
                                 , read);
    }
    hubbub_parser_completed(m_pParser);
}

template<class T>
hubbub_error CLinkExtractor<T>::hubbubCallback(const hubbub_token* token, void* pw)
{
    if (HUBBUB_TOKEN_START_TAG != token->type) {
        return HUBBUB_OK;
    }

    CLinkExtractor<T>* self    =   static_cast<CLinkExtractor<T>*>(pw);
    self->extract(token->data.tag);

    return HUBBUB_OK;
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

template<class T>
hubbub_error CLinkExtractor<T>::extract(const hubbub_tag& tag)
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

#endif //_LINK_EXTRACTOR_H_
