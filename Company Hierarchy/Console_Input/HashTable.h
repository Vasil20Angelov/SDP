#pragma once
#include <iostream>
#include <vector>
#include <forward_list>
#include <string>
#include "../HomeWork2/interface.h"
using std::string;
using std::forward_list;
using std::vector;

struct Data
{
    Hierarchy fValue;
    string fKey;
    bool modified;

    Data(const string& key = "", const Hierarchy& value = Hierarchy(""), bool mod = false) : fKey(key), fValue(value), modified(mod) {}
};

class HashMap
{
public:
    using Hash = std::hash<string>;
    const size_t DEFAULT_BUCKET_COUNT = 8;
    const float  DEFAULT_MAX_LOAD_FACTOR = 0.7;

    HashMap() : fArr(DEFAULT_BUCKET_COUNT), fSize(0), fMaxLoadFactor(DEFAULT_MAX_LOAD_FACTOR) {}
    ~HashMap() = default;

    bool insert(const Data& pair)
    {
        if (this->contains(pair.fKey))
            return false;

        this->add(pair);

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

    forward_list<Data*> getModified()
    {
        forward_list<Data*> modLst;
        for (forward_list<Data>& list : fArr)
        {
            for (Data& elem : list)
            {
                if (elem.modified)
                    modLst.push_front(&elem);
            }
        }

        return modLst;
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

    vector<forward_list<Data>> fArr;
    Hash   fHashFunc;
    size_t fSize;
    float  fMaxLoadFactor;
};

// Based on: https://github.com/peshe/SDP20-21/blob/master/exercises/2%264/Data%20Structures/HashTables/HashMap.hpp