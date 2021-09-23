#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Record 
{
private:
    string keyword; // 搜索词 
    int count; // 出现次数
public:
    Record(){}
    Record(string keyword, int count):keyword(keyword),count(count){}
    ~Record(){}
    string getKeyword();
    int getCount();
    void addCount(int x);
};

class User
{
private:
    uint32_t ID;
    vector<Record> records;
public:
    User(uint32_t ID, vector<Record> records):ID(ID), records(records){}
    ~User(){}
    uint32_t getID();
    vector<Record> getRecords();
    void pushRecord(Record newRecord);
    void updateRecords(vector<Record> newRecords);
};