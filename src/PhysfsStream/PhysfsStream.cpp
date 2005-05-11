#include "PhysfsStream.hpp"

#include <physfs.h>
#include <stdexcept>
#include <sstream>

IFileStreambuf::IFileStreambuf(const std::string& filename)
{
    file = PHYSFS_openRead(filename.c_str());
    if(file == 0) {
        std::stringstream msg;
        msg << "Couldn't open file '" << filename << "': "
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
}

IFileStreambuf::~IFileStreambuf()
{
    PHYSFS_close(file);
}

int
IFileStreambuf::underflow()
{
    if(PHYSFS_eof(file))
        return traits_type::eof();
    
    size_t bytesread = (size_t) PHYSFS_read(file, buf, 1, sizeof(buf));
    if(bytesread == 0)
        return traits_type::eof();
    setg(buf, buf, buf + bytesread);

    return buf[0];
}

//---------------------------------------------------------------------------

OFileStreambuf::OFileStreambuf(const std::string& filename)
{
    file = PHYSFS_openWrite(filename.c_str());
    if(file == 0) {
        std::stringstream msg;
        msg << "Couldn't open file '" << filename << "': "
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    
    setp(buf, buf+sizeof(buf));
}

OFileStreambuf::~OFileStreambuf()
{
    sync();
    PHYSFS_close(file);
}

int
OFileStreambuf::overflow(int c)
{
    if(pbase() == pptr())
        return 0;

    size_t size = pptr() - pbase();
    PHYSFS_sint64 res = PHYSFS_write(file, pbase(), 1, size);
    if(res <= 0)
        return traits_type::eof();
    
    if(c != traits_type::eof()) {
        PHYSFS_sint64 res = PHYSFS_write(file, &c, 1, 1);
        if(res <= 0)
            return traits_type::eof();
    }

    setp(buf, buf + res);
    return 0;
}

int
OFileStreambuf::sync()
{
    return overflow(traits_type::eof());
}

//---------------------------------------------------------------------------

IFileStream::IFileStream(const std::string& filename)
    : std::istream(new IFileStreambuf(filename))
{
}

IFileStream::~IFileStream()
{
    delete rdbuf();
}

//---------------------------------------------------------------------------

OFileStream::OFileStream(const std::string& filename)
    : std::ostream(new OFileStreambuf(filename))
{
}

OFileStream::~OFileStream()
{
    delete rdbuf();
}

