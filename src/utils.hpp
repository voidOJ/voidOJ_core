#pragma once

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>


enum OJ {AC,WA,TLE,OLE,MLE,RE,PE,CE};


std::string gen_tempfile_name(std::string suffix);
std::string gen_time_now();
int int_execute(std::string command);