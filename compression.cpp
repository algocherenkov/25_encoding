#include "compression.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

constexpr int BLOCK_SIZE = 256;
constexpr int PSEUDO_REPEATED_LENGTH = 2;

void CA::RLEcompress(std::string source, std::string outputName)
{
    char buffer[BLOCK_SIZE];

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

            bool prevSeriesWasSingles = false;

            int counterRS = 0;
            int counterSS = 0;            

            for(int i = 0; i < readBytes - 1; i++)
            {
                if(buffer[i] == buffer[i + 1])
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

                if(counterRS && repeatedSeriesEnded)
                {
                    counterRS++;

                    if(prevSeriesWasSingles && counterRS == PSEUDO_REPEATED_LENGTH)
                    {
                        prevSeriesWasSingles = false;
                        std::string additionalData(PSEUDO_REPEATED_LENGTH, buffer[i]);
                        results[results.size() - 1].second.append(additionalData);
                        results[results.size() - 1].first -= PSEUDO_REPEATED_LENGTH; //because there are negative number
                    }
                    else
                        results.emplace_back(counterRS, std::string(buffer + i, buffer + i + 1));

                    repeatedSeriesEnded = false;
                    singleSeriesEnded = false;

                    counterRS = 0;
                    counterSS = 0;
                }
                else if(counterSS && singleSeriesEnded)
                {
                    auto k = -1 * counterSS;

                    std::string series;
                    for(int j = counterSS; j > 0; j--)
                        series.push_back(buffer[i - j]);

                    results.emplace_back(k, series);

                    repeatedSeriesEnded = false;
                    singleSeriesEnded = false;

                    counterRS = 0;
                    counterSS = 0;

                    prevSeriesWasSingles = true;

                    i -= 1; //necessary step back
                }
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
        // The end of the file
        fseek(fin, 0, SEEK_END);
        // Get size of the file
        double m_file_size = ftell(fin);
        // Go to start
        rewind(fin);

        std::string result;

        // Read and write by "BLOCK_SIZE" bytes
        for(size_t i = 0; i < std::ceil(m_file_size / BLOCK_SIZE); ++i)
        {
            // Read "BLOCK_SIZE" bytes to buffer
            auto readBytes = fread(buffer, sizeof(unsigned char), BLOCK_SIZE, fin);

            for(int i = 0; i < readBytes; i++)
            {
                if(buffer[i] > 0)
                {
                    if(i == readBytes - 1)
                    {
                        auto pos = ftell(fin);
                        if(fseek(fin, pos - 1, SEEK_SET) != 0 && ferror(fin))
                            std::cerr << "error during reading the file" << std::endl;
                        i = readBytes;
                    }
                    else
                    {
                        result += std::string(buffer[i], buffer[i + 1]);
                        i++;
                    }
                }
                else
                {
                    if((readBytes - i) < std::abs(buffer[i]))
                    {
                        auto pos = ftell(fin);
                        if(fseek(fin, pos - (readBytes - i), SEEK_SET) != 0 && ferror(fin))
                            std::cerr << "error during reading the file" << std::endl;
                        i = readBytes;
                    }
                    else
                    {
                        result += std::string(buffer + i + 1, buffer + i + 1 + std::abs(buffer[i]));
                        i+= std::abs(buffer[i]);
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
