#include "keyword.h"
#include <fstream>
#include <sstream>
using namespace std;

#define NUM 3

vector<User> userKeywordCounts;
vector<Record> rankedRecords;

// ------------------------------------------------------------------------------------------------------------------ part1
 
void userAddKeyword (vector<User> &userKeywordCounts, uint32_t ID, string keyword)
{
    bool userIRU = false, keywordIRU = false;
    for (vector<User>::iterator iter1 = userKeywordCounts.begin(); iter1 != userKeywordCounts.end(); iter1++)
    {
        if (iter1->getID() == ID)
        {
            userIRU = true;
            vector<Record> tempRecords = iter1->getRecords();
            for (vector<Record>::iterator iter2 = tempRecords.begin(); iter2 != tempRecords.end(); iter2++)
            {
                if (iter2->getKeyword() == keyword)
                {
                    keywordIRU = true;
                    iter2->addCount(1);
                    iter1->updateRecords(tempRecords); // 双层iter，第二层的修改映射不到第一层，必须手动update
                    break;
                }
            }
            if (keywordIRU == false)
            {
                Record newRecord(keyword, 1);
                iter1->pushRecord(newRecord);
            }
        }
    }

    if (userIRU == false)
    {
        Record newRecord(keyword, 1);
        vector<Record> tempList;
        tempList.push_back(newRecord);
        User newUser(ID, tempList);
        userKeywordCounts.push_back(newUser);
    }
}

void loadKeywordCounts (string filename, vector<User> &userKeywordCounts)
{
    ifstream infile(filename, ios::in | ios::binary); 
    string buff;
    string keyword;
    while (getline(infile, buff))
    {
        uint32_t ID = stoi(buff.substr(0, buff.find(" ") - 0));
        string keyword = buff.substr(buff.find(" ") + 1, buff.length() - buff.find(" ") - 1);

        userAddKeyword(userKeywordCounts, ID, keyword);
    }

    infile.close();
}

// ------------------------------------------------------------------------------------------------------------------ part2

void swap(Record arr[], int i, int j)
{
    Record temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(Record arr[], int n)
{
    for(int i = n - 1; i > 0; i--)
        for(int j = 0; j < i; j++)
            if(arr[j].getCount() < arr[j+1].getCount()) // 大的放在前面 
                swap(arr[j], arr[j+1]);
            else if (arr[j].getCount() == arr[j+1].getCount() && arr[j].getKeyword() < arr[j+1].getKeyword())
                swap(arr[j], arr[j+1]);
}

void rankingAddKeyword(vector<Record> &tempRecords, string keyword, int count)
{
    for (vector<Record>::iterator iter = tempRecords.begin(); iter != tempRecords.end(); iter++)
    {
        if (iter->getKeyword() == keyword)
        {
            iter->addCount(count);
            return;
        }
    }

    Record newRecord(keyword, count);
    tempRecords.push_back(newRecord);
}

string* rankKeywords(vector<User> userKeywordCounts)
{
    vector<Record> tempRecords;
    for (vector<User>::iterator iter1 = userKeywordCounts.begin(); iter1 != userKeywordCounts.end(); iter1++)
    {
        vector<Record> temptempRecords = iter1->getRecords();
        for (vector<Record>::iterator iter2 = temptempRecords.begin(); iter2 != temptempRecords.end(); iter2++)
            rankingAddKeyword(tempRecords, iter2->getKeyword(), iter2->getCount());
    }

    Record *arr = new Record[tempRecords.size()];
    for (int i = 0; i < tempRecords.size(); i++)
        arr[i] = tempRecords[i];

    bubbleSort(arr, tempRecords.size());
    for (int i = 0; i < tempRecords.size(); i++)
    {
        cout << arr[i].getKeyword() << " " << arr[i].getCount() << endl;
        rankedRecords.push_back(arr[i]);
    }

    string *resultArr = new string[tempRecords.size()];
    for (int i = 0; i < tempRecords.size(); i++)
        resultArr[i] = arr[i].getKeyword();

    return resultArr;
}

// ------------------------------------------------------------------------------------------------------------------ part3

int getRelationK (uint32_t id1, uint32_t id2, string k) // 算出两个用户间关于k的关联度
{
    int count1, count2, weightsk;
    bool kARU1 = false;
    bool kARU2 = false;
    for (vector<User>::iterator iter1 = userKeywordCounts.begin(); iter1 != userKeywordCounts.end(); iter1++)
    {
        if (iter1->getID() == id1)
        {
            vector<Record> tempRecords = iter1->getRecords();
            for (vector<Record>::iterator iter2 = tempRecords.begin(); iter2 != tempRecords.end(); iter2++)
            {
                if (iter2->getKeyword() == k)
                {
                    kARU1 = true;
                    count1 = iter2->getCount();
                    break;
                }
            }
            if (kARU1 == false)
            {
                kARU1 = true;
                count1 = 0;
            }
            if (kARU2 == true) // 如果id2已经出现过了
                break;
        }
        else if (iter1->getID() == id2)
        {
            vector<Record> tempRecords = iter1->getRecords();
            for (vector<Record>::iterator iter2 = tempRecords.begin(); iter2 != tempRecords.end(); iter2++)
            {
                if (iter2->getKeyword() == k)
                {
                    kARU2 = true;
                    count2 = iter2->getCount();
                    break;
                }
            }
            if (kARU2 == false)
            {
                kARU2 = true;
                count2 = 0;
            }
            if (kARU1 == true) // 如果id1已经出现过了
                break;   
        }
    }

    for (int i = 0; i < rankedRecords.size(); i++)
        if (rankedRecords[i].getKeyword() == k)
            weightsk = rankedRecords.size() - i; // 关键词总数减去k在ranking中的下标

    int result = (count1 < count2) ? count1*weightsk : count2*weightsk; // 小的乘以weight
    return result;
}

void calcRelations () 
{
    int userNum = userKeywordCounts.size();
    int** relations = new int *[userNum];
    for (int i = 0; i < userNum; i++)
    {
        relations[i] = new int [userNum];
        for (int j = 0; j < i; j++)
            relations[i][j] = 0;
    }
        

    for (int i = 0; i < userNum; i++)
        for (int j = 0; j < i; j++)
            for (int k = 0; k < rankedRecords.size(); k++)
                relations[i][j] += getRelationK(userKeywordCounts[i].getID(), userKeywordCounts[j].getID(), rankedRecords[k].getKeyword());

    for (int i = 0; i < userNum; i++)
    {
        cout << userKeywordCounts[i].getID() << ": ";
        for (int j = 0; j < i; j++)
            cout << relations[i][j] << " ";
        cout << endl;
    }
        
}

int main()
{
    for (int i = 1; i <= NUM; i++)
    {
        stringstream ss;
        ss << "./demo/keywords-" << i << ".log";
        loadKeywordCounts(ss.str(), userKeywordCounts);
    }
    cout << "part 1:" << endl;
    for (vector<User>::iterator iter1 = userKeywordCounts.begin(); iter1 != userKeywordCounts.end(); iter1++)
    {
        cout << iter1->getID() << " ";
        vector<Record> tempRecords = iter1->getRecords();
        for (vector<Record>::iterator iter2 = tempRecords.begin(); iter2 != tempRecords.end(); iter2++)
            cout << iter2->getKeyword() << " " << iter2->getCount() << " ";
        cout << endl; 
    }
    cout << endl;

    cout << "part 2:" << endl;
    string *ranking = rankKeywords(userKeywordCounts);
    cout << endl;

    cout << "part 3:" << endl;
    calcRelations();
    cout << endl;

    return 0;
}