#include "gtest/gtest.h"
#include "../trie.h"

TEST(Creation, Basic) {
	Trie tr;
	EXPECT_EQ(tr.Size(), 0);
	EXPECT_EQ(tr.NumNodes(), 0);
}
TEST(Creation, TwoWordsInit) {
	Trie tr{ "XYZ", "ABC" };
	EXPECT_EQ(tr.Size(), 2);
	EXPECT_EQ(tr.NumNodes(), 6);
}

TEST(Creation, ThreeWordsInit) {
	Trie tr{ "XYZ", "ABC", "ABDD" };
	EXPECT_EQ(tr.Size(), 3);
	EXPECT_EQ(tr.NumNodes(), 8);
}

TEST(Insert, TwoWordsChain) {
	Trie tr;
	tr.Insert("ABC");
	EXPECT_EQ(tr.Size(), 1);
	EXPECT_EQ(tr.NumNodes(), 3);
	tr.Insert("ABCD");
	EXPECT_EQ(tr.Size(), 2);
	EXPECT_EQ(tr.NumNodes(), 4);
}

TEST(Insert, ThreeWords) {
	Trie tr;
	tr.Insert("ABC");
	EXPECT_EQ(tr.Size(), 1);
	EXPECT_EQ(tr.NumNodes(), 3);
	tr.Insert("ABD");
	tr.Insert("XYZ");
	EXPECT_EQ(tr.Size(), 3);
	EXPECT_EQ(tr.NumNodes(), 7);
}

TEST(Remove, TwoWordsFast) {
	Trie tr{ "XYZ", "ABC" };
	tr.Remove("XYZ");
	tr.Remove("ABC");
	EXPECT_EQ(tr.Size(), 0);
	EXPECT_EQ(tr.NumNodes(), 6);
}

TEST(Remove, TwoWordsFull) {
	Trie tr{ "XYZ", "ABC" };
	EXPECT_EQ(tr.NumNodes(), 6);
	tr.RemoveAndDeleteNodes("XYZ");
	tr.RemoveAndDeleteNodes("ABC");
	EXPECT_EQ(tr.Size(), 0);
	EXPECT_EQ(tr.NumNodes(), 0);
}

TEST(Remove, ThreeWordsFull) {
	Trie tr{ "XYZ", "ABC", "ABDD" };
	EXPECT_EQ(tr.NumNodes(), 8);
	tr.RemoveAndDeleteNodes("XYZ");
	tr.RemoveAndDeleteNodes("ABC");
	EXPECT_EQ(tr.Size(), 1);
	EXPECT_EQ(tr.NumNodes(), 4); // ABDD was left
}