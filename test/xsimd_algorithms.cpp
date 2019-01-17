/***************************************************************************
* Copyright (c) 2016, Wolf Vollprecht, Johan Mabille and Sylvain Corlay    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <fstream>
#include <iostream>
#include <numeric>

#include "xsimd/xsimd.hpp"

#include "gtest/gtest.h"

struct binary_functor
{
    template <class T>
    T operator()(const T& a, const T& b) const
    {
        return a + b;
    }
};

struct unary_functor
{
    template <class T>
    T operator()(const T& a) const
    {
        return -a;
    }
};

TEST(xsimd, binary_transform)
{
    std::vector<double> expected(93);

    std::vector<double> a(93, 123), b(93, 123), c(93);
    std::vector<double, xsimd::aligned_allocator<double, XSIMD_DEFAULT_ALIGNMENT>> aa(93, 123), ba(93, 123), ca(93);

    std::transform(a.begin(), a.end(), b.begin(), expected.begin(),
                    binary_functor{});

    xsimd::transform(a.begin(), a.end(), b.begin(), c.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), ba.begin(), c.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), b.begin(), c.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(a.begin(), a.end(), ba.begin(), c.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), ba.begin(), ca.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), ca.begin()) && expected.size() == ca.size());
    std::fill(ca.begin(), ca.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), b.begin(), ca.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), ca.begin()) && expected.size() == ca.size());
    std::fill(ca.begin(), ca.end(), -1); // erase

    xsimd::transform(a.begin(), a.end(), ba.begin(), ca.begin(),
                     binary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), ca.begin()) && expected.size() == ca.size());
    std::fill(ca.begin(), ca.end(), -1); // erase
}


TEST(xsimd, unary_transform)
{
    std::vector<double> expected(93);
    std::vector<double> a(93, 123), c(93);
    std::vector<double, xsimd::aligned_allocator<double, XSIMD_DEFAULT_ALIGNMENT>> aa(93, 123), ca(93);

    std::transform(a.begin(), a.end(), expected.begin(),
                   unary_functor{});

    xsimd::transform(a.begin(), a.end(), c.begin(),
                     unary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), c.begin(),
                     unary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), c.begin()) && expected.size() == c.size());
    std::fill(c.begin(), c.end(), -1); // erase

    xsimd::transform(a.begin(), a.end(), ca.begin(),
                     unary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), ca.begin()) && expected.size() == ca.size());
    std::fill(ca.begin(), ca.end(), -1); // erase

    xsimd::transform(aa.begin(), aa.end(), ca.begin(),
                     unary_functor{});
    EXPECT_TRUE(std::equal(expected.begin(), expected.end(), ca.begin()) && expected.size() == ca.size());
    std::fill(ca.begin(), ca.end(), -1); // erase
}



class xsimd_reduce : public ::testing::Test
{
public:
    using aligned_vec_t = std::vector<double, xsimd::aligned_allocator<double, XSIMD_DEFAULT_ALIGNMENT>>;

    static constexpr std::size_t num_elements = 4 * xsimd::simd_traits<double>::size;

    aligned_vec_t  vec = aligned_vec_t(num_elements, 123.);
    double         init = 1337.;

    struct multiply
    {
        template <class T>
        T operator()(const T& a, const T& b) const
        {
            return a * b;
        }
    };
};

TEST_F(xsimd_reduce, unaligned_begin_unaligned_end)
{
    auto const begin = std::next(vec.begin());
    auto const end = std::prev(vec.end());

    EXPECT_EQ(std::accumulate(begin, end, init), xsimd::reduce(begin, end, init));
}

TEST_F(xsimd_reduce, unaligned_begin_aligned_end)
{
    auto const begin = std::next(vec.begin());
    auto const end = vec.end();

    EXPECT_EQ(std::accumulate(begin, end, init), xsimd::reduce(begin, end, init));
}

TEST_F(xsimd_reduce, aligned_begin_unaligned_end)
{
    auto const begin = vec.begin();
    auto const end = std::prev(vec.end());

    EXPECT_EQ(std::accumulate(begin, end, init), xsimd::reduce(begin, end, init));
}

TEST_F(xsimd_reduce, aligned_begin_aligned_end)
{
    auto const begin = vec.begin();
    auto const end = vec.end();

    EXPECT_EQ(std::accumulate(begin, end, init), xsimd::reduce(begin, end, init));
}

TEST_F(xsimd_reduce, using_custom_binary_function)
{
    auto const begin = vec.begin();
    auto const end = vec.end();

    EXPECT_DOUBLE_EQ(std::accumulate(begin, end, init, multiply{}), xsimd::reduce(begin, end, init, multiply{}));
}
