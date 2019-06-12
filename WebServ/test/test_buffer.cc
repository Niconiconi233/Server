#include "Buffer.h"
#include "File.h"

#include <iostream>
#include <cassert>

using namespace std;

const int SIZE = 1028 * 8;

int main()
{
    Buffer bf(SIZE);
    File file("1.jpg");
    cout<<boolalpha;
    cout<<file.getFileSize()<<endl;
    cout<<file.can_read()<<noboolalpha <<endl;
    int fd = file.get_fd();
    int n = bf.readFd(fd);
    cout<<n;
    //std::cout<<bf.got().capacity();
    /*size_t ret = bf.writeableBytes();
    assert(ret == SIZE);
    ret = bf.readableBytes();
    assert(ret == 0);

    char buf[] = "hello mother fucker";
    bf.append(buf, sizeof buf);
    ret = bf.readableBytes();
    assert(ret == sizeof buf);
    char buf2[32];
    ret = bf.getAll(buf2);
    assert(ret == sizeof buf);
    cout<<buf2<<endl;
    ret = bf.append("123456", 6);
    assert(ret == 6);
    assert(bf.readableBytes() == 6);
    assert(bf.writeableBytes() == SIZE - 6);
    ret = bf.get(buf2, 3);
    assert(ret == 3);
    assert(bf.readableBytes() == 3);
    for(int i = 0; i < 256; ++i)
    {
        bf.append("a", 1);
    }
    assert(bf.readableBytes() == 256 + 3);
    cout<<*bf.got().begin()<<endl;*/

    return 0;
}