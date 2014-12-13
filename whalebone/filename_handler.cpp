

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/convenience.hpp>

#include "filename_handler.h"

CFilenameHandler::CFilenameHandler(const std::string &base_dir)
:m_baseDir(base_dir){
    if(m_baseDir[m_baseDir.size() - 1] != '/')
        m_baseDir  +=  "/";
}
bool CFilenameHandler::createPath(std::string server, std::string uri, std::string const &ext, std::string &filename)
{
    bool        ret(false);
    std::string name("index.html");



    filename    =   m_baseDir;
    filename    +=   server;



    if(!uri.empty()){

        boost::replace_all(uri, "+", "_");
        boost::replace_all(uri, "(", "_");
        boost::replace_all(uri, ")", "_");
        boost::replace_all(uri, "&", "_");
        boost::replace_all(uri, "?", "_");
        boost::replace_all(uri, "%", "_");
        boost::replace_all(uri, "=", "_");

        //generate name
        if(*uri.rbegin()=='/') {
            //only directory
            filename    +=  uri;
        }else{
            //include name of file
            size_t  pos(uri.rfind('/'));
            name        =   uri.substr(pos + 1);
            filename    +=  uri.substr(0, pos + 1);
        }

        //add extension to name
        std::string ext_(".");
        ext_    +=  ext;
        size_t  extSize(ext_.size()),
                nameSize(name.size());
        bool    add_(false);


        if(extSize > nameSize){
            add_    =   true;
        }else{
            add_    =   (name.compare(nameSize - extSize, extSize, ext_)!=0);
        }

        if(add_){
            name    +=  ext_;
        }


        while (*filename.rbegin() == '/') {
            filename.erase(filename.end() - 1);
        }

        ret =   boost::filesystem::exists(filename);
        ret |=  boost::filesystem::create_directories(filename);

        filename    +=  '/';
        filename    +=  name;


    }

    return ret;
}

