package main

import (
        "os"
        "fmt"
        "math"
       )

const (
    Red                 = (1 << iota)
    Green               = (1 << iota)
    Blue, ColorMask     = (1 << iota), (1 << (iota+1))-1
)

type Example struct {
    Val string
    count int
}

func Log(e *Example)(int) {
    e.count++
    fmt.Printf("Ex: %d %s\n", e.count, e.Val)
    return 49
}
type integer int

func log(i integer) {
    fmt.Printf("%d\n", i);
}


// interface stuff
type cartesianPoint struct {
    x, y float64
}
type polarPoint struct {
    r, theta float64
}
func (p cartesianPoint) X() float64 {return p.x}
func (p cartesianPoint) Y() float64 {return p.y}
func (p polarPoint) X() float64 {return p.r * math.Cos(p.theta)}
func (p polarPoint) Y() float64 {return p.r * math.Sin(p.theta)}
func (self cartesianPoint) Print() {fmt.Printf("(%f, %f)\n", self.x, self.y)}
func (self polarPoint) Print() {fmt.Printf("(%f, %f)\n", self.r, self.theta)}
type Point interface {
    Printer
    X() float64
    Y() float64
}
type Printer interface {
    Print()
}

func DoGeom() {
    fmt.Printf("DoGeom\n");
    cart := cartesianPoint{1,2}
    polar := polarPoint{3,4}

    var cp Printer
    var rp Printer

    cp = cart
    rp = polar

    var cpt Point
    var rpt Point

    cpt = cart
    rpt = polar

    cp.Print()
    rp.Print()

    fmt.Printf("cart:  %f, %f\n", cpt.X(), cpt.Y())
    fmt.Printf("polar: %f, %f\n", rpt.X(), rpt.Y())

    cpt.Print()
    rpt.Print()
}

type Logger struct {
    out *os.File
}

func (l Logger) Log(s string) {
    out := l.out
    if (out == nil) {
        out = os.Stderr
    }
    fmt.Fprintf(out, "%s [%d]: %s\n", os.Args[0], os.Getppid(), s)
}

func (l *Logger) SetOutput(out *os.File) {
    l.out = out
}

type stackEntry struct {
    next *stackEntry
    value interface{}
}
type stack struct {
    top *stackEntry
}
func (s *stack) Push(v interface{}) {
    var e stackEntry
    e.value = v
    e.next = s.top
    s.top = &e
}
func (s *stack) Pop() interface{} {
    if s.top == nil {
        return nil
    }
    v := s.top.value
    s.top = s.top.next
    return v
}

func stackTest() {
    var s stack
    for i := 0; i < 4; i++ {
        v := i
        s.Push(v)
    }
    for { 
        v := s.Pop()
        if v == nil {
            break;
        }
        fmt.Printf("pop: %d\n", v);
    }
}

func main() {
    var log Logger
    file,err := os.Create("dude.txt")
    if err != nil {
        fmt.Printf("err: %s\n", err)
    }
    log.SetOutput(file)
    log.Log("oh crap")


    loops := 1
    for loops > 0 {
        fmt.Printf("\nNumber of loops?\n")
        fmt.Scanf("%d", &loops)
        for i := 0; i < loops; i++ {
            fmt.Printf("%d ", i)
        }
    }
    for {
        fmt.Printf("break\n");
        break;
    }

    fmt.Printf("enums\n")
    fmt.Printf("Red: %x, Green: %x, Blue: %x, Mask: %x\n", Red, Green, Blue, ColorMask);


    fmt.Printf("first top of L\n");
    for i := 0; i < 10; i++ {
L:
        for {
            for {
                break L
                //continue L
            }
            fmt.Printf("Never reached:%d\n", i)
        }
        fmt.Printf("L: %d\n", i)
    }

    var x Example
    var z integer
    x.Val = "bart"
    x.count = 42
    fmt.Printf("%d %s\n", x.count, x.Val)
    z = 37
    fmt.Printf("z: %d\n", z)
    //log(z)
    y := Log(&x)
    fmt.Printf("Log ret: %d\n", y)

    DoGeom()

    stackTest();
}

