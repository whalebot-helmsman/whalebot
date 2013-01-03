#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include "stringparser.h"
#include "optionsbase.h"

class COptionsParser{
public:
                    COptionsParser(COptionsBase &options);
    void            parse(std::string const &str);
private:
    CStringParser   parser;
    COptionsBase    &options;
};

#endif // OPTIONSPARSER_H
