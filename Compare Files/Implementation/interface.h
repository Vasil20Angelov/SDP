#pragma once
#include <iostream>
#include <set>
#include <string>
#include "HashMap.h"
using std::string;

///
/// Represents a multiset of words
///
/// If you need to see how it is intended to be used,
/// check out the corresponding unit tests
///
/// Implement all methods of this class
/// 
class WordsMultiset {
public:
	/// Adds times occurences of word to the container
	///
	/// For example, add("abc") adds the word "abc" once,
	/// while add("abc", 4) adds 4 occurrances.
	void add(const std::string& word, size_t times = 1)
	{
		try {
			container.insert(Data(word, times));
		}
		catch (...) {
			throw;
		}
	}

	/// Checks whether word is contained in the container
	bool contains(const std::string& word) const
	{
		return container.contains(word);
	}

	/// Number of occurrances of word 
	size_t countOf(const std::string& word) const
	{
		const Data* data = container.find(word);
		if (!data)
			return 0;

		return data->count;
	}

	/// Number of unique words in the container
	size_t countOfUniqueWords() const
	{
		return container.size();
	}

	/// Returns a multiset of all words in the container
	std::multiset<std::string> words() const
	{
		try {
			return container.GetValues();
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

		return std::multiset<string>();
	}

	void removeNonUniqueWords(WordsMultiset& other)
	{
		container.removeNonUnique(other.container);
	}

	void clear() 
	{
		container.clear(); 
	}

private:
	HashMap container;
};

///
/// Results of the comparison of two streams of words
/// DO NOT modify this class
/// If you need to see how it is intended to be used,
/// check out the corresponding unit tests
///
class ComparisonReport {
public:
	/// A multiset of all words that exist in both streams
	WordsMultiset commonWords;

	/// Multisets of words unique to the two streams
	/// The first element of the array contains the words that are unique
	/// to the first stream (a) and the second one -- to the second stream (b)
	WordsMultiset uniqueWords[2];
};


/// 
/// Can be used to compare two streams of words
///
class Comparator {
public:
	ComparisonReport compare(std::istream& a, std::istream& b)
	{
		try {
			init(a, b); // Reads the both streams and fills the wordsMultiset objects
		}
		catch (...) {
			throw;
		}

		try {
			// Removes the non-unique words from both wordsMultisets
			report.uniqueWords[0].removeNonUniqueWords(report.uniqueWords[1]);
		}
		catch (...) {
			report.commonWords.clear();
			report.uniqueWords[0].clear();
			report.uniqueWords[1].clear();
			throw;
		}

		/*
		* After executing removeNonUniqueWords mothod, there is a big chance many of the cells in the hash-maps to be empty.
		* I could have rehashed both maps to free some memory but as the program terminates after that method - there is no point.
		*/

		return report;
	}

	void init(std::istream& a, std::istream& b)
	{
		try {
			readFromStream(a, &Comparator::addFromFirstStream); // Reads stream a and loads report.uniqueWords[0]
			readFromStream(b, &Comparator::addFromSndStream);  // Reads stream b and loads report.uniqueWords[1] and report.commonWords
		}
		catch (...) {
			report.commonWords.clear();
			report.uniqueWords[0].clear();
			report.uniqueWords[1].clear();
			throw;
		}
	}

	// Reads a stream and fills a wordsMultiset object according to a given method
	void readFromStream(std::istream& stream, void (Comparator::*func) (const string&))
	{
		std::string str;
		stream >> str;
		if (!str.empty())
		{
			do {
				addValidWords(str, func);
			} while (stream >> str);
		}
	}

	// By given string it adds to a wordsMultiset all non-whitespace strings in the given one (skipping all \t, \r, etc..)
	void addValidWords(const string& input, void (Comparator::*func) (const string&))
	{
		string toAdd;
		for (int i = 0; i < input.size(); ++i)
		{
			if (i + 1 < input.size() && specialCharWhiteSpace(input[i], input[i + 1]))
			{
				++i;
				if (!toAdd.empty())
				{
					(this->*func)(toAdd);
					toAdd.clear();
				}
			}
			else
				toAdd += input[i];
		}
		if (!toAdd.empty())
		{
			(this->*func)(toAdd);
		}
	}

	// Checks if 2 chars form a special symbol (whitespace symbols only)
	bool specialCharWhiteSpace(char c1, char c2)
	{
		if (c1 == '\\' && (c2 == 'n' || c2 == 't' || c2 == 'r' || c2 == 'v' || c2 == 'f'))
			return true;
		return false;
	}

	int WordsCountInStream1() const { return totalWordsInStream1; }
	int WordsCountInStream2() const { return totalWordsInStream2; }
	int CommonWordsCount()	  const { return totalCommonWords; }

private:
	// Adds a word to the 1st element of report.uniqueWords
	void addFromFirstStream(const string& str)
	{
		report.uniqueWords[0].add(str);
		++totalWordsInStream1;
	}

	// While adding a word to the 2nd element of uniqueWords, add it also to the commonWords if it exists in the 1st element of uniqueWords
	void addFromSndStream(const string& str)
	{
		report.uniqueWords[1].add(str);
		// a common word's occurances count is the lesser number from both wordsMultisets 
		if (report.uniqueWords[1].countOf(str) <= report.uniqueWords[0].countOf(str))
		{
			report.commonWords.add(str);
			++totalCommonWords;
		}
		++totalWordsInStream2;
	}

	ComparisonReport report;
	int totalWordsInStream1 = 0;
	int totalWordsInStream2 = 0;
	int totalCommonWords = 0;
};