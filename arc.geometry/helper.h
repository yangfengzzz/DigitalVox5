//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cmath>
#include <cstdlib>
#include <functional>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace arc {
namespace utility {

/// hash_tuple defines a general hash function for std::tuple
/// See this post for details:
///   http://stackoverflow.com/questions/7110301
/// The hash_combine code is from boost
/// Reciprocal of the golden ratio helps spread entropy and handles duplicates.
/// See Mike Seymour in magic-numbers-in-boosthash-combine:
///   http://stackoverflow.com/questions/4948780

template <typename TT>
struct hash_tuple {
    size_t operator()(TT const& tt) const { return std::hash<TT>()(tt); }
};

namespace {

template <class T>
inline void hash_combine(std::size_t& seed, T const& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
    static void apply(size_t& seed, Tuple const& tuple) {
        HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
        hash_combine(seed, std::get<Index>(tuple));
    }
};

template <class Tuple>
struct HashValueImpl<Tuple, 0> {
    static void apply(size_t& seed, Tuple const& tuple) { hash_combine(seed, std::get<0>(tuple)); }
};

}  // unnamed namespace

template <typename... TT>
struct hash_tuple<std::tuple<TT...>> {
    size_t operator()(std::tuple<TT...> const& tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
        return seed;
    }
};

template <typename T>
struct hash_eigen {
    std::size_t operator()(T const& matrix) const {
        size_t seed = 0;
        for (int i = 0; i < (int)matrix.size(); i++) {
            auto elem = *(matrix.data() + i);
            seed ^= std::hash<typename T::Scalar>()(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

// Hash function for enum class for C++ standard less than C++14
// https://stackoverflow.com/a/24847480/1255535
struct hash_enum_class {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

/// Function to split a string, mimics boost::split
/// http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& delimiters = " ",
                                     bool trim_empty_str = true);

/// Returns true of the source string contains the destination string.
/// \param src Source string.
/// \param dst Destination string.
bool ContainsString(const std::string& src, const std::string& dst);

/// Returns true if \p src starts with \p tar.
/// \param src Source string.
/// \param tar Target string.
bool StringStartsWith(const std::string& src, const std::string& tar);

/// Returns true if \p src ends with \p tar.
/// \param src Source string.
/// \param tar Target string.
bool StringEndsWith(const std::string& src, const std::string& tar);

std::string JoinStrings(const std::vector<std::string>& strs, const std::string& delimiter = ", ");

/// String util: find length of current word staring from a position
/// By default, alpha numeric chars and chars in valid_chars are considered
/// as valid characters in a word
size_t WordLength(const std::string& doc, size_t start_pos, const std::string& valid_chars = "_");

std::string& LeftStripString(std::string& str, const std::string& chars = "\t\n\v\f\r ");

std::string& RightStripString(std::string& str, const std::string& chars = "\t\n\v\f\r ");

/// Strip empty characters in front and after string. Similar to Python's
/// str.strip()
std::string& StripString(std::string& str, const std::string& chars = "\t\n\v\f\r ");

/// Convert string to the lower case
std::string ToLower(const std::string& s);

/// Convert string to the upper case
std::string ToUpper(const std::string& s);

/// Format string
template <typename... Args>
inline std::string FormatString(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
    if (size_s <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(),
                       buf.get() + size - 1);  // We don't want the '\0' inside
};

void Sleep(int milliseconds);

/// Computes the quotient of x/y with rounding up
inline int DivUp(int x, int y) {
    div_t tmp = std::div(x, y);
    return tmp.quot + (tmp.rem != 0 ? 1 : 0);
}

/// \class UniformRandIntGenerator
///
/// \brief Draw pseudo-random integers bounded by min and max (inclusive)
/// from a uniform distribution
class UniformRandIntGenerator {
public:
    UniformRandIntGenerator(const int min, const int max, std::mt19937::result_type seed = std::random_device{}())
        : distribution_(min, max), generator_(seed) {}
    int operator()() { return distribution_(generator_); }

protected:
    std::uniform_int_distribution<int> distribution_;
    std::mt19937 generator_;
};

/// Returns current time stamp.
std::string GetCurrentTimeStamp();

}  // namespace utility
}  // namespace arc
