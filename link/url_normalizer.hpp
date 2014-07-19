#pragma once
#include <fstream>

class IAcceptor;
class CLink;

class CUrlNormalizer {
public:
    CUrlNormalizer(const std::string& storagePath);

    void pushLink(const std::string& link);
    void setFrom(const CLink &uri);
    void setAcceptor(IAcceptor &acceptor);
private:
    const CLink*    ParentLink;
    IAcceptor*      Acceptor;
    std::ofstream   Storage;
};
