// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <ctime>

#include <iostream>
#include <string>
#include <utility>

class Stopwatch {
public:
    explicit Stopwatch(std::string new_name)
        : name(std::move(new_name))
        , start(clock()) {
        std::cout << "Start " << name << std::endl;
    }

    void end() {
        double secs = (clock() - start) / CLOCKS_PER_SEC;
        std::cout << name << " ended in " << secs << " seconds" << std::endl;
        this->ended = true;
    }

    ~Stopwatch() {
        if (!ended) {
            this->end();
        }
    }

private:
    const std::string name;
    const double start;
    bool ended = false;
};
