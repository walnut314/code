package main

import (
    "log"
    "io/ioutil"
    "net/http"
    //"os"
)
 
// https://api.tomorrow.io/v4/weather/forecast?location=42.3478,-71.0466&apikey=TixnDC4LagTDLyxS2YSLXkL0lO2mXU8n'
func main() {
    req, err := http.NewRequest(
            http.MethodGet,
            //"https://api.climacell.co/v3/weather/forecast/hourly?lat=42.3826&lon=-71.1460&fields=temp",
            "https://api.tomorrow.io/v4/weather/forecast?location=42.3478,-71.0466",
            nil,
            )
    if err != nil {
        log.Fatalf("error creating HTTP request: %v", err) 
    }
 
    apikey := "TixnDC4LagTDLyxS2YSLXkL0lO2mXU8n"
    req.Header.Add("Accept", "application/json")
    req.Header.Add("apikey", apikey) //os.Getenv("CLIMACELL_API_KEY"))

    res, err := http.DefaultClient.Do(req)
    if err != nil {
        log.Fatalf("error sending HTTP request: %v", err)
    }

    responseBytes, err := ioutil.ReadAll(res.Body)
    if err != nil {
        log.Fatalf("error reading HTTP response: %v", err)
    }

    //log.Println("We got the response: ", string(responseBytes))
    log.Println("We got the response...")

/*
    out, err := os.Create("filename.ext")
    if err != nil {
        log.Fatalf("error creating output file: %v", err)
    }
    defer out.Close()
*/
    ioutil.WriteFile("weather.out", responseBytes, 0777)

}
