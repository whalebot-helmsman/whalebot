#pragma once

#include <gtest/gtest.h>
#include <mapping_traits.h>
#include <pod_mappers.h>

static const unsigned int kDefualtMappingVectorSize    =   10000;

namespace korm {
namespace test {

template<class T>
bool testTMapping(const T& testValue)
{
    T                                              result;
    typename CMappingTraits<T>::CDefaultMapper     mapper(testValue);
    typename CMappingTraits<T>::CDefaultUnMapper   unMapper(result);

    unMapper.unMap(mapper.dataPointer(), mapper.dataSize());

    EXPECT_TRUE(testValue == result);
}

template<class T>
bool testTVectorMapping()
{
    std::vector<T>  testValue(kDefualtMappingVectorSize, static_cast<T>(0));

    for (unsigned int i = 0; i != kDefualtMappingVectorSize; ++i) {
        testValue[i]    =   static_cast<T>(i);
    }
    return testTMapping<typename std::vector<T> >(testValue);
}

TEST(MappingTest, CharTest)
{
    testTMapping<int>(24);
}

TEST(MappingTest, IntTest)
{
    testTMapping<int>(-32);    
}

TEST(MappingTest, UIntTest)
{
    testTMapping<unsigned int>(8);
}

TEST(MappingTest, FloatTest)
{
    testTMapping<float>(4.4);
}

TEST(MappingTest, DoubleTest)
{
    testTMapping<double>(0.5456677);
}

TEST(MappingTest, PairTest)
{
    testTMapping(std::make_pair(4.4, 5));
}

TEST(MappingTest, StringTest)
{
    std::string testValue("test_test_test_test_test_test_test_test");
    testTMapping(testValue);
}

TEST(MappingTest, WStringTest)
{
    std::wstring testValue(L"test_test_test_test_test_test_test_test");
    testTMapping(testValue);
}

TEST(MappingTest, VectorCharTest)
{
    testTVectorMapping<char>();
}

TEST(MappingTest, VectorIntTest)
{
    testTVectorMapping<int>();
}

TEST(MappingTest, VectorDoubleTest)
{
    testTVectorMapping<double>();
}

TEST(MappingTest, VectorFloatTest)
{
    testTVectorMapping<float>();
}
    
}//test
}// korm 
