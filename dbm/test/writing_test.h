#pragma once

#include <gtest/gtest.h>
#include <writing_traits.h>
#include <pod_writers.h>



static const unsigned int kDefualtWritingVectorSize =   10000;
static const size_t       kBufferSize               =   kDefualtWritingVectorSize * sizeof(double) + sizeof(size_t);
char  gBuffer[kBufferSize];

namespace korm {
namespace test {

template<class T>
bool testTWriting(const T& testValue)
{
    T                                           result;
    typename CWritingTraits<T>::CDefaultWriter  writer(testValue);
    typename CWritingTraits<T>::CDefaultReader  reader(result);

    writer.write(gBuffer);
    reader.read(gBuffer);

    EXPECT_TRUE(testValue == result);
}

template<class T>
bool testTVectorWriting()
{
    std::vector<T>  testValue(kDefualtWritingVectorSize, static_cast<T>(0));

    for (unsigned int i = 0; i != kDefualtMappingVectorSize; ++i) {
        testValue[i]    =   static_cast<T>(i);
    }
    return testTWriting<typename std::vector<T> >(testValue);
}

TEST(WritingTest, CharTest)
{
    testTWriting<int>(24);
}

TEST(WritingTest, IntTest)
{
    testTWriting<int>(-32);
}

TEST(WritingTest, UIntTest)
{
    testTWriting<unsigned int>(8);
}

TEST(WritingTest, FloatTest)
{
    testTWriting<float>(4.4);
}

TEST(WritingTest, DoubleTest)
{
    testTWriting<double>(0.5456677);
}

TEST(WritingTest, StringTest)
{
    std::string testStr("test_test_test_test");
    testTWriting(testStr);
}

TEST(WritingTest, WStringTest)
{
    std::wstring testValue(L"test_test_test_test_test_test_test_test");
    testTMapping(testValue);
}

TEST(WritingTest, VectorCharTest)
{
    testTVectorWriting<char>();
}

TEST(WritingTest, VectorIntTest)
{
    testTVectorWriting<int>();
}

TEST(WritingTest, VectorDoubleTest)
{
    testTVectorWriting<double>();
}

TEST(WritingTest, VectorFloatTest)
{
    testTVectorWriting<float>();
}

}//test
}// korm
