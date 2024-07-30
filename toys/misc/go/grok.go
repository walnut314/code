package main

import (
    "fmt"
)

func sum(a []int) int {
    if len(a) == 0 {
        return 0
    } else {
        return a[0] + sum(a[1:])
    }
}

func count(a []int) int {
    if len(a) == 0 {
        return 0
    } else {
        return 1 + count(a[1:])
    }
}

func maxi(a []int) int {
    if len(a) == 1 {
        return a[0]
    }
    max := maxi(a[:len(a)-1])
    if a[len(a)-1] > max {
      return a[len(a)-1]
   }
   return max
}

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

func qsort(a []int) []int {
    if len(a) < 2 {
        return a
    } else {
        pivot   := a[0]
        less    := []int{}
        greater := []int{}
        for i := 1; i < len(a); i++ {
            if a[i] <= pivot {
                less = append(less, a[i])
            } else {
                greater = append(greater, a[i])
            }
        }
        cong := []int{}
        cong = append(cong, qsort(less)...)
        cong = append(cong, pivot)
        cong = append(cong, qsort(greater)...)
        return cong 
    }
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

    c := []int{4,1,66,8,2,5,30,-10}
    fmt.Printf("sum: %d\n", sum(c))
    fmt.Printf("count: %d\n", count(c))
    fmt.Printf("max: %d\n", maxi(c))
    arrshow(c)
    d := qsort(c)
    arrshow(d)
}
