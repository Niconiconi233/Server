#include "File.h"
#include <iostream>

using namespace std;

int main()
{
    File file("1.jpg");
    cout<<file.can_read()<<endl;
    cout<<file.can_write()<<endl;
    cout<<file.get_fd()<<endl;
    cout<<file.getFileSize()<<endl;
    cout<<file.is_file()<<endl;
    return 0;

}