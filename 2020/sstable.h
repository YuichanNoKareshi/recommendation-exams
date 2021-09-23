#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class KVPair 
{
private:
    int key;
    string value; 

public:
    KVPair(int key, string value);
    ~KVPair(){}
    int getKey();
    string getValue();
};

class SSTable
{
private:
    int time;
    int nkeys;
    vector<KVPair> pairs;
public:
    SSTable(int time, int nkeys, vector<KVPair> pairs);
    ~SSTable(){}
    vector<KVPair> getPairs();
    int getTime();
};