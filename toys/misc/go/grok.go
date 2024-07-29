package main

import (
    "fmt"
)

func binsea(a []int, key int) int {
    lo := 0
    hi := len(a)-1
    
    for ; lo <= hi; {
        mid := (lo+hi)/2
        if a[mid] == key {
            return mid
        } else if a[mid] > key {
            hi = mid-1
        } else {
            lo = mid+1
        }
    }
    return -1;
}

func findSmallest(a []int) int {
    smallest := a[0]
    smallest_index := 0
    for i := 0; i < len(a); i++ {
        if a[i] < smallest {
            smallest = a[i]
            smallest_index = i
        }
    }
    return smallest_index
}

func selsort(a []int) []int {
    b := a
    var newarr []int
    for i := 0; i < len(a); i++ {
        smallest := findSmallest(b)
        newarr = append(newarr, b[smallest])
        b = append(b[:smallest], b[smallest+1:]...)
    }
    return newarr
}

func arrshow(a []int) {
    for i := 0; i < len(a); i++ {
        fmt.Printf("%d ", a[i])
    }
    fmt.Printf("\n")
}

func main() {
    a := []int{4,1,8,2,5}
    arrshow(a)
    b := selsort(a)
    arrshow(b)
    fmt.Printf("\n")
    t := 5
    fmt.Printf("sea(%d): %d\n", t, binsea(b, t))
}
