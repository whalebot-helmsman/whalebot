#ifndef _STREAMPARSERENGINE_H_
#define _STREAMPARSERENGINE_H_
#include <iostream>
#include <string>


class CStreamParserEngine{
public:
    template<class OneStringParser, class Logger>
    static void parseFile(std::istream &in, OneStringParser &parser, Logger &log){
        std::string read_str;
        size_t      string_counter(0);

        while(std::getline(in, read_str)){

            parser.parse(read_str);

            ++string_counter;
            log.message(string_counter);
        }

    }
    template<class OneStringParser>
    static void parseFile(std::istream &in, OneStringParser &parser){
        std::string read_str;

        while(std::getline(in, read_str)){
            parser.parse(read_str);
        }

    }
};

class CStreamLogger {
public:
        CStreamLogger(std::ostream &out, size_t rate = 10);
   void message(size_t message);
private:
    std::ostream    &out;
    size_t          rate,
                    current_number;
};



#endif // _STREAMPARSERENGINE_H_
