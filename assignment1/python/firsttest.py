#!/bin/python3

import math
import os
import random
import re
import sys



#
# Complete the 'subsetA' function below.
#
# The function is expected to return an INTEGER_ARRAY.
# The function accepts INTEGER_ARRAY arr as parameter.
#

def removeIntersections(arr):
    return list(dict.fromkeys(arr))

def findSum(array):
    sum = 0
    for i in array:
        sum = sum + i
    return sum
    
def subsetA(arr):
    # Write your code here
    tempArr=removeIntersections(arr)

    answer=[0]*2
    for i in range(0,len(tempArr),1):
        for j in range(0,len(tempArr)-1,1):
            k = tempArr[i]
            l = tempArr[j]
            del tempArr[i]
            del tempArr[j]
            print("TEST 1 : i ", i," j ",j)
            print("FIND SUM" ,findSum(tempArr) ," k ",k, " l ", l)
            print(tempArr)

            if findSum(tempArr) <= k + l and i != j and  k  not in tempArr and l not in tempArr:
                answer[0]= min(k,l)
                answer[1]=max(k,l)
                # print("i ", i," j ",j)
                # print(tempArr)
                # print("k ",k, " l ", l)
            tempArr=removeIntersections(arr)
    return answer

if __name__ == '__main__':
    fptr = open(os.environ['OUTPUT_PATH'], 'w')

    arr_count = int(input().strip())

    arr = []

    for _ in range(arr_count):
        arr_item = int(input().strip())
        arr.append(arr_item)

    result = subsetA(arr)

    fptr.write('\n'.join(map(str, result)))
    fptr.write('\n')

    fptr.close()
