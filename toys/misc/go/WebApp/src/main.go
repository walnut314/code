package main

// gowebexamples.com -- great resource

import (
    "net/http"
    "os"
    //"io/ioutil"
    "fmt"
    "encoding/json"
    "html/template"
    "time"
)

type App struct {
    Port    string
    Router  *http.ServeMux
}

type UserResponse struct {
        ID    int    `json:"id"`
        Name  string `json:"name"`
        Email string `json:"email"`
}

func (a *App) indexHandler(w http.ResponseWriter, r *http.Request) {
    w.Write([]byte("<h1>Hi Cindy!</h1>"))
    fmt.Fprint(w, "<h1>Hello Again To You!</h1>")
}

type Response struct {
    CurrentTime string
    Name string
}

func (a *App) dudeHandler(w http.ResponseWriter, r *http.Request) {
    // http://localhost:5000/dude/?name=bart
    //nm := r.URL.Query().Get("name")
    //fmt.Fprintf(w, "Dude, wusup %s -> %d", nm, 77)

    ctime := Response{
        CurrentTime: time.Now().Format(time.RFC3339),
        Name: "hooda dawg",
    }
    byteArray, err := json.Marshal(ctime)
    if err != nil {
        fmt.Println(err)
    }
    w.Write(byteArray)
}

func (a *App) htmlHandler(w http.ResponseWriter, r *http.Request) {
    // http://localhost:5000/html
    // https://www.w3schools.com/html/html_forms.asp

/*    
    tmpl := `
        <!DOCTYPE html>
        <html>
        <head>
            <title>{{.Title}}</title>
        </head>
        <body>
            <h1>Hello, {{.Name}}!</h1>
            <p>Age: {{.Age}}</p>
        </body>
        </html>
        `

    t := template.Must(template.New("index").Parse(tmpl))
*/

type Todo struct {
    Title string
    Done  bool
}

type TodoPageData struct {
    PageTitle string
    Todos     []Todo
}

    t := template.Must(template.ParseFiles("layout.html"))
/*
    data := TodoPageData{
            PageTitle: "My TODO list",
            Todos: []Todo{
                {Title: "Quiet time", Done: false},
                {Title: "Church", Done: true},
                {Title: "Yard work", Done: true},
                {Title: "Pay bills", Done: false},
            },
    }
*/
    title1 := "Quiet time"
    title2 := "Church"
    title3 := "Yard walkaround"
    title4 := "Pay bills"
    ptitle := "dude just do it!"
    todo1 := Todo{Title: title1, Done: false}
    todo2 := Todo{Title: title2, Done: true}
    todo3 := Todo{Title: title3, Done: true}
    todo4 := Todo{Title: title4, Done: false}
    page  := TodoPageData{
        PageTitle: ptitle, 
            Todos: []Todo{
                todo1, 
                todo2,
                todo3,
                todo4,
                },
            }
    // next put data in CSV

/*
    data, err := ioutil.ReadFile("todo.txt")
    if err != nil {
        fmt.Printf("failed reading data from file: %s", err)
    }    
    fmt.Printf("%s\n", data)
*/

    err := t.Execute(w, page)
    if err != nil {
        http.Error(w, "Internal Server Error", http.StatusInternalServerError)
        return
    }
}

func (a *App) jsonHandler(w http.ResponseWriter, r *http.Request) {
    // http://localhost:5000/json

    resp := UserResponse{
                ID:    123,
                Name:  "Bart",
                Email: "bart@dude.com",
        }

    err := json.NewEncoder(w).Encode(resp)
    if err != nil {
            http.Error(w, err.Error(), 500)
            return
    }

    //data := map[string]string{"message":"Hello Json!"}
    //w.Header().Set("Content-Type", "application/json")
    //json.NewEncoder(w).Encode(data)
}

/*
func loggingMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        fmt.Printf("Incoming request: %s %s\n", r.Method, r.URL.Path)
        next.ServeHTTP(w,r)
    })
}
*/
/*
func loggingMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        fmt.Printf("Incoming request: %s %s\n", r.Method, r.URL.Path)
        next.ServeHTTP(w, r)
        })
}
*/

func logging(f http.HandlerFunc) http.HandlerFunc {
    fmt.Println("pre dude")
    return func(w http.ResponseWriter, r *http.Request) {
        fmt.Println(r.URL.Path)
        f(w, r)
    }
}

func middleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        fmt.Printf("Executing middleware pre-path: %s\n", r.URL.Path)
        switch (r.URL.Path) {
            case "/":
                fmt.Println("path: /")
            case "/dude/":
                fmt.Println("path: /dude/");
        }
        next.ServeHTTP(w, r)
        fmt.Printf("Executing middleware post-path: %s\n", r.URL.Path)
    })
}

func (a *App) Init() {
    //finalIndexHandler := http.HandlerFunc(a.indexHandler)
    finalDudeHandler  := http.HandlerFunc(a.dudeHandler)

    a.Router.Handle("/", http.FileServer(http.Dir("web")))    
    //a.Router.Handle("/",            middleware(finalIndexHandler))
    a.Router.Handle("/dude/",       middleware(finalDudeHandler))

    a.Router.HandleFunc("/json/",   a.jsonHandler)
    a.Router.HandleFunc("/html/",   a.htmlHandler)
}

func (a *App) Config() {
    a.Port = os.Getenv("PORT")
    if a.Port == "" { a.Port = "5000" }

}

func (a *App) Run() {
    fmt.Printf("Server is running on http:localhost:%s\n", a.Port)
    http.ListenAndServe(":"+a.Port, a.Router)
}

func main() {
    app := &App{}
    app.Router = http.NewServeMux()

    app.Config()
    app.Init()
    app.Run()
}
