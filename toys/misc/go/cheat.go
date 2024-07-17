package main

import (
    "fmt"
)

func count_bits(a int) int {
    num_bits := 0
    for ; a > 0 ; {
        num_bits++
        a &= (a - 1)
    }
    return num_bits
}

type List struct {
    Next *List
    Item int
}

func ShowList(head *List) {
    temp := head.Next
    for ; temp != nil ; {
        fmt.Printf("item: %d\n", temp.Item)
        temp = temp.Next
    }
}

func ListReverse(head *List) *List {
    p := head
    q := head
    var r *List
    r = nil
    for ; p != nil ; {
        q = p.Next
        p.Next = r
        r = p
        p = q
    }
    return r
}

func AddList(head *List, item int) {
    temp := head.Next
    list := new(List)
    list.Item = item
    head.Next = list
    list.Next = temp
}

func DoList() {
    head := new(List) // returns a pointer to List
    for i := 0; i < 8; i++ {
        AddList(head, i)
    }
    ShowList(head)
    fmt.Printf("\n")
    r := ListReverse(head)
    ShowList(r)
}

func main() {
    a := [...]int{1,2,3,4}
    for i := 0; i < len(a); i++ {
        fmt.Printf("%d ", a[i])
    }
    fmt.Printf("\n")

    x := 0x17
    fmt.Printf("bits: %d\n", count_bits(x))

    DoList()
}
