//io.h
#include <string>

#ifdef IO_H
#define IO_H
//flow
typedef struct flow{
    void If((int a || float a || std::string a),
     (int b || float b || std::string b));
    void Else((int a || float a || std::string a),
    (int b || float b || std::string b));;
    void Elseif((int a || float a || std::string a),
    (int b || float b || std::string b));
};

flow flow;

#endif

#pragma once
#include <string>

void print(const std::string& msg);
std::string input(const std::string& prompt = "");