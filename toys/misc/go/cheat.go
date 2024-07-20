package main

import (
    "fmt"
)

type Heap struct {
    a [256]int
    size int
}

func HeapUpMax(heap *Heap, k int) {
    v := heap.a[k]
    for ; heap.a[k/2] <= v; {
        heap.a[k] = heap.a[k/2]
        k /= 2
    }
    heap.a[k] = v
}
    
func HeapDownMax(heap *Heap, k int) {
    last := heap.a[k]
    for ; k <= heap.size/2; {
        child := 2 * k
        if child < heap.size && heap.a[child] < heap.a[child+1] {
            child++
        }
        if last >= heap.a[child] {
            break;
        }
        heap.a[k] = heap.a[child]
        k = child
    }
    heap.a[k] = last
}

func HeapShow(heap Heap) {
    fmt.Printf("heap show: size %d\n", heap.size)
    for i := 1; i < heap.size; i++ {
        fmt.Printf("%d ", heap.a[i])
    }
    fmt.Printf("\n")
}

func HeapTest() {
    heap := Heap{}
    heap.a[0] = 999
    heap.size = 0
    a := []int{ 34, 5, 23, 12, 33, 98, 4, 13, 44, 37, 1, 86, 8};
    for i := 0; i < len(a); i++ {
        heap.size++
        k := heap.size
        heap.a[k] = a[i]
        HeapUpMax(&heap, k)
    }
    HeapShow(heap)
    for ; heap.size != 0; {
        mx := heap.a[1]
        fmt.Printf("max: %d\n", mx)
        heap.a[1] = heap.a[heap.size]
        heap.size--
        HeapDownMax(&heap, 1)
    }
    HeapShow(heap)
}

func count_bits(a int) int {
    num_bits := 0
    for ; a > 0 ; {
        num_bits++
        a &= (a - 1)
    }
    return num_bits
}

func InsSort(a []int) {
    fmt.Printf("len: %d\n", len(a))
    for c := 1; c <= len(a)-1; c++ {
        d := c
        for ; d > 0 && a[d] < a[d-1]; {
            t := a[d]
            a[d] = a[d-1]
            a[d-1] = t
            d--
        }
    }
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

func ArrayShow(a []int) {
    for i := 0; i < len(a); i++ {
        fmt.Printf("%d ", a[i]);
    }
    fmt.Printf("\n")
}

func main() {
    a := []int{2,4,1,3}
    for i := 0; i < len(a); i++ {
        fmt.Printf("%d ", a[i])
    }
    fmt.Printf("\n")

    x := 0x17
    fmt.Printf("bits: %d\n", count_bits(x))

    DoList()

    ArrayShow(a)
    InsSort(a)
    ArrayShow(a)

    HeapTest()
}
