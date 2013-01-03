#include "streamparser.h"

CStreamLogger::CStreamLogger(std::ostream &out, size_t rate)
        :out(out), rate(rate), current_number(0){
}
void CStreamLogger::message(size_t message){
   if(current_number % rate == 0){
       out<<message<<std::endl;
   }
   ++current_number;
}

