#include <iostream>
#include "mapping_test.h"
#include "writing_test.h"

using namespace korm::test;

int main ( int argc, char* argv[] )
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
