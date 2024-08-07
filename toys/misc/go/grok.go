package main

import (
    "fmt"
    "container/list"
    //"container/stack"
    //"github.com/golang-collections/collections/stack"
    //"reflect"
    //"strings"
)

func check_voter(m map[string]int, s string) string {
    if _, ok := m[s]; ok {
        return s + " already voted"
    } else {
        m[s] = 1
        return "let " + s + " vote now"
    }
}

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

func bfs(graph map[string][]string, start string) bool {
    search_queue := list.New()
    search_queue.PushBack(graph[start])
    searched := map[string]int{};
    fmt.Println(start)
    for ; search_queue.Len() > 0; {
        e := search_queue.Front()
        names := e.Value.([]string)
        for _,name := range names {
            search_queue.PushBack(graph[name])
            if _, ok := searched[name]; ok {
            } else {
                searched[name] = 1
                fmt.Println(name)
                search_queue.PushBack(graph[name])
            }
        }
        search_queue.Remove(e)
    }
    return false
}

func bfs2(graph map[string][]string, start string) bool {
    search_queue := list.New()
    search_queue.PushBack(start)
    searched := map[string]bool{};
    fmt.Println(start)
    searched[start] = true
    for ; search_queue.Len() > 0; {
        e := search_queue.Front()
        nm := e.Value.(string)
        names := graph[nm]
        for _,name := range names {
            if !searched[name] {
                searched[name] = true
                fmt.Println(name)
                search_queue.PushBack(name)
            }
        }
        search_queue.Remove(e)
    }
    return false
}

func dfs_r(tree map[string][]string, visited map[string]bool, start string) { 
    // Mark the current node as visited
    if !visited[start] {
        visited[start] = true
        fmt.Println(start)
    }
    names := tree[start]
    for _,name := range names {
        v := visited[name]
        if (!v) {
            fmt.Println(name)
            visited[name] = true
            dfs_r(tree, visited, name)
        }
    }
}

type stack []string
func (s stack) Push(v string) stack {
    return append(s, v)
}

func (s stack) Pop() (stack, string) {
    // FIXME: What do we do if the stack is empty, though?
    l := len(s)
    return  s[:l-1], s[l-1]
}

func dfs(tree map[string][]string, start string) { 
    s := make(stack, 0)
    s = s.Push(start)
    n := ""
    fmt.Println(len(s))
    searched := map[string]bool{};
    searched[start] = true
    fmt.Println(start)
    for len(s) > 0 {
        s,n = s.Pop()
        names := tree[n]
        for _,name := range names {
            v := searched[name]
            if (!v) {
                searched[name] = true
                s = s.Push(name)
                fmt.Println(name)
            }
        }
    }
}

type Tree struct {
    left  *Tree
    right *Tree
    val   int
}

//func (t Tree)Insert(root *Tree, val int) *Tree {
func (t *Tree) Insert(val int) *Tree {
    if t == nil {
        t = &Tree{nil, nil, val}
        return t
    } else if val < t.val {
        t.left = t.left.Insert(val)
        //root.left = TreeInsert(root.left, val)
    } else if val > t.val {
        t.right = t.right.Insert(val)
        //root.right = TreeInsert(root.right, val)
    }
    return t
}

func (t *Tree) Traverse() {
    if t == nil {
        return
    }
    t.left.Traverse()
    fmt.Printf("%d ", t.val)
    t.right.Traverse()
}

type Map struct {
    leaf map[string][]string
}

func (m *Map) Insert(root string, val string) {
    if m.leaf == nil {
        m.leaf = make(map[string][]string, 0)
    }
    m.leaf[root] = append(m.leaf[root], val)
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

    voted := map[string]int{}
    //m = map[string]float32 {
    //    "apple": 0.67,
    //    "milk": 1.49,
    //    "avocado": 1.89,
    //}
    name := "chip"
    fmt.Println(check_voter(voted, name))
    fmt.Println(check_voter(voted, name))

    fmt.Println("\nBFS...graph")
    graph := map[string][]string{}
    graph["you"]    = append(graph["you"], "alice", "bob", "claire")
    graph["bob"]    = append(graph["bob"], "anuj", "peggy")
    graph["alice"]  = append(graph["alice"], "peggy")
    graph["claire"] = append(graph["claire"], "thom", "jonny")
    graph["anuj"]   = append(graph["anuj"], "")
    graph["peggy"]  = append(graph["peggy"], "")
    graph["thom"]   = append(graph["thom"], "")
    graph["jonny"]  = append(graph["jonny"], "")
    bfs(graph, "you")
    fmt.Println("\nBFS2...graph")
    bfs2(graph, "you")
/*
    tree := map[string][]string{}
    tree["root"]        = append(tree["root"], "2001", "odyssey.png")
    tree["2001"]        = append(tree["2001"], "a.png", "space.png")
    tree["odyssey.png"] = append(tree["odyssey.png"], "")
    tree["a.png"]       = append(tree["a.png"], "")
    tree["space.png"]   = append(tree["space.png"], "")
    fmt.Println("\nbfs...tree")
    bfs(tree, "root")
    fmt.Println("\ndfs...tree")
    dfs(tree, "root")
    fmt.Println("\ndfs_r...tree")
    visited := map[string]bool{};
    dfs_r(tree, visited, "root")
*/
    fmt.Println("Tree...")
    tree2 := &Tree{}
    for i := range a {
        fmt.Println(i)
        tree2 = tree2.Insert(i)
    }
    tree2.Traverse()
    fmt.Println()

    fmt.Println("Map...")
    mapper := &Map{}

    mapper.Insert("root", "2001")
    mapper.Insert("root", "odyssey.png")
    mapper.Insert("2001", "a.png")
    mapper.Insert("2001", "space.png")
    mapper.Insert("odyssey.png", "")
    mapper.Insert("a.png", "")
    mapper.Insert("space.png", "")

    fmt.Println(mapper)

}
