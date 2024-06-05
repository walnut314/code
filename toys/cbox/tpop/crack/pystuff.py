#!/usr/bin/python3


def findSmallest(arr):
    smallest = arr[0]
    smallest_index = 0
    for i in range(1, len(arr)):
        if arr[i] < smallest:
            smallest = arr[i]
            smallest_index = i
    return smallest_index

def selsort(arr):
    newArr = []
    for i in range(len(arr)):
        smallest = findSmallest(arr)
        newArr.append(arr.pop(smallest))
    return newArr

def qsort(arr):
    if len(arr) < 2:
        return arr
    else:
        pivot = arr[0]
        less    = [v for v in arr[1:] if v <= pivot]
        greater = [v for v in arr[1:] if v > pivot]
        return qsort(less) + [pivot] + qsort(greater)

def sum(arr):
    if len(arr) == 0:
        return 0
    val = arr.pop(0)
    return val + sum(arr)

def test_rsum():
    list = [2,4,6]
    print("sum: " + str(sum(list)))

def test_selsort():
    list = [7,1,3,9,4]
    print(list)
    newarr = selsort(list)
    print(newarr)

def main():
#    test_rsum()
    list = [7,1,3,9,4]
    print("qsort: " + str(qsort(list)))


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print("Exception occured running main():")
        print(str(e))
