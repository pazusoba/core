/**
 * binding.h
 * create binding to python
 * by Yiheng Quan
 */

#include <string>

extern "C"
{
    std::string hello()
    {
        return "hello world";
    }
}
