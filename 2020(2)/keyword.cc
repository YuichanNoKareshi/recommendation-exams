#include "keyword.h"
using namespace std;

string Record::getKeyword()
{
    return this->keyword;
}

int Record::getCount()
{
    return this->count;
}

void Record::addCount(int x)
{
    this->count += x;
}

uint32_t User::getID()
{
    return this->ID;
}

vector<Record> User::getRecords()
{
    return this->records;
}

void User::pushRecord(Record newRecord)
{
    this->records.push_back(newRecord);
}

void User::updateRecords(vector<Record> newRecords)
{
    this->records = newRecords;
}