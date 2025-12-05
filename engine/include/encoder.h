#pragma once
#include <string>
#include <vector>
#include <fstream>

class Encoder
{
public:
    std::string encodeToBase64(const unsigned char* dataToEncode, size_t len);
    std::string encodePNGToB64(const std::string& filePath);
};