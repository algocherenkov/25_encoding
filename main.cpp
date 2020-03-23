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

//BOOST_AUTO_TEST_CASE(RLE_test_simple_data)
//{
//    CA::RLEcompress("rle_test.in", "rle_test.cmp");
//    CA::RLEunpack("rle_test.cmp", "rle_test.out");
//}

BOOST_AUTO_TEST_CASE(RLE_test_real_data)
{
    CA::RLEcompress("basset", "basset.cmp");
    CA::RLEunpack("basset.cmp", "basset.out");
}
BOOST_AUTO_TEST_SUITE_END()