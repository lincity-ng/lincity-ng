#include <config.h>

#include "XmlReader.hpp"

#include <physfs.h>

XmlReader::XmlReader(const std::string& filename)
{
    PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
    if(file == 0) {
        std::stringstream msg;
        msg << "Couldn't open file '" << filename << "': " 
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    
    reader = xmlReaderForIO(readCallback, closeCallback, file, 
            0, 0, XML_PARSE_NONET);
    if(reader == 0) {
        PHYSFS_close(file);
        std::stringstream msg;
        msg << "Couldn't parse file '" << filename << "'";
        throw std::runtime_error(msg.str());
    }
    while(read() && getNodeType() != XML_READER_TYPE_ELEMENT)
        ;
}

XmlReader::~XmlReader()
{
    xmlFreeTextReader(reader);
}

int
XmlReader::readCallback(void* context, char* buffer, int len)
{
    PHYSFS_file* file = (PHYSFS_file*) context;
    PHYSFS_sint64 result = PHYSFS_read(file, buffer, 1, len);
    if(result < 0) {
        std::cerr << "Read error: " << PHYSFS_getLastError() << "\n";
    }
    return (int) result;
}

int
XmlReader::closeCallback(void* context)
{
    PHYSFS_file* file = (PHYSFS_file*) context; 
    int res = PHYSFS_close(file);
    if(res < 0) {
        std::cerr << "Close error: " << PHYSFS_getLastError() << "\n";
    }
    return res;
}

