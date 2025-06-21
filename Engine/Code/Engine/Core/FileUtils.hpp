#pragma once

#include <vector>
#include <string>

//------------------------------------------------------------------------------------------------------------------

bool FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename);
bool FileReadToString(std::string& outBuffer, const std::string& filename);