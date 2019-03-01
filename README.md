# Toys

## Kernighan-Lin

Multi-way partition by Lin-Kernighan Algorithm considering unequal sized sets

考虑各种不等于2^s情况的多式划分，核心部分是二叉树进行多次均等二式LK划分

**Build and Run**

```
g++ kl.cpp -o kl.out

./kl.out graph.txt
```

**Input graph.txt**

11: number of sum tasks

3: number of sets you want by partition

4: max number of each set

matrix: communication cost

## ASAP-ALAP-ListScheduling

**1.文件说明**

- .vscode：vscode运行的配置文件
- list scheduling.exe：调试生成的可执行程序，可直接运行
- main.cpp：C++源程序文件
- test.txt：数据样本

**2.代码说明**

1. 文本文件的处理函数(preHandle())细节过于繁琐复杂，可以跳过此部分
2. List Scheduling使用ALAP和ASAP的结果作为优先级判定标准

**3.运行结果**

使用test.txt的运行结果如下所示

![image](https://github.com/hunterlxt/ASAP-ALAP-ListScheduling/raw/master/screenshots/result.PNG)