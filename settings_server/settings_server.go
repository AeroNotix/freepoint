package main

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
)

type variable interface{}

type JSONMessage struct {
	Rows [][]string
	Metadata map[string][][]string
}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	var out io.Writer = w
	w.Header().Set("Content-type", "application/json")
	jsonMap := JSONMessage{
		[][]string{
			[]string{"SUPDATA"},
		},
		map[string][][]string{
			"FIELDA": [][]string{
				[]string{"THIS", "IS", "DATA"},
			},
		},
	}
	
	for x := 0; x < 500; x++ {
		newRow := []string{"1", "2", "3"}
		jsonMap.Rows = append(jsonMap.Rows, newRow)
	}
	err := json.NewEncoder(out).Encode(jsonMap)
	fmt.Println(err)
}

func Root(w http.ResponseWriter, req *http.Request) {
	io.WriteString(w, "pong")
}
	
func main() {
	fmt.Println("STARTUP")
	//http.HandleFunc("/", Root)
	http.HandleFunc("/params/", databaseParameters)

	err := http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
