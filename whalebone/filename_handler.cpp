#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

#include "filename_handler.h"

CFilenameHandler::CFilenameHandler(const std::string &base_dir)
:m_baseDir(base_dir){
    if(m_baseDir[m_baseDir.size() - 1] != '/')
        m_baseDir  +=  "/";
}

bool CFilenameHandler::StoreFile( const std::string& server
                                , const std::string& uri
                                , const std::string& ext
                                , const std::string& filename )
{
    std::string resultPath      =   "";
    bool        isPathCreated   =   createPath(server, uri, ext, resultPath);
    if (false == isPathCreated) {
        return false;
    }
    boost::system::error_code   error;
    boost::filesystem::rename(filename, resultPath, error);
    return error.value() == 0;
}

bool CFilenameHandler::createPath( const std::string& server
                                 , const std::string& uri
                                 , const std::string& ext
                                 , std::string &filename )
{
    bool        ret(false);
    std::string name("index.html");



    filename    =   m_baseDir;
    filename    +=   server;



    std::string uriCopy =   uri;
    if(!uriCopy.empty()){

        boost::replace_all(uriCopy, "+", "_");
        boost::replace_all(uriCopy, "(", "_");
        boost::replace_all(uriCopy, ")", "_");
        boost::replace_all(uriCopy, "&", "_");
        boost::replace_all(uriCopy, "?", "_");
        boost::replace_all(uriCopy, "%", "_");
        boost::replace_all(uriCopy, "=", "_");

        //generate name
        if(*uriCopy.rbegin()=='/') {
            //only directory
            filename    +=  uriCopy;
        }else{
            //include name of file
            size_t  pos(uriCopy.rfind('/'));
            name        =   uriCopy.substr(pos + 1);
            filename    +=  uriCopy.substr(0, pos + 1);
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

