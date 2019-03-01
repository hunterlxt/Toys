/*
 * Multi-Way Partition by KL
 *
 * Author: Liu Xintao
 */

#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

int taskNum;
int setNum;
int maxNum;
int m, s;  //partition number
bool flag_power = true;
vector<vector<int>> adjacencyMatrix;
set<set<int>> resultSets;
set<set<int>> outputSets;
struct node {
    set<int> set_data;
    struct node *lchild;
    struct node *rchild;
} FirstNode;

void read_file(char *name) {
    ifstream file;
    file.open(name);
    file >> taskNum;
    file >> setNum;
    file >> maxNum;
    for (int i = 0; i < taskNum; ++i) {
        vector<int> row;
        for (int j = 0; j < taskNum; ++j) {
            int temp;
            file >> temp;
            row.push_back(temp);
        }
        adjacencyMatrix.push_back(row);
    }
    set<int> temp_set;
    for (int k = 0; k < taskNum; ++k) {
        temp_set.insert(k + 1);
    }
    FirstNode.set_data = temp_set;
    file.close();
}

int is2Power(int num) {
    int index = 1;
    while (1) {
        int low = pow(2, index);
        int high = pow(2, index + 1);
        if (num == low) {
            flag_power = true;
            return index;
        }
        if (low < num && num < high) {
            flag_power = false;
            return index + 1;
        }
        index++;
    }
}

void compute_D(set<int> &A, set<int> &B, int I[], int E[], int D[]) {

    for (int l = 0; l < (A.size() + B.size()) + 1; ++l) {
        I[l] = 0;
        E[l] = 0;
        D[l] = 0;
    }

    for (auto a:A) {
        for (auto aa:A) {
            if (a != aa) {
                I[a] += adjacencyMatrix[a - 1][aa - 1];
            }
        }
    }
    for (auto b:B) {
        for (auto bb:B) {
            if (b != bb) {
                I[b] += adjacencyMatrix[b - 1][bb - 1];
            }
        }
    }
    for (auto a:A) {
        for (auto bb:B) {
            E[a] += adjacencyMatrix[a - 1][bb - 1];
        }
    }
    for (auto b:B) {
        for (auto aa:A) {
            E[b] += adjacencyMatrix[b - 1][aa - 1];
        }
    }
    for (int a:A) {
        D[a] = E[a] - I[a];
    }
    for (int b:B) {
        D[b] = E[b] - I[b];
    }
}

void two_way_partition(node *T) {
    int eachNum = T->set_data.size() / 2;
    int g[FirstNode.set_data.size() + 1][T->set_data.size() + 1] = {0};
    int I[FirstNode.set_data.size() + 1] = {0};
    int E[FirstNode.set_data.size() + 1] = {0};
    int D[FirstNode.set_data.size() + 1] = {0};
    int g_max = -999;
    int a_max;
    int b_max;
    int G = 1;
    int sumg;
    int maxSumIndex;
    int gk[eachNum + 1][3]; //0:max g; 1:max a; 2:max b
    set<int> A, B;
    set<int>::iterator it;
    int temp_num = eachNum;

    /*initialize the A and B (uniform)*/
    for (it = T->set_data.begin(); it != T->set_data.end(); it++, temp_num--) {
        A.insert(*it);
        if (temp_num == 1)
            break;
    }
    for (it++; it != T->set_data.end(); it++) {
        B.insert(*it);
    }

    /*repeat iteration*/
    while (G > 0) {

        /*swap TotalSize/2 times*/
        set<int> unlockA = A;
        set<int> unlockB = B;
        set<int> lockA, lockB;
        for (int i = 0; i < eachNum; ++i) {

            /*clear and process data structure*/
            compute_D(unlockA, unlockB, I, E, D);
            g_max = -9999;
            for (int a:unlockA) {
                for (int b:unlockB) {
                    g[a][b] = D[a] + D[b] - 2 * adjacencyMatrix[a - 1][b - 1];
                    if (g_max < g[a][b]) {
                        g_max = g[a][b];
                        a_max = a;
                        b_max = b;
                    }
                }
            }
            gk[i + 1][0] = g_max;
            gk[i + 1][1] = a_max;
            gk[i + 1][2] = b_max;
            lockB.insert(a_max);
            lockA.insert(b_max);
            unlockA.erase(a_max);
            unlockB.erase(b_max);
        }
        sumg = 0;
        maxSumIndex = 1;
        G = gk[1][0];
        for (int i = 1; i < eachNum + 1; ++i) {
            sumg += gk[i][0];
            if (sumg > G) {
                G = sumg;
                maxSumIndex = i;
            }
        }
        if (G > 0) {
            for (int i = 1; i <= maxSumIndex; ++i) {
                A.insert(gk[i][2]);
                B.insert(gk[i][1]);
                A.erase(gk[i][1]);
                B.erase(gk[i][2]);
            }
        }

    }
    node *rc = (node *) malloc(sizeof(node));
    rc->set_data = A;
    T->rchild = rc;
    node *lc = (node *) malloc(sizeof(node));
    lc->set_data = B;
    T->lchild = lc;
}

void build_tree(node *T, int level, int maxLevel) {
    if (level > maxLevel) {
        return;
    }

    /*partition and store new divided set into child nodes*/
    two_way_partition(T);
    build_tree(T->rchild, level + 1, maxLevel);
    build_tree(T->lchild, level + 1, maxLevel);
}

void extractSet(node *T, int maxLevel, int L) {
    if (L == maxLevel + 1) {
        resultSets.insert(T->set_data);
        return;
    }
    extractSet(T->lchild, maxLevel, L + 1);
    extractSet(T->rchild, maxLevel, L + 1);
}

void clear_Zero_in_resultSets() {
    int flag = resultSets.size();
    while (flag--) {
        for (auto i:resultSets) {
            if (i.size() <= 0) {
                resultSets.erase(i);
            }
        }
    }
}

void take_off_max_sets() {
    int flag = resultSets.size();
    while (flag--) {
        for (auto e:resultSets) {
            if (e.size() == maxNum) {
                resultSets.erase(e);
                outputSets.insert(e);
                break;
            }
        }
    }
}

int num_of_need_cluser() {
    return (setNum - outputSets.size());
}

void cluster() {

    /*take off zero elements*/
    set<int> maxNumSet;
    set<int> needDelete;
    set<set<int>>::iterator it;
    set<int>::iterator ix;
    int F = resultSets.size() * maxNum;
    bool breakFlag;
    while (F--) {
        breakFlag = false;
        for (it = resultSets.begin(); it != resultSets.end(); it++) {
            for (ix = (*it).begin(); ix != (*it).end(); ix++) {
                if ((*ix) > taskNum) {
                    set<int> temp = (*it);
                    temp.erase(*ix);
                    resultSets.erase(*it);
                    resultSets.insert(temp);
                    breakFlag = true;
                    break;
                }
            }
            if (breakFlag)
                break;
        }
    }

    take_off_max_sets();

    clear_Zero_in_resultSets();

    /*cluster the remained sets*/
    while ((num_of_need_cluser() - 1) > 0) {

        int maxNUM = 0;
        for (auto i:resultSets) {
            if (i.size() > maxNUM) {
                maxNUM = i.size();
                maxNumSet = i;
            }
        }

        int max = -10000;
        int max_num;
        int loop_out_flag = 1;
        for (auto i:resultSets) {
            if (i.begin() != maxNumSet.begin()) {
                for (int j:i) {
                    for (int x:maxNumSet) {
                        if (adjacencyMatrix[j - 1][x - 1] > max) {
                            max_num = j;
                            needDelete = i;
                        }
                    }
                }
            }
        }
        maxNumSet.insert(max_num);
        needDelete.erase(max_num);

        take_off_max_sets();
        clear_Zero_in_resultSets();
    }

    outputSets.insert(*resultSets.begin());
}

void multi_way_partition() {

    if (taskNum != setNum * maxNum) {
        int gap = setNum * maxNum - taskNum;
        for (int i = 0; i < gap; ++i) {
            FirstNode.set_data.insert(taskNum + i + 1);
        }
    }

    /*using two solution to handle the problem*/
    int num = is2Power(FirstNode.set_data.size());
    if (flag_power) {

        s = num;
        int temp_level = pow(2, s) / maxNum;
        int maxLevel = is2Power(temp_level);

        /*partition using binary tree and store set in leaf node*/
        build_tree(&FirstNode, 1, maxLevel);

        /*extract sets out from leaf node*/
        extractSet(&FirstNode, maxLevel, 1);

        cluster();
    } else {
        m = num;
        int gap = pow(2, m) - FirstNode.set_data.size();
        int size = FirstNode.set_data.size();
        for (int i = 0; i < gap; ++i) {
            FirstNode.set_data.insert(size + i + 1);
        }

        /*add some zero elements into adjacencyMatrix*/
        for (int j = 0; j < (FirstNode.set_data.size() - taskNum); ++j) {
            vector<int> temp;
            for (int i = 0; i < FirstNode.set_data.size(); ++i) {
                temp.push_back(0);
            }
            adjacencyMatrix.push_back(temp);
        }
        for (int l = 0; l < taskNum; ++l) {
            for (int k = 0; k < (FirstNode.set_data.size() - taskNum); ++k) {
                adjacencyMatrix[l].push_back(0);
            }
        }

        int temp_level = pow(2, m) / maxNum;
        int maxLevel = is2Power(temp_level);

        /*partition using binary tree and store set in leaf node*/
        build_tree(&FirstNode, 1, maxLevel);

        /*extract sets out from leaf node*/
        extractSet(&FirstNode, maxLevel, 1);

        cluster();
    }

}

int sumcost() {
    int cost = 0;
    bool flag;
    for (auto i:outputSets) {
        for (auto j:outputSets) {
            flag = true;
            for (int x:i)
                for (int y:j)
                    if (x == y)
                        flag = false;
            if (flag) {
                for (int k:i) {
                    for (int h:j) {
                        cost += adjacencyMatrix[k - 1][h - 1];
                    }
                }
            }
        }
    }
    cost = cost / 2;
    return cost;
}

void output() {
    for (auto i:outputSets) {
        cout << "[ ";
        for (auto j:i) {
            cout << j << ' ';
        }
        cout << "]" << endl;
    }
    int cost = sumcost();
    cout << "\nTotal Cost: " << cost << endl;
}

int main(int argc, char *argv[]) {
    read_file(argv[1]);
    multi_way_partition();
    output();
    std::cout << "\nCompleted!" << std::endl;
    return 0;
}
