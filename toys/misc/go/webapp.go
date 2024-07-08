package main

import (
    "fmt"
    "net/http"
    "strings"
    "log"
)

func sayhelloName(w http.ResponseWriter, r *http.Request) {
    r.ParseForm()  // parse arguments, you have to call this by yourself
    fmt.Println(r.Form)  // print form information in server side
    fmt.Println("path", r.URL.Path)
    fmt.Println("scheme", r.URL.Scheme)
    fmt.Println(r.Form["url_long"])
    for k, v := range r.Form {
        fmt.Println("key:", k)
        fmt.Println("val:", strings.Join(v, ""))
    }
    fmt.Fprintf(w, "Hello asstaxi!") // send data to client side

    //sum := 0
    //x := 0
    //for x < 10 {
    //    fmt.Fprintf(w, "sum:", sum)
    //}

}

func main() {
    http.HandleFunc("/", sayhelloName) // set router
    err := http.ListenAndServe(":5000", nil) // set listen port
    if err != nil {
        log.Fatal("ListenAndServe: ", err)
    }
}

