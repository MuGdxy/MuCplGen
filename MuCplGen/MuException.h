#pragma once
#include <exception>
#include <string>
namespace MuCplGen
{
    class Exception : public std::exception
    {
    public:
        Exception(const char* e): content(e) {}
        virtual const char* what() const noexcept override {return content.c_str();}
        virtual ~Exception() override {}
    private:
        std::string content;
    };
}
