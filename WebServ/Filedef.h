#ifndef FILESERV_FILEDEF_H
#define FILESERV_FILEDEF_H
#include <cstdint>
#include <cstdlib>

struct header{
    char str[32];
    uint32_t size;
};

#endif // !FILESERV_FILEDEF_H