#include "PhysfsSDL.hpp"

#include <physfs.h>

#include <stdexcept>
#include <sstream>
#include <iostream>

static int funcSeek(struct SDL_RWops* context, int offset, int whence)
{
    PHYSFS_file* file = (PHYSFS_file*) context->hidden.unknown.data1;
    int res;
    switch(whence) {
        case SEEK_SET:
            PHYSFS_seek(file, offset);
            break;
        case SEEK_CUR:
            res = PHYSFS_seek(file, PHYSFS_tell(file) + offset);
            break;
        case SEEK_END:
            res = PHYSFS_seek(file, PHYSFS_fileLength(file) + offset);
            break;
        default:
            res = 0;
            assert(false);
            break;
    }
    if(!res) {
        std::cerr << "Error seeking in file: " << PHYSFS_getLastError() << "\n";
        return -1;
    }

    return (int) PHYSFS_tell(file);
}

static int funcRead(struct SDL_RWops* context, void* ptr, int size, int maxnum)
{
    PHYSFS_file* file = (PHYSFS_file*) context->hidden.unknown.data1;

    int res = PHYSFS_read(file, ptr, size, maxnum);
    return res;
}

static int funcClose(struct SDL_RWops* context)
{
    PHYSFS_file* file = (PHYSFS_file*) context->hidden.unknown.data1;
    
    PHYSFS_close(file);
    delete context;

    return 0;
}

SDL_RWops* getPhysfsSDLRWops(const std::string& filename)
{
    PHYSFS_file* file = (PHYSFS_file*) PHYSFS_openRead(filename.c_str());
    if(!file) {
        std::stringstream msg;
        msg << "Couldn't open '" << filename << "': "
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    
    SDL_RWops* ops = new SDL_RWops();
    ops->type = 0;
    ops->hidden.unknown.data1 = file;
    ops->seek = funcSeek;
    ops->read = funcRead;
    ops->write = 0;
    ops->close = funcClose;
    return ops;
}
