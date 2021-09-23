#include <fstream>
#include <sstream>
#include "sstable.h"
using namespace std;

#define NUM 3 // 文件数
#define LARGEST_SIZE 256*1024 // 文件最大256kb

SSTable loadSSTables(string filename) 
{
    ifstream infile(filename, ios::in | ios::binary); // 二进制打开

    int fileSize, time, nkeys;
    string valueList;

    infile.read((char*)&fileSize, sizeof(int));
    infile.read((char*)&time, sizeof(int));
    infile.read((char*)&nkeys, sizeof(int)); // 读出file_size、time和nkeys

    // int valueLen = ;
    int valueLen = fileSize - (nkeys * 2 + 3) * sizeof(int);
    char* tempValue = new char[valueLen]; // 临时存放读出的所有value

    vector<int> keyList, offsetList;
    for (int i = 0; i < nkeys; i++)
    {
        int key, offset;
        infile.read((char*)&key, sizeof(int));
        infile.read((char*)&offset, sizeof(int));
        keyList.push_back(key);
        offsetList.push_back(offset);
    }
    for (int i = 0; i < valueLen; i++)
    {
        char tempCh;
        infile.read((char*)&tempCh, sizeof(char));
        tempValue[i] = tempCh;
    }
    valueList = tempValue; // char*转string，方便截取

    vector<KVPair> pairs; // 下面来构造KVPairs
    for (int i = 0; i < nkeys; i++)
    {
        int key = keyList[i];
        string value;
        if (i == 0) 
            value = valueList.substr(0, offsetList[i+1] - offsetList[i]);
        else if (i == nkeys-1)
            value = valueList.substr(offsetList[i] - offsetList[0], fileSize - offsetList[i]);
        else
            value = valueList.substr(offsetList[i] - offsetList[0], offsetList[i+1] - offsetList[i]); // 从第一个参数开始，截取第二个参数长度的字符串

        KVPair pair(key, value);
        pairs.push_back(pair);
    }

    SSTable sstable(time, nkeys, pairs);

    infile.close();
    return sstable;
}

vector<KVPair> sort2Pairs(vector<KVPair> pairs1, vector<KVPair> pairs2, int time1, int time2) // 工具函数，对两个KVPairs进行排序
{
    int cursor1 = 0, cursor2 = 0; // 分别记录每个list的游标
    vector<KVPair> resultPairs; // 要返回的排序后的vector
    while (!(cursor1 == pairs1.size() && cursor2 == pairs2.size()))
    {
        if (cursor1 == pairs1.size()) // 已经遍历了pairs1中的全部元素
        {
            while (cursor2 != pairs2.size())
                resultPairs.push_back(pairs2[cursor2++]);
            break;
        }
        else if (cursor2 == pairs2.size()) // 已经遍历了pairs2中的全部元素
        {
            while (cursor1 != pairs1.size())
                resultPairs.push_back(pairs1[cursor1++]);
            break;
        }

        if (pairs1[cursor1].getKey() == pairs2[cursor2].getKey())
            if (time1 < time2) // 1的时间小，选1
                resultPairs.push_back(pairs1[cursor1++]);
            else 
                resultPairs.push_back(pairs2[cursor2++]);
        else if (pairs1[cursor1].getKey() < pairs2[cursor2].getKey())
            resultPairs.push_back(pairs1[cursor1++]);
        else 
            resultPairs.push_back(pairs2[cursor2++]);
    }

    return resultPairs;
}

vector<KVPair> sortKVPairs(vector<SSTable> sstableList)
{
    vector<KVPair> resultPairs = sstableList[0].getPairs();
    int resultTime = sstableList[0].getTime();
    for (int i = 1; i < NUM; i++) // 初始化
    {
        resultPairs = sort2Pairs(resultPairs, sstableList[i].getPairs(), resultTime, sstableList[i].getTime());
        resultTime = sstableList[i].getTime();
    }

    return resultPairs;
}

vector<KVPair> cleanKVPairs(vector<KVPair> sortedKVPairs)
{
    vector<KVPair> cleanedKVPairs;
    for (int i = 0 ; i < sortedKVPairs.size(); i++)
    {
        if (i == sortedKVPairs.size() - 1 && sortedKVPairs[i].getValue().length() != 0)
            cleanedKVPairs.push_back(sortedKVPairs[i]);
        else if (sortedKVPairs[i].getKey() != sortedKVPairs[i + 1].getKey() && sortedKVPairs[i].getValue().length() != 0)
            cleanedKVPairs.push_back(sortedKVPairs[i]);
    }

    return cleanedKVPairs;
}

void writeToSSTable(string filename, int file_size, int nkeys, vector<KVPair> aimKVPairs)
{
    int time = 0x00FFFFFF;
    int offset = (nkeys * 2 + 3) * sizeof(int);
    ofstream outfile(filename, ios::out | ios::binary);
    outfile.write((char*)&file_size, sizeof(int));
    outfile.write((char*)&time, sizeof(int));
    outfile.write((char*)&nkeys, sizeof(int));
    for (vector<KVPair>::iterator iter = aimKVPairs.begin(); iter != aimKVPairs.end(); iter++)
    {
        int key = iter->getKey();
        outfile.write((char*)&key, sizeof(int));
        outfile.write((char*)&offset, sizeof(int));
        offset += iter->getValue().length();
    }
    for (vector<KVPair>::iterator iter = aimKVPairs.begin(); iter != aimKVPairs.end(); iter++)
    {
        string value = iter->getValue();
        outfile.write((char*)&value[0], value.length());
    }

    outfile.close();
}

void saveSSTables(vector<KVPair> leftKVPairs)
{
    int currFileIndex = 0;

begin:
    int file_size = 3 * 4; // file_size + time + nkeys
    int nkeys = 0;
    int i = 0;
    while (i < leftKVPairs.size())
    {
        file_size = file_size + leftKVPairs[i].getValue().length() + 2 * sizeof(int);
        nkeys++;
        if (i == leftKVPairs.size() - 1 || file_size + leftKVPairs[i + 1].getValue().length() + 2 * sizeof(int) >= LARGEST_SIZE)
        {
            vector<KVPair> tempKVPairs;
            tempKVPairs.assign(leftKVPairs.begin(), leftKVPairs.begin() + nkeys);
            leftKVPairs.assign(leftKVPairs.begin() + nkeys, leftKVPairs.end());

            stringstream ss;
            ss << "./small-case/myoutput-" << currFileIndex++ << ".sst";
            writeToSSTable(ss.str(), file_size, nkeys, tempKVPairs);
            goto begin;
        }
        i++;
    }
}

int main ()
{
    vector<SSTable> sstableList;

    for (int i = 1; i <= NUM; i++)
    {
        stringstream ss;
        ss << "./small-case/sstable-" << i << ".sst";
        SSTable tempTable = loadSSTables(ss.str());
        sstableList.push_back(tempTable);
    }

    vector<KVPair> sortedKVPairs = sortKVPairs(sstableList);
    vector<KVPair> cleanedKVPairs = cleanKVPairs(sortedKVPairs);

    saveSSTables(cleanedKVPairs);

    return 0;
}