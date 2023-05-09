#include <gtest/gtest.h>
#include <iostream>
#include <libtech/hashmap.hpp>

template<auto& os>
class Tracer {
    public:
    Tracer() : {os << "Tracer()\n";}
    Tracer(const Tracer& t) {os << "Tracer(const Tracer&)\n";}
    Tracer(Tracer&& t) {os << "Tracer(Tracer&&)\n";}
    Tracer& operator=(const Tracer& t) {os << "operator=(const Tracer&)\n"; return *this;}
    Tracer& operator=(Tracer&& t) {os << "operator=(Tracer&&)\n"; return *this;}
    ~Tracer() {os << "~Tracer()\n";}
};

template<auto& os>
class StringTracer {
    public:
    std::string str;
    StringTracer(const std::string& s) : str(s) {os << "StringTracer(const std::string&)\n";}
    StringTracer() {os << "StringTracer()\n";}
    StringTracer(const StringTracer& t) : str(t.str) {os << "StringTracer(const StringTracer&)\n";}
    StringTracer(StringTracer&& t) : str(std::move(t.str)) {os << "StringTracer(StringTracer&&)\n";}
    StringTracer& operator=(const StringTracer& t) {
        str = t.str;
        os << "operator=(const StringTracer&)\n";
        return *this;
    }
    StringTracer& operator=(StringTracer&& t) {
        str = std::move(t.str);
        os << "operator=(StringTracer&&)\n";
        return *this;
    }
    ~StringTracer() {os << "~StringTracer()\n";}
    
}

template<class Os, auto& tos>
Os& operator<<(Os& os, const Tracer<tos>& tracer) {
    return os << "Tracer\n";
}
template<class Os, auto& tos>
Os& operator<<(Os& os, const StringTracer<tos>& stracer) {
    return os << stracer.str;
}

namespace std {
    template<auto Os>
    class hash<StringTracer<Os>> {
        std::uint64_t operator()(const StringTracer<Os>& stracer) const {
            return std::hash<std::string>{}(stracer.str);
        }
    };
}

template<auto Os>
class myhash {
public:
    std::uint64_t operator()(const StringTracer<Os>& stracer) const {
        return std::hash<std::string>{}(stracer.str);
    }
};



TEST(HashMapTest, AddElementTest) {
    tech::HashMap<std::string, int> map;
    map["somename"] = 3;
    ASSERT_EQ(map["somename"], 3);
}

TEST(HashMapTest, DefaultValuesTest) {
    tech::HashMap<std::string, std::string> my_map;
    std::unordered_map<std::string, std::string> std_map;
    ASSERT_EQ(my_map, std_map);
    ASSERT_EQ(my_map.buckets_count(), std_map.buckets_count());
    ASSERT_EQ(my_map.max_load_factor(), std_map.max_load_factor());
    ASSERT_EQ(my_map.size(), std_map.size());
    ASSERT_EQ(my_map.load_factor(), std_map.load_factor());
}

TEST(HashMapTest, RehashMapTest) {
    std::stringstream sreal;
    std::stringstream sexpected;
    tech::HashMap<StringTracer<sreal>, Tracer<sreal>> my_map;
    std::unordered_map<StringTracer<sexpected>, Tracer<sexpected>> std_map;
    my_map["key1"] = Tracer();
    std_map["key1"] = Tracer();
    Tracer t;
    my_map["2"] = t;
    std_map["2"] = t;
    my_map.max_load_factor(10.0);
    my_map.rehash(1);
    sreal << my_map.bucket_count();
    std_map.max_load_factor(10.0);
    std_map.rehash(1);
    sexpected << std_map.bucket_count();
    ASSERT_EQ(sreal.str(), sexpected.str());
}

TEST(HashMapTest, RangeBaseForTest) {
    std::stringstream sreal;
    std::stringstream sexpected;
    tech::HashMap<StringTracer<sreal>, Tracer<sreal>> my_map;
    std::unordered_map<StringTracer<sexpected>, Tracer<sexpected>> std_map;
    my_map["key1"] = Tracer();
    std_map["key1"] = Tracer();
    Tracer t;
    my_map["2"] = t;
    std_map["2"] = t;
    for (const auto& [key, value] : my_map) {
        sreal << key.str << ' ' << value << ' ';
    }
    for (const auto& [key, value] : std_map) {
        sexpected << key.str << ' ' << value << ' '
    }
    ASSERT_EQ(sreal.str(), sexpected.str());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}