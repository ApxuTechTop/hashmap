#include <gtest/gtest.h>
#include <iostream>
#include <libtech/hashmap.hpp>
#include <list>
#include <sstream>
#include <unordered_map>
#include <vector>

std::stringstream sreal;
std::stringstream sexpected;

struct my_Tracer {
	int value;
	my_Tracer(int v = 0) : value(v) { sreal << "Tracer(" << value << ")\n"; }
	my_Tracer(const my_Tracer& t) : value(t.value) {
		sreal << "Tracer(const Tracer& " << value << ")\n";
	}
	my_Tracer(my_Tracer&& t) noexcept : value(std::move(t.value)) {
		sreal << "Tracer(Tracer&& " << value << ")\n";
	}
	my_Tracer& operator=(const my_Tracer& t) {
		value = t.value;
		sreal << "operator=(const Tracer& " << value << ")\n";
		return *this;
	}
	my_Tracer& operator=(my_Tracer&& t) noexcept {
		value = std::move(t.value);
		sreal << "operator=(Tracer&& " << value << ")\n";
		return *this;
	}
	~my_Tracer() { sreal << "~Tracer(" << value << ")\n"; }
};

struct std_Tracer {
	int value;
	std_Tracer(int v = 0) : value(v) {
		sexpected << "Tracer(" << value << ")\n";
	}
	std_Tracer(const std_Tracer& t) : value(t.value) {
		sexpected << "Tracer(const Tracer& " << value << ")\n";
	}
	std_Tracer(std_Tracer&& t) noexcept : value(std::move(t.value)) {
		sexpected << "Tracer(Tracer&& " << value << ")\n";
	}
	std_Tracer& operator=(const std_Tracer& t) {
		value = t.value;
		sexpected << "operator=(const Tracer& " << value << ")\n";
		return *this;
	}
	std_Tracer& operator=(std_Tracer&& t) noexcept {
		value = std::move(t.value);
		sexpected << "operator=(Tracer&& " << value << ")\n";
		return *this;
	}
	~std_Tracer() { sexpected << "~Tracer(" << value << ")\n"; }
};

struct my_StringTracer {
	std::string str;
	my_StringTracer(const std::string& s) : str(s) {
		sreal << "StringTracer(const std::string&)\n";
	}
	my_StringTracer(const char* s) : str(s) {
		sreal << "StringTracer(const char*)\n";
	}
	my_StringTracer() { sreal << "StringTracer()\n"; }
	my_StringTracer(const my_StringTracer& t) : str(t.str) {
		sreal << "StringTracer(const StringTracer&)\n";
	}
	my_StringTracer(my_StringTracer&& t) : str(std::move(t.str)) {
		sreal << "StringTracer(StringTracer&&)\n";
	}
	my_StringTracer& operator=(const my_StringTracer& t) {
		str = t.str;
		sreal << "operator=(const StringTracer&)\n";
		return *this;
	}
	my_StringTracer& operator=(my_StringTracer&& t) noexcept {
		str = std::move(t.str);
		sreal << "operator=(StringTracer&&)\n";
		return *this;
	}
	bool operator==(const my_StringTracer& other) const {
		return (str.size() == other.str.size()) && (str == other.str);
	}
	std::strong_ordering operator<=>(const my_StringTracer& other) const {
		return str <=> other.str;
	}
	~my_StringTracer() { sreal << "~StringTracer()\n"; }
};

struct std_StringTracer {
	std::string str;
	std_StringTracer(const std::string& s) : str(s) {
		sexpected << "StringTracer(const std::string&)\n";
	}
	std_StringTracer(const char* s) : str(s) {
		sexpected << "StringTracer(const char*)\n";
	}
	std_StringTracer() { sexpected << "StringTracer()\n"; }
	std_StringTracer(const std_StringTracer& t) : str(t.str) {
		sexpected << "StringTracer(const StringTracer&)\n";
	}
	std_StringTracer(std_StringTracer&& t) noexcept : str(std::move(t.str)) {
		sexpected << "StringTracer(StringTracer&&)\n";
	}
	std_StringTracer& operator=(const std_StringTracer& t) {
		str = t.str;
		sexpected << "operator=(StringTracer&&)\n";
		return *this;
	}
	std_StringTracer& operator=(std_StringTracer&& t) noexcept {
		str = std::move(t.str);
		sexpected << "operator=(StringTracer&&)\n";
		return *this;
	}
	bool operator==(const std_StringTracer& other) const {
		return (str.size() == other.str.size()) && (str == other.str);
	}
	std::strong_ordering operator<=>(const std_StringTracer& other) const {
		return str <=> other.str;
	}
	~std_StringTracer() { sexpected << "~StringTracer()\n"; }
};

std::ostream &operator<<(std::ostream &os, const my_Tracer &tracer) {
	return os << tracer.value;
}
std::ostream &operator<<(std::ostream &os, const std_Tracer &tracer) {
	return os << tracer.value;
}
std::ostream &operator<<(std::ostream &os, const my_StringTracer &stracer) {
	return os << stracer.str;
}
std::ostream &operator<<(std::ostream &os, const std_StringTracer &stracer)
{ 	return os << stracer.str;
}

namespace std {
template <> struct hash<my_StringTracer> {
	std::uint64_t operator()(const my_StringTracer &stracer) const {
		return std::hash<std::string>{}(stracer.str);
	}
};
template <> struct hash<std_StringTracer> {
	std::uint64_t operator()(const std_StringTracer &stracer) const {
		return std::hash<std::string>{}(stracer.str);
	}
};
} // namespace std

template <class StringTracer> struct myhash {
	std::uint64_t operator()(const StringTracer& stracer) const {
		return std::hash<std::string>{}(stracer.str);
	}
};

TEST(ListTest, TwoPushBackTest) {
	tech::List<my_Tracer> my_list;
	std::list<std_Tracer> std_list;
	my_list.push_back(my_Tracer());
	std_list.push_back(std_Tracer());
	my_Tracer my_tracer;
	std_Tracer std_tracer;
	my_list.push_back(my_tracer);
	std_list.push_back(std_tracer);
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}
TEST(ListTest, EmplaceBackTest) {
	tech::List<my_StringTracer> my_list;
	std::list<std_StringTracer> std_list;
	my_list.emplace_back("somevalue");
	std_list.emplace_back("somevalue");
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(ListTest, RangeBasedForTest) {
	tech::List<int> my_list;
	std::list<int> std_list;
	for (int i = 0; i < 4; ++i) {
		my_list.push_back(i);
		std_list.push_back(i);
	}
	for (const auto& item : my_list) {
		sreal << item << ' ';
	}
	for (const auto& item : std_list) {
		sexpected << item << ' ';
	}
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(ListTest, EraseTest) {
	tech::List<int> my_list;
	std::list<int> std_list;
	for (int i = 0; i < 4; ++i) {
		my_list.push_back(i);
		std_list.push_back(i);
	}
	auto my_it = my_list.begin();
	++my_it;
	auto std_it = std_list.begin();
	++std_it;
	my_list.erase(my_it);
	std_list.erase(std_it);
	for (const auto& item : my_list) {
		sreal << item << ' ';
	}
	for (const auto& item : std_list) {
		sexpected << item << ' ';
	}
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(VectorTest, EmplaceBackTest) {
	tech::Vector<my_StringTracer> my_vector;
	std::vector<std_StringTracer> std_vector;
	my_vector.emplace_back("good");
	std_vector.emplace_back("good");
	ASSERT_EQ(my_vector.capacity(), std_vector.capacity());
	ASSERT_EQ(my_vector[0].str, std::string("good"));
	ASSERT_EQ(std_vector[0].str, std::string("good"));
	my_vector.emplace_back();
	std_vector.emplace_back();
	ASSERT_EQ(my_vector.capacity(), std_vector.capacity());
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(VectorTest, PushBackTest) {
	tech::Vector<my_StringTracer> my_vector;
	std::vector<std_StringTracer> std_vector;
	my_vector.push_back(my_StringTracer());
	std_vector.push_back(std_StringTracer());
	my_StringTracer my_stracer;
	std_StringTracer std_stracer;
	my_vector.push_back(my_stracer);
	std_vector.push_back(std_stracer);
	my_vector.push_back(std::move(my_stracer));
	std_vector.push_back(std::move(std_stracer));
	ASSERT_EQ(my_vector.capacity(), std_vector.capacity());
	ASSERT_EQ(my_vector.size(), std_vector.size());
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(VectorTest, ReserveTest) {
	tech::Vector<my_Tracer> my_vector;
	std::vector<std_Tracer> std_vector;
	ASSERT_EQ(my_vector.capacity(), 0);
	my_vector.reserve(6);
	std_vector.reserve(6);
	ASSERT_EQ(my_vector.capacity(), std_vector.capacity());
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}
TEST(VectorTest, ResizeTest) {
	tech::Vector<my_Tracer> my_vector;
	std::vector<std_Tracer> std_vector;
	my_vector.resize(9);
	std_vector.resize(9);
	ASSERT_EQ(my_vector.size(), std_vector.size());
	ASSERT_EQ(sreal.str(), sexpected.str());
}

TEST(VectorTest, RangeBasedForTest) {
	tech::Vector<int> my_vector;
	std::vector<int> std_vector;
	for (int i = 0; i < 4; ++i) {
		my_vector.push_back(i);
		std_vector.push_back(i);
	}
	for (const auto& item : my_vector) {
		sreal << item << ' ';
	}
	for (const auto& item : std_vector) {
		sexpected << item << ' ';
	}
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(HashMapTest, DefaultValuesTest) {
	tech::HashMap<std::string, std::string> my_map;
	std::unordered_map<std::string, std::string> std_map;
	ASSERT_EQ(my_map.bucket_count(), std_map.bucket_count());
	ASSERT_EQ(my_map.max_load_factor(), std_map.max_load_factor());
	ASSERT_EQ(my_map.size(), std_map.size());
	ASSERT_EQ(my_map.load_factor(), std_map.load_factor());
}

TEST(HashMapTest, AddElementTest) {
	tech::HashMap<std::string, int> map;
	map["somename"] = 3;
	ASSERT_EQ(map["somename"], 3);
	ASSERT_EQ(map.size(), 1);
}

TEST(HashMapTest, RehashMapTest) {
	tech::HashMap<my_StringTracer, my_Tracer, myhash<my_StringTracer>> my_map;
	std::unordered_map<std_StringTracer, std_Tracer, myhash<std_StringTracer>>
		std_map;

	my_map[my_StringTracer("key1")] = my_Tracer(2);
	std_map[std_StringTracer("key1")] = std_Tracer(2);

	my_Tracer my_t;
	std_Tracer std_t;
	my_map["2"] = my_t;
	std_map["2"] = std_t;
	my_map.max_load_factor(10.0);
	my_map.rehash(1);
	std_map.max_load_factor(10.0);
	std_map.rehash(1);
	ASSERT_EQ(sreal.str(), sexpected.str());
	ASSERT_EQ(my_map.bucket_count(), 1);
	ASSERT_EQ(my_map["key1"].value, 2);
	ASSERT_EQ(std_map["key1"].value, 2);
	sreal.str("");
	sexpected.str("");
}

TEST(HashMapTest, RangeBasedForTest) {
	tech::HashMap<std::string, int> my_map;
	std::unordered_map<std::string, int> std_map;
	my_map["key1"] = 1;
	std_map["key1"] = 1;
	my_map["2"] = 2;
	std_map["2"] = 2;
	my_map["key1"] = 42;
	std_map["key1"] = 42;
	int my_i = 0;
	int std_i = 0;
	bool isGood = true;
	for (const auto& [key, value] : my_map) {
		my_i++;
		if (std_map[key] != value) {
			isGood = false;
		}
	}
	for (const auto& [key, value] : std_map) {
		std_i++;
		if (my_map[key] != value) {
			isGood = false;
		}
	}
	ASSERT_EQ(my_i, std_i);
	EXPECT_TRUE(isGood);
}

TEST(HashMapTest, RehashMapTest2) {
	tech::HashMap<std::string, int> my_map;
	std::unordered_map<std::string, int> std_map;
	
	for (int i = 0; i < 10; ++i) {
		my_map[std::to_string(i)] = i;
		std_map[std::to_string(i)] = i;
	}
	my_map.max_load_factor(2.5);
	std_map.max_load_factor(2.5);
	my_map.rehash(5);
	std_map.rehash(5);
	int my_i = 0;
	int std_i = 0;
	bool isGood = true;
	for (const auto& [key, value] : my_map) {
		my_i++;
		if (std_map[key] != value) {
			isGood = false;
		}
	}
	for (const auto& [key, value] : std_map) {
		std_i++;
		if (my_map[key] != value) {
			isGood = false;
		}
	}
	ASSERT_EQ(my_map.bucket_count(), std_map.bucket_count());
	ASSERT_EQ(my_map.size(), std_map.size());
	ASSERT_EQ(my_i, std_i);
	EXPECT_TRUE(isGood);
}

TEST(HashMapTest, EmplaceTest) {
	tech::HashMap<my_StringTracer, my_Tracer> my_map;
	std::unordered_map<std_StringTracer, std_Tracer> std_map;
	my_map.emplace("em", 2);
	std_map.emplace("em", 2);
	my_map.emplace("kak", 3);
	std_map.emplace("kak", 3);
	my_map.emplace("em", 42);
	std_map.emplace("em", 42);
	
	ASSERT_EQ(my_map.at("em").value, std_map.at("em").value);
	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(HashMapTest, FindContainsTest) {
	tech::HashMap<std::string, int> my_map;
	std::unordered_map<std::string, int> std_map;
	my_map["key"] = 5;
	ASSERT_EQ(my_map.contains("key"), true);
	ASSERT_EQ(my_map.find("value"), my_map.end());
	ASSERT_EQ((my_map.find("key"))->second, 5);
}

TEST(HashMapTest, InsertTest) {
	tech::HashMap<std::string, int> my_map;
	my_map.insert({"a", 1});
	ASSERT_EQ(my_map["a"], 1);
	std::vector<std::pair<std::string, int>> tmp_vec = {{"b", 2}, {"c", 3}};
	my_map.insert(tmp_vec.begin(), tmp_vec.end());
	ASSERT_EQ(my_map["b"], 2);
	ASSERT_EQ(my_map["c"], 3);
}

TEST(HashMapTest, IteratorConstructor) {
	std::unordered_map<std::string, int> std_map = {{"a", 'a'}, {"b", 'b'}, {"c", 3}};
	tech::HashMap<std::string, int> my_map(std_map.begin(), std_map.end());
	ASSERT_EQ(my_map.size(), 3);
	for (const auto& [key, value] : std_map) {
		ASSERT_EQ(value, my_map[key]);
	}
	for (const auto& [key, value] : my_map) {
		ASSERT_EQ(value, std_map[key]);
	}
}

TEST(HashMapTest, InitializerListTest) {
	tech::HashMap<my_StringTracer, my_Tracer> my_map = {{"a", 'a'}, {"b", 'b'}, {"c", 3}};
	std::unordered_map<std_StringTracer, std_Tracer> std_map = {{"a", 'a'}, {"b", 'b'}, {"c", 3}};

	ASSERT_EQ(sreal.str(), sexpected.str());
	sreal.str("");
	sexpected.str("");
}

TEST(HashMapTest, EraseTest) {
	tech::HashMap<my_StringTracer, my_Tracer> my_map = {{"a", 'a'}, {"b", 'b'}, {"c", 3}};
	std::unordered_map<std_StringTracer, std_Tracer> std_map = {{"a", 'a'}, {"b", 'b'}, {"c", 3}};

	my_map.erase(my_map.find("b"));
	std_map.erase(std_map.find("b"));
	ASSERT_EQ(my_map.size(), std_map.size());
	for (const auto& [key, value] : std_map) {
		ASSERT_EQ(value.value, my_map[key.str].value);
	}
	for (const auto& [key, value] : my_map) {
		ASSERT_EQ(value.value, std_map[key.str].value);
	}
	ASSERT_EQ(sreal.str(), sexpected.str()); 
	sreal.str("");
	sexpected.str("");
}

TEST(HashMapTest, StdFindTest) {
	tech::HashMap<std::string, int> my_map = {{"not", -1}, {"haha", 228}, {"find me", 42}, {"qwerty", 12345}};
	std::stringstream stest;
	auto result1 = std::find_if(my_map.begin(), my_map.end(), [&stest](const std::pair<std::string, int>& p){
		stest << p.first << ':' << p.second << '\n';
		return p.second == 42;
	});
	ASSERT_FALSE(result1 == my_map.end()) << stest.str();
	ASSERT_EQ(result1->first, "find me");
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}