package main

import (
    "fmt"
)

type Record struct {
    Id int;
    Name string;
}

func main() {
    var recs[]Record
    hash := make(map[string]Record)

    for i := 0; i < 4; i++ {
        id := i+42
        name := "the dude "+string('0'+i)
        recs = append(recs, Record{Id: id, Name: name})
        hash[name] = Record{Id: id, Name: name}
    }

    fmt.Println("array");
    for _,r := range recs {
        fmt.Printf("%d => %s\n", r.Id, r.Name)
    }

    fmt.Println("hash");
    for key,val := range hash {
        fmt.Printf("[%s]: %d => %s\n", key, val.Id, val.Name)
    }
}

