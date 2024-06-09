


def binsea(list, item):
    low = 0
    high = len(list) - 1

    while low <= high:
        mid = (low + high) // 2
        guess = list[mid]
        if guess == item:
            return mid
        if guess > item:
            high = mid - 1
        else:
            low = mid + 1
    return None
    

def main():
    find = 3
    list = [1,2,3,4]
    print(list)
    i = binsea(list, find)
    if i != None:
        print("found: " + str(list[i]))
    else:
        print(str(find) + " was not found")

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print("Exception occured running main():")
        print(str(e))
