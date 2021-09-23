#include "sstable.h"

KVPair::KVPair(int key, string value) 
{
    this->key = key;
    this->value = value;
}

int KVPair::getKey()
{
    return this->key;
}

string KVPair::getValue()
{
    return this->value;
}

SSTable::SSTable(int time, int nkeys, vector<KVPair> pairs)
{
    this->time = time;
    this->nkeys = nkeys;
    this->pairs = pairs;
}

vector<KVPair> SSTable::getPairs()
{
    return this->pairs;
}

int SSTable::getTime()
{
    return this->time;
}