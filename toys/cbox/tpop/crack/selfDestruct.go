package main

import (
        "fmt"
        "time"
       )



func cancel(abort chan bool) {
    var r int
    fmt.Scanf(" %c", &r)
    switch r {
        default: {
            fmt.Printf("err\n")
            //abort <- false
        }
        case 'y': abort <- true
        case 'Y': abort <- true
    }
}

func countDown(count chan int) {
    for i := 10; i >= 0; i-- {
        count <- i
        time.Sleep(1000000000)
    }
}

func main() {
    abort := make(chan bool)
    count := make(chan int)
    go cancel(abort)
    go countDown(count)
    for {
        select {
            case a := <- abort:
                if a {
                    fmt.Printf("self destruct aborted\n");
                } else {
                    fmt.Printf("destroy 2!\n");

                }
            case i := <- count:
                if 0 == i {
                    fmt.Printf("destroy 1!\n");
                    return
                }
                fmt.Printf("%d seconds remaining\n", i);

         }
    }
}


