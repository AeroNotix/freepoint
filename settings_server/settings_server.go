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
	Metadata map[string]map[string][]string
}

// Helper method to clean up syntax of adding new rows
// to the map
func (js *JSONMessage) AddRow(row []string) {
	js.Rows = append(js.Rows, row)
}

// Adds metadata to the JSONMessage object
func (js *JSONMessage) AddMetadata(heading, option string, data []string) {
	if _, ok := js.Metadata[heading]; !ok {
		js.Metadata[heading] = make(map[string][]string)
	}
	js.Metadata[heading][option] = data
}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	var out io.Writer = w
	w.Header().Set("Content-type", "application/json")
	var jsonMap JSONMessage
	
	for x := 0; x < 500; x++ {
		newRow := []string{"1", "2", "3"}
		jsonMap.AddRow(newRow)
	}
	jsonMap.Metadata = make( map[string]map[string][]string )
	jsonMap.AddMetadata("HEADER2", "TYPE", []string{"123123"})
	jsonMap.AddMetadata("HEADER3", "TYPE", []string{"123123"})
	jsonMap.AddMetadata("HEADER3", "TYPE2", []string{"123123"})
	err := json.NewEncoder(out).Encode(jsonMap)
	fmt.Println(err)
}

func Root(w http.ResponseWriter, req *http.Request) {
	io.WriteString(w, "pong")
}
	
func main() {
	fmt.Println("STARTUP")
	http.HandleFunc("/params/", databaseParameters)

	err := http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
