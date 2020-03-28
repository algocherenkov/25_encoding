#include "compression.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

constexpr int BLOCK_SIZE = 256;
constexpr int REPEATED_SERIES_MAX_LENGTH = 127;
constexpr int SINGLE_SERIES_MAX_LENGTH = 128;

void CA::RLEcompress(std::string source, std::string outputName)
{
    char buffer[BLOCK_SIZE + 2]; // to prevent using data out of range

    FILE *fin = fopen(source.c_str(), "rb");

    if(fin != nullptr)
    {
        std::vector<std::pair<int, std::string>> results;
        // The end of the file
        fseek(fin, 0, SEEK_END);
        // Get size of the file
        double m_file_size = ftell(fin);
        // Go to start
        rewind(fin);

        // Read and write by "BLOCK_SIZE" bytes
        for(size_t i = 0; i < std::ceil(m_file_size / BLOCK_SIZE); ++i)
        {
            // Read "BLOCK_SIZE" bytes to buffer
            auto readBytes = fread(buffer, sizeof(unsigned char), BLOCK_SIZE, fin);

            bool repeatedSeriesEnded = false;
            bool singleSeriesEnded = false;

            int counterRS = 0;
            int counterSS = 0;            

            for(size_t j = 0; j < readBytes; j++)
            {
                if(buffer[j] == buffer[j + 1] &&
                        (buffer[j] == buffer[j + 2] || !counterSS || counterRS))
                {
                    counterRS++;
                    singleSeriesEnded = true;
                    repeatedSeriesEnded = false;
                }
                else
                {
                    counterSS++;
                    repeatedSeriesEnded = true;
                    singleSeriesEnded = false;
                }

                if((counterRS && repeatedSeriesEnded) || counterRS == REPEATED_SERIES_MAX_LENGTH - 1)
                {
                    counterRS++;

                    results.emplace_back(counterRS, std::string(1, buffer[j]));

                    repeatedSeriesEnded = false;
                    singleSeriesEnded = false;

                    counterRS = 0;
                    counterSS = 0;

                    if(counterRS == REPEATED_SERIES_MAX_LENGTH)
                        j++;
                }
                else if((counterSS && singleSeriesEnded) || counterSS == SINGLE_SERIES_MAX_LENGTH)
                {
                    if(counterSS == SINGLE_SERIES_MAX_LENGTH)
                        j++;
                    std::string series;
                    for(int k = counterSS; k > 0; k--)
                        series.push_back(buffer[j - k]);

                    results.emplace_back(counterSS * (-1), series);

                    repeatedSeriesEnded = false;
                    singleSeriesEnded = false;

                    counterRS = 0;
                    counterSS = 0;

                    j--; //necessary step back
                }
            }

            //in case if last series didn't pushed into the store
            if(counterRS)
                results.emplace_back(counterRS, std::string(1, buffer[readBytes - 1]));
            else if(counterSS)
            {
                std::string series;
                for(int j = counterSS; j > 0; j--)
                    series.push_back(buffer[readBytes - j]);

                results.emplace_back(counterSS * (-1), series);
            }
        }

        FILE *fout = fopen(outputName.c_str(), "wb");
        if(fout == nullptr)
        {
            std::cerr << "cannot create comressed file" << std::endl;
            return;
        }

        for(const auto& pair: results)
        {
            fputc(pair.first, fout);
            fwrite(pair.second.c_str(), static_cast<int>(sizeof (char)), pair.second.size(), fout);
        }

        fclose(fout);
    }
    else
    {
        std::cerr << "cannot open source file" << std::endl;
        return;
    }
    fclose(fin);
}

void CA::RLEunpack(std::string source, std::string outputName)
{
    char buffer[BLOCK_SIZE];

    FILE *fin = fopen(source.c_str(), "rb");

    if(fin != nullptr)
    {
        std::string result;
        size_t readBytes = 1;

        // Read and write by "BLOCK_SIZE" bytes
        while(readBytes)
        {
            // Read "BLOCK_SIZE" bytes to buffer
            readBytes = fread(buffer, sizeof(unsigned char), BLOCK_SIZE, fin);

            for(size_t j = 0; j < readBytes; j++)
            {
                if(buffer[j] > 0)
                {
                    if(j == readBytes - 1) //check for case when the border between chunks of data cuts the series
                    {
                        auto pos = ftell(fin);
                        if(fseek(fin, pos - 1, SEEK_SET) != 0 && ferror(fin))
                            std::cerr << "error during reading the file" << std::endl;
                        j = readBytes; //break the inner cycle
                    }
                    else
                    {
                        result += std::string(buffer[j], buffer[j + 1]);
                        j++;
                    }
                }
                else
                {
                    if(static_cast<int>((readBytes - j - 1)) < std::abs(buffer[j])) //check for case when the border between chunks of data cuts the series
                    {
                        auto pos = ftell(fin);
                        if(fseek(fin, pos - (readBytes - j), SEEK_SET) != 0 && ferror(fin))
                            std::cerr << "error during reading the file" << std::endl;
                        j = readBytes; //break the inner cycle
                    }
                    else
                    {
                        result += std::string(buffer + j + 1, buffer + j + 1 + std::abs(buffer[j]));
                        j+= std::abs(buffer[j]);
                    }
                }
            }
        }

        FILE *fout = fopen(outputName.c_str(), "wb");
        if(fout == nullptr)
        {
            std::cerr << "cannot create comressed file" << std::endl;
            return;
        }

        fwrite(result.c_str(), static_cast<int>(sizeof (char)), result.size(), fout);

        fclose(fout);
    }
    else
    {
        std::cerr << "cannot open comressed file" << std::endl;
        return;
    }
    fclose(fin);
}
