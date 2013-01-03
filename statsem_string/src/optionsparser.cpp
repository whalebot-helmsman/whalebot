#include <to_string.h>
#include <optionsparser.h>


COptionsParser::COptionsParser(COptionsBase &options)
        :parser('='), options(options){}

void COptionsParser::parse(std::string const &str){
    std::string key(""),
                value("");
    parser.parseString(str);

    bool    _continue(true);
    _continue   &=   parser.getElement(key, 0);
    _continue   &=   parser.getElement(value, 1);


    trim(key, " \t\n");
    trim(value, " \t\n");

    _continue   &=  !key.empty();
    _continue   &=  !value.empty();

    if(_continue){
        options.set(key, value);
    }
}
