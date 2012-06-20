package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
)

type variable interface{}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	fmt.Println(req.URL)
	var out io.Writer = w
	w.Header().Set("Content-type", "application/json")
	jsonMap := map[string]map[string]variable{}
	jsonMap["WASSAAAAA"] = map[string]variable{"str": []variable{1, "B"}}
	err := json.NewEncoder(out).Encode(jsonMap)
	fmt.Println(err)
}

func Root(w http.ResponseWriter, req *http.Request) {
	io.WriteString(w, "pong")
}

func main() {
	//http.HandleFunc("/", Root)
	http.HandleFunc("/params/", databaseParameters)

	err := http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
