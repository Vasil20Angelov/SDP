#pragma once
#include <iostream>
#include <vector>
#include <forward_list>
#include <string>
#include <set>

using std::string;
using std::forward_list;
using std::vector;

struct Data
{
    string fKey;
    int count;

    Data(const string& key = "", int cnt = 0) : fKey(key), count(cnt) {}
};

struct Iterator
{
    forward_list<Data>::iterator it;
    forward_list<Data>::iterator before_it;
    int index;
    Iterator(const forward_list<Data>::iterator& bItr, const forward_list<Data>::iterator& itr, int idx)
        : before_it(bItr), it(itr), index(idx) {}
};

class HashMap
{
public:
    using Hash = std::hash<string>;
    const size_t DEFAULT_BUCKET_COUNT = 8;
    const float  DEFAULT_MAX_LOAD_FACTOR = 0.9;

    HashMap() : fArr(DEFAULT_BUCKET_COUNT), fSize(0), fMaxLoadFactor(DEFAULT_MAX_LOAD_FACTOR) {}
    ~HashMap() = default;

    bool insert(const Data& pair)
    {
        Data* data = find(pair.fKey);
        if (data == nullptr)
            this->add(pair);
        else
            data->count += pair.count;

        return true;
    }

    bool erase(const string& key)
    {
        size_t index = this->hash(key);

        forward_list<Data>::iterator eraserIt = fArr[index].before_begin();
        forward_list<Data>::iterator comparerIt = fArr[index].begin();

        while (comparerIt != fArr[index].end())
        {
            if (comparerIt->fKey == key)
            {
                fArr[index].erase_after(eraserIt);
                --fSize;
                return true;
            }
            ++eraserIt;
            ++comparerIt;
        }

        return false;
    }

    void clear()
    {
        fArr.clear();
        fSize = 0;
    }

    bool contains(const string& key) const
    {
        if (this->find(key))
            return true;

        return false;
    }

    size_t size() const
    {
        return fSize;
    }

    size_t bucket_count() const
    {
        return fArr.size();
    }

    float load_factor() const
    {
        return (float)this->size() / this->bucket_count();
    }

    float max_load_factor() const
    {
        return fMaxLoadFactor;
    }

    void max_load_factor(float factor)
    {
        fMaxLoadFactor = factor;
        size_t  minBuckets = std::ceil(this->size() / this->max_load_factor());

        if (this->bucket_count() < minBuckets)
            this->rehash(minBuckets);
    }

    void rehash(size_t bucketsCount)
    {
        size_t minBuckets = std::ceil(this->size() / this->max_load_factor());
        if (bucketsCount < minBuckets)
            bucketsCount = minBuckets;

        if (this->bucket_count() == bucketsCount)
            return;

        vector<forward_list<Data>> newArr(bucketsCount);
        std::swap(fArr, newArr);
        fSize = 0;

        for (const std::forward_list< Data >& list : newArr)
            for (const Data& elem : list)
                this->insert(elem);
    }

    const Data* find(const string& key) const
    {
        size_t index = this->hash(key);

        for (const Data& elem : fArr[index])
            if (elem.fKey == key)
                return &elem;

        return nullptr;
    }

    Data* find(const string& key)
    {
        return const_cast<Data*>(((const HashMap*)this)->find(key));
    }

    std::multiset<std::string> GetValues() const
    {
        std::multiset<std::string> values;
        for (forward_list<Data> fwData : fArr)
        {
            for (Data& data : fwData)
            {
                // Insert the word as many times in the multiset as it is found in the hash-map
                for (int i = 0; i < data.count; ++i)
                    values.insert(data.fKey);
            }
        }

        return values;
    }

    // Removes the non-unique words from both hash-maps simultaneously
    void removeNonUnique(HashMap& other)
    {
        // Iterate only through 1 hash-map (*this)
        vector<forward_list<Data>>::iterator vecIt = fArr.begin();
        while (vecIt != fArr.end())
        {
            forward_list<Data>::iterator beforeIt = vecIt->before_begin();
            forward_list<Data>::iterator it = vecIt->begin();
            while (it != vecIt->end())
            {
                // Try to get an element from the 2nd hash-map with key - the current iterator's key
                Iterator otherIt = other.getIt(it->fKey);
                if (otherIt.index != -1) // Check if such element exists
                {   // Compare the occurances
                    
                    // In case they are equal in both hash-maps -> the word is not uniqe for any of them so remove the current word
                    if (otherIt.it->count == it->count)
                    {
                        it = vecIt->erase_after(beforeIt); // Delete the word from the 1st hash-map
                        other.fArr[otherIt.index].erase_after(otherIt.before_it); // Delete the word from the 2nd hash-map
                        --fSize;
                        --other.fSize;
                        continue;
                    }

                    // In case a word is found more times in the 2nd hash-map, delete it from the 1st and reduce the ocurrances in the 2nd hash-map
                    if (it->count < otherIt.it->count)
                    {
                        otherIt.it->count -= it->count; // Reduce the occurances count
                        it = vecIt->erase_after(beforeIt); // Delete the word from the 1st hash-map
                        --fSize;
                        continue;
                    }

                    // In case a word is found more times in the 1st hash-map
                    it->count -= otherIt.it->count; // Reduce the occirances count
                    other.fArr[otherIt.index].erase_after(otherIt.before_it); // Delete the word from the 2nd hash-map
                    --other.fSize;

                    // In case any of the hash-maps is already empty - terminate the process
                    if (fSize == 0 || other.fSize == 0)
                        return;
                }

                ++it; // Increment the current iterator if it hasn't been already changed in the steps above
            }

            ++vecIt;
        }
       
    }

private:
    size_t hash(const string& key) const
    {
        return fHashFunc(key) % this->bucket_count();
    }

    Data* add(const Data& pair)
    {
        size_t index = this->hash(pair.fKey);

        if ((float)(this->size() + 1) / this->bucket_count() > this->max_load_factor())
            this->rehash(this->bucket_count() * 2);

        fArr[index].push_front(pair);

        ++fSize;
        return &fArr[index].front();
    }

    // Finds an element with given key and returns it's iterator, the iterator before it and the with what bucket it is found
    Iterator getIt(const string& key)
    {
        size_t index = this->hash(key);

        forward_list<Data>::iterator beforeIt = fArr[index].before_begin();
        forward_list<Data>::iterator comparerIt = fArr[index].begin();

        while (comparerIt != fArr[index].end())
        {
            if (comparerIt->fKey == key)
                return Iterator(beforeIt, comparerIt, index);

            ++beforeIt;
            ++comparerIt;
        }

        return Iterator(fArr[0].end(), fArr[0].end(), -1); // Something invalid if it is not found
    }

    vector<forward_list<Data>> fArr;
    Hash   fHashFunc;
    size_t fSize;
    float  fMaxLoadFactor;
};

// Based on: https://github.com/peshe/SDP20-21/blob/master/exercises/2%264/Data%20Structures/HashTables/HashMap.hpp