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




func main() {
    a := [...]int{1,2,3,4}
    for i := 0; i < len(a); i++ {
        fmt.Printf("%d ", a[i])
    }
    fmt.Printf("\n")

    x := 0x17
    fmt.Printf("bits: %d\n", count_bits(x))
}
