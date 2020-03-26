#include "compression.h"
#include <string>
#include <ctime>
#include <chrono>
#include <random>
#include <iterator>
#include <map>
#include <cstdio>

#define BOOST_TEST_MODULE test_main

#include <boost/test/included/unit_test.hpp>

using namespace boost::unit_test;
BOOST_AUTO_TEST_SUITE(test_suite_main)

BOOST_AUTO_TEST_CASE(RLE_test_simple_data)
{
    CA::RLEcompress("rle_test.in", "rle_test.cmp");
    CA::RLEunpack("rle_test.cmp", "rle_test.out");
}

BOOST_AUTO_TEST_CASE(RLE_test_real_data)
{
    CA::RLEcompress("computer", "computer.cmp");
    CA::RLEunpack("computer.cmp", "computer.out");

    CA::RLEcompress("basset", "basset.cmp");
    CA::RLEunpack("basset.cmp", "basset.out");

    CA::RLEcompress("photo.jpg", "photo.cmp");
    CA::RLEunpack("photo.cmp", "photo.out.jpg");

    CA::RLEcompress("audio.mp3", "audio.cmp");
    CA::RLEunpack("audio.cmp", "audio.out.mp3");

    CA::RLEcompress("archive.zip", "archive.cmp");
    CA::RLEunpack("archive.cmp", "archive.out.zip");
}
BOOST_AUTO_TEST_SUITE_END()
