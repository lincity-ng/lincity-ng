#include "XmlReader.hpp"

XmlReader::XmlReader(const std::string& filename)
{
    // hack until we have proper resource handling
    std::string real_filename = "data/" + filename;
    
    reader = xmlReaderForFile(real_filename.c_str(), 0, XML_PARSE_NONET);
    if(reader == 0) {
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
