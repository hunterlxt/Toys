/*
*   ALAP-ASAP-List Scheduling
*   Created on:Oct 15,2017
*   Author:Liu Xintao
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <iomanip>
using namespace std;

int vertex;  // total number of vertex
struct gnode // define properties of nodes in graph
{
    vector<int> child;  // child nodes of this node
    vector<int> parent; // parent nodes of this node
    int soon = 1;       // control step in asap
    int late = 1;       // control step in asap
};
vector<gnode> node;                   // array of every node information
int *temp_step;                       // pointer of array in which temp step of each node stored
int max_length = 1;                   // the critical path length in ALAP
int preHandle();                      // handle data from input file
void asap(vector<gnode> &);           // ASAP
void alap(vector<gnode> &);           // ALAP
void deep(vector<gnode> &, int);      // search child node as deep as possible
void up(vector<gnode> &, int);        // search parent node as high as possible
void listScheduling(vector<gnode> &); // schedule the operations using list scheduling

int main()
{
    if (preHandle() != 0)
    {
        cout << "Unable to preHandle\n\n";
        system("pause");
        return 0;
    }
    asap(node);
    alap(node);
    cout << "Mobility Window Result:\n"
         << "        ASAP | ALAP\n";
    for (int i = 0; i < vertex; i++)
    {
        cout << "node" << setw(3) << i << ":";
        cout << setw(4) << node[i].soon << " | " << setw(2) << node[i].late << endl;
    }
    listScheduling(node);
    system("pause");
    return 0;
}

int preHandle() // too many details in this function and this is not key point
{
    string line, temp_string, filename;
    gnode a;
    char temp_filename[20];
    cout << "Enter the name of the graph file: ";
    cin >> filename;
    strcpy_s(temp_filename, filename.c_str());
    ifstream myfile(temp_filename);

    if (myfile.is_open())
    {
        getline(myfile, line);
        stringstream ss(line);
        ss >> vertex;
        for (int i = 0; i < vertex; i++)
            node.push_back(a);
        while (getline(myfile, line))
        {
            ss.clear();
            ss.str(line);
            int stringlength = line.length();
            int temp_node, temp;
            ss >> temp_node;
            for (int idx = 1; idx < stringlength; idx++)
            {
                ss >> temp;
                if (idx > 1 && temp == node[temp_node].child[idx - 2])
                    break;
                node[temp_node].child.push_back(temp);
                node[temp].parent.push_back(temp_node);
            }
        }
    }
    else
    {

        cout << "Unable to open file\n";
        return -1;
    }
    return 0;
}

void asap(vector<gnode> &node)
{
    temp_step = new int[vertex]; // store current temporary step
    for (int i = 0; i < vertex; i++)
        temp_step[i] = 0;
    for (int idx = 0; idx < vertex; idx++)
    {
        if (node[idx].parent.empty())
        {
            node[idx].soon = 1;
            temp_step[idx] = 1;
            deep(node, idx);
        }
    }
    delete[] temp_step;
    return;
}

void alap(vector<gnode> &node)
{
    temp_step = new int[vertex]; // store current temporary step
    for (int i = 0; i < vertex; i++)
        temp_step[i] = 0;
    for (int idx = 0; idx < vertex; idx++)
    {
        if (node[idx].child.empty())
        {
            node[idx].late = 1;
            temp_step[idx] = 1;
            up(node, idx);
        }
    }
    for (int i = 0; i < vertex; i++)
        node[i].late = max_length + 1 - node[i].late;
    delete[] temp_step;
    return;
}

void deep(vector<gnode> &node, int idx)
{
    for (int i = 0; i < node[idx].child.size(); i++) // go through all child nodes of current node
    {
        int child_num = node[idx].child[i];
        temp_step[child_num] = temp_step[idx] + 1;
        node[child_num].soon = max(temp_step[child_num], node[child_num].soon);
        deep(node, child_num);
    }
    return;
}

void up(vector<gnode> &node, int idx)
{
    for (int i = 0; i < node[idx].parent.size(); i++) // go through all parent nodes of current node
    {
        int parent_num = node[idx].parent[i];
        temp_step[parent_num] = temp_step[idx] + 1;
        max_length = max(max_length, temp_step[parent_num]); // store the length of critical path
        node[parent_num].late = max(temp_step[parent_num], node[parent_num].late);
        up(node, parent_num);
    }
    return;
}

void listScheduling(vector<gnode> &node)
{
    cout << "Enter the number of restriction of the resource: ";
    int N;
    cin >> N;                                // initialize N by resource restrict
    vector<int> scheduled_list[N];           // N resource units
    vector<int> ScheduleNode(vertex, 0);     // nodes which is scheduled in previous control step
    vector<int> TempScheduleNode(vertex, 0); // nodes which is scheduled in current control step
    vector<int> thisCycleInvalid;            // mark the nodes whose precondition is not done
    int remainderNode = vertex;              // the number of remaind Nodes which is not scheduled
    struct                                   // tb denotes best node should be scheduled in current step
    {
        int soon;
        int late;
        int idx;
    } tb;
    while (remainderNode > 0)
    {
        for (int j = 0; j < N; j++)
        {
            int getvalid = 0;
            if (getvalid == -1)
                continue;
            while (getvalid == 0)
            {
                tb.soon = vertex + 1;
                tb.late = vertex + 1;
                for (int i = 0; i < vertex; i++)
                {
                    bool flag = true;
                    for (auto e : thisCycleInvalid)
                    {
                        if (e == i)
                            flag = false;
                    }
                    if (!flag || TempScheduleNode[i] == 1)
                        continue;
                    if (node[i].late < tb.late)
                    {
                        tb.idx = i;
                        tb.soon = node[i].soon;
                        tb.late = node[i].late;
                    }
                    else if (node[i].late == tb.late && node[i].soon < tb.soon)
                    {
                        tb.idx = i;
                        tb.soon = node[i].soon;
                        tb.late = node[i].late;
                    }
                }

                if (tb.soon == vertex + 1)
                {
                    getvalid = -1;
                    scheduled_list[j].push_back(-1);
                    break;
                }
                bool flag = true;
                for (int i = 0; i < node[tb.idx].parent.size(); i++)
                {
                    if (ScheduleNode[node[tb.idx].parent[i]] == 0)
                        flag = false;
                }
                if (flag)
                {
                    TempScheduleNode[tb.idx] = 1;
                    remainderNode--;
                    scheduled_list[j].push_back(tb.idx);
                    getvalid = 1;
                }
                else
                {
                    thisCycleInvalid.push_back(tb.idx);
                }
            }
        }
        thisCycleInvalid.clear();
        ScheduleNode = TempScheduleNode;
    }

    for (int j = 0; j < scheduled_list[0].size(); j++)
    {
        cout << "control step";
        cout << setw(3) << j + 1 << ":";
        for (int d = 0; d < N; d++)
        {
            if (scheduled_list[d][j] != -1)
                cout << ' ' << setw(3) << scheduled_list[d][j];
        }
        cout << endl;
    }
    return;
}