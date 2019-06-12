#ifndef FILESERV_FILE_H
#define FILESERV_FILE_H
#include <sys/stat.h>
#include <string>
#include <cerrno>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>

#include "Logging/base/Logging.h"

class File{
public: 
    File(const std::string filename):fileName_(filename),fd_(-1)
    {
    }

    ~File()
    {
        if(fd_ != -1)
            ::close(fd_);
    }

    bool have_file()
    {
        int ret = stat(fileName_.c_str(), &filestat_);
        if(ret != 0){
            LOG_ERROR << "File::file " << strerror(errno);
            return false;
        }
        return true;
    }

    size_t getFileSize()
    {
        return filestat_.st_size;
    }

    bool is_file()
    {
        return S_ISREG(filestat_.st_mode);
    }

    int get_fd()
    {
        fd_ = ::open(fileName_.c_str(), O_RDONLY | O_CLOEXEC | O_CREAT);
        return fd_;
    }

    bool can_read()
    {
        return ::access(fileName_.c_str(), R_OK) == 0 ? true : false;
    }

    bool can_write()
    {
        return ::access(fileName_.c_str(), W_OK) == 0 ? true : false;
    }

private: 
    std::string fileName_;
    struct stat filestat_;
    int fd_;

};

#endif // !FILESERV_FILE_H