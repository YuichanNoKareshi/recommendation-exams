#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include "rainyroad.h"
using namespace std;

vector<leftWater> leftWaters; // 用来记录每个地点的高度和积水量，每小时更新一次
map<int, vector<rainWater>> rainWatersAtTime; // 用来记录每小时，每个地点的降雨量（每小时可能有多个地点同时降雨）
int **graph;
int NUM; // 地点数

// --------------------------------------------------------------------------------- part1

void printLogo()
{
    ifstream infile("./data/logo", ios::in);
    string buff;
    while (getline(infile, buff))
        cout << buff << endl;    
    infile.close();
}

// --------------------------------------------------------------------------------- part2

void swap(leftWater arr[], int i, int j)
{
    leftWater temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void bubbleSort(leftWater arr[], int n)
{
    for(int i = n - 1; i > 0; i--)
        for(int j = 0; j < i; j++)
            if(arr[j].getHeight() < arr[j+1].getHeight()) // 大的放在前面 
                swap(arr[j], arr[j+1]);
            else if (arr[j].getHeight() == arr[j+1].getHeight() && arr[j].getLocation() > arr[j+1].getLocation())
                swap(arr[j], arr[j+1]);
}

void loadFile (string filename)
{
    ifstream infile(filename, ios::in);
    string buff;
    int N, M; // N个地点, M条边

    getline(infile, buff); // 第一行
    N = stoi(buff.substr(0, buff.find(" ") - 0));
    NUM = N;
    M = stoi(buff.substr(buff.find(" ") + 1, buff.length() - buff.find(" ") - 1));

    getline(infile, buff); // 第二行
    int lastSpaceIndex = -1; // 记录上一个空格的位置
    int currSpaceIndex; // 记录现在空格的位置
    for (int i = 0; i < N; i++)
    {
        int height;
        // if (i == 0)
        // {
        //     currSpaceIndex = buff.find(" ");
        //     height = stoi(buff.substr(lastSpaceIndex + 1, currSpaceIndex - lastSpaceIndex - 1));
        // }
        // else 
        if (i == N - 1)
            height = stoi(buff.substr(lastSpaceIndex + 1, buff.length() - lastSpaceIndex - 1));
        else 
        {
            currSpaceIndex = buff.find(" ", lastSpaceIndex + 1); // 从上一个空格位置的下一位开始找
            height = stoi(buff.substr(lastSpaceIndex + 1, currSpaceIndex - lastSpaceIndex - 1));
            lastSpaceIndex = currSpaceIndex; // 更新上一个空格的位置
        }

        leftWater temp(i, height, 0);
        leftWaters.push_back(temp);
    }
    // 下面把leftWaters根据地势从大到小, location从小到大排序
    leftWater *arr = new leftWater[N];
    for (int i = 0; i < N; i++)
        arr[i] = leftWaters[i];
    bubbleSort(arr, N);
    leftWaters.clear();
    for (int i = 0; i < N; i++)
        leftWaters.push_back(arr[i]);

    graph = new int*[N]; // 继续读文件，看哪两个点之间有边
    for (int i = 0; i < N; i++)
    {
        graph[i] = new int [N];
        for (int j = 0; j < N; j++)
            graph[i][j] = 0; // 初值全部设为0
    }
    
    int location1, location2;  
    while(getline(infile, buff))
    {
        if (buff.find(" ") == -1)
            break;
        location1 = stoi(buff.substr(0, buff.find(" ") - 0));
        location2 = stoi(buff.substr(buff.find(" ") + 1, buff.length() - buff.find(" ") - 1));
        
        graph[location1][location2] = 1;
        graph[location2][location1] = 1;
    }
    // for (int i = 0; i < N; i++)
    // {
    //     for (int j = 0; j < N; j++)
    //         cout << graph[i][j] << " ";
    //     cout << endl;
    // }

    int mostEdgeNum = 0; // 当前有最多边的点的边数
    int mostlocation = 0; // 当前有最多边的点是哪个
    for (int j = 0; j < N; j++) // 遍历每列
    {
        int currEdgeNum = 0;
        for (int i = 0; i < N; i++) // 算出该列的边数
        {
            if (graph[i][j] == 1) 
                currEdgeNum++; 
        }
        if (currEdgeNum > mostEdgeNum)
        {
            mostlocation = j;
            mostEdgeNum = currEdgeNum;
        }
    }
    cout << mostlocation << endl;

    infile.close();
}

// --------------------------------------------------------------------------------- part3

void constructRainWatersAtTime(string filename)
{
    ifstream infile(filename, ios::in);
    string buff;
    int RainNum;
    while(getline(infile, buff))
    {
        if (buff.find(" ") != -1) // 跳过构建graph的行
            continue;
        RainNum = stoi(buff); // 直到读到R
        break;
    }
    int T, L, W; // 降雨时间、地点、水量
    while(getline(infile, buff))
    {
        int lastSpaceIndex = -1;
        int currSpaceIndex = buff.find(" ");
        T = stoi(buff.substr(lastSpaceIndex + 1, currSpaceIndex - lastSpaceIndex - 1));
        lastSpaceIndex = currSpaceIndex;
        currSpaceIndex = buff.find(" ", lastSpaceIndex + 1);
        L = stoi(buff.substr(lastSpaceIndex + 1, currSpaceIndex - lastSpaceIndex - 1));
        lastSpaceIndex = currSpaceIndex;
        W = stoi(buff.substr(lastSpaceIndex + 1, buff.length() - lastSpaceIndex - 1));

        if (rainWatersAtTime.count(T) == 0) // 如果改时间已经有降雨了
        {
            rainWater tempRainWater(L, W);
            vector<rainWater> tempRainWaters;
            tempRainWaters.push_back(tempRainWater);
            rainWatersAtTime[T] = tempRainWaters;
        }
        else
        {
            rainWater tempRainWater(L, W);
            vector<rainWater> tempRainWaters = rainWatersAtTime[T];
            tempRainWaters.push_back(tempRainWater);
            rainWatersAtTime[T] = tempRainWaters;
        }
    }
    // for(auto iter = rainWatersAtTime.cbegin(); iter != rainWatersAtTime.cend(); iter++)
    // {
    //     vector<rainWater> tempRainWaters = iter->second;
    //     cout << iter->first << " ";
    //     for (int i = 0; i < tempRainWaters.size(); i++)
    //         cout << tempRainWaters[i].getLocation() << " " << tempRainWaters[i].getRain() << " ";
    //     cout << endl;
    // }
}

bool isDry(int t)
{
    for (int i = 0; i < leftWaters.size(); i++) // 遍历所有地点
        if (leftWaters[i].getWater() > 0)
            return false;
    for (auto iter = rainWatersAtTime.begin(); iter != rainWatersAtTime.end(); iter++)
        if (t <= iter->first) // 如果此后还有降雨也不行
            return false;
    return true;
}

void phase1()
{
    for (int i = 0; i < leftWaters.size(); i++) // 遍历所有地点
        if (leftWaters[i].getWater() > 0)
            leftWaters[i].subWater(1); // 排出1单位的水
}

vector<rainWater> phase2()
{
    vector<rainWater> phase3Rain; // 在phase3中有哪些地点会有水流入

    for (auto iter1 = leftWaters.begin(); iter1 != leftWaters.end(); iter1++) // 遍历所有地点
        if (iter1->getWater() > 0) // 如果该点有积水
        {
            for (auto iter2 = iter1 + 1; iter2 != leftWaters.end(); iter2++) // 检查这个点所有的后面的点(height都不会大于它)
            {
                if(iter1->getHeight() == iter2->getHeight())
                    continue;
                if(graph[iter1->getLocation()][iter2->getLocation()] == 1) // 如果两个点联通
                {
                    iter1->subWater(1);
                    bool isExist = false; // j地点不能直接加积水量，要等到phase3再加，先存着
                    for (auto iter = phase3Rain.begin(); iter != phase3Rain.end(); iter++)
                    {
                        if (iter->getLocation() == iter2->getLocation())
                        {
                            isExist = true;
                            iter->addRain(1);
                            break;
                        }
                    }
                    if (isExist == false)
                    {
                        rainWater temp(iter2->getLocation(), 1);
                        phase3Rain.push_back(temp);
                    }
                }
                if(iter1->getWater() == 0) // 如果向j流水之后i没水了，break
                    break;
            }
        }

    return phase3Rain;
}

void phase3(vector<rainWater> phase3Rain)
{
    for (auto iter1 = phase3Rain.begin(); iter1 != phase3Rain.end(); iter1++)
        for (auto iter2 = leftWaters.begin(); iter2 != leftWaters.end(); iter2++)
            if (iter2->getLocation() == iter1->getLocation())
            {
                iter2->addWater(iter1->getRain());
                break;
            }
}

void phase4(int t)
{
    if(rainWatersAtTime.count(t) == 1)
    {
        vector<rainWater> phase4Rain = rainWatersAtTime[t];
        for (auto iter1 = phase4Rain.begin(); iter1 != phase4Rain.end(); iter1++)
            for (auto iter2 = leftWaters.begin(); iter2 != leftWaters.end(); iter2++)
                if (iter2->getLocation() == iter1->getLocation())
                {
                    iter2->addWater(iter1->getRain());
                    break;
                }
    }   
}

void SimFlow(string filename) // 从t时刻到t+1时刻
{
    constructRainWatersAtTime(filename); // 先读文件，把准备工作都做好，下面开始模拟
    int t = 0; // 时间从0开始
    while (true)
    {
        // 阶段1
        phase1();
        vector<rainWater> phase3Rain = phase2();
        phase3(phase3Rain);
        phase4(++t);

        if (isDry(t)) 
            break;
    } 

    cout << t <<endl;
}

// --------------------------------------------------------------------------------- part3

int BFS()
{
    int *step = new int[NUM];
    for (int i = 1; i < NUM; i++)
        step[i] = -1;
    step[0] = 0;
    int currStep = 0;
    while(step[NUM-1] == -1) // 一直循环到NUM-1节点可以到达
    {
        for (int i = 0; i < NUM; i++)
            if(step[i] == currStep) // 对于走了当前步数的节点，之前的节点就不看了
                for (int j = 0; j < NUM; j++) // 找他的邻居
                {
                    if (graph[i][j] == 1 && step[j] == -1) // i、j相通且j还没人走到
                        step[j] = currStep + 1;
                }
        currStep++;
    }

    return step[NUM - 1];

}

void CalcShortestPath()
{
    cout << BFS() << " ";
}

int main()
{
    printLogo();
    string filename;
    cout << "pls input filename:";
    cin >> filename;
    // filename = "./data/small.in";
    loadFile(filename);
    SimFlow(filename);
    CalcShortestPath();
    return 0;
}