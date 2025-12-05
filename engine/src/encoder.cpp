#include "encoder.h"

std::string Encoder::encodeToBase64(const unsigned char* dataToEncode, size_t len)
{

    static const char* base64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


    std::string temp;
    int i = 0;
    int j = 0;
    unsigned char charArray3[3];
    unsigned char charArray4[4];

    while (len--)
    {
        charArray3[i++] = *(dataToEncode++);
        if (i == 3)
        {
            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (i = 0; i < 4; i++)
            {
                temp += base64Chars[charArray4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
        {
            charArray3[j] = '\0';
        }

        charArray4[0] = (charArray3[0] & 0xfc) >> 2;
        charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
        charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
        charArray4[3] = charArray3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
        {
            temp += base64Chars[charArray4[j]];
        }

        while ((i++ < 3))
        {
            temp += '=';
        }
    }

    return temp;
}

std::string Encoder::encodePNGToB64(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return "";
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());
    return encodeToBase64(buffer.data(), buffer.size());
}