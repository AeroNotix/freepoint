package main

import (
	"encoding/json"
	"fmt"
	//	mysql "github.com/ziutek/mymysql/mysql"
	//	_ "github.com/ziutek/mymysql/native"
	"io"
	"log"
	"net/http"
	"strings"
	"time"
)

// This type defines what signatures of functions can be used
// as request handlers.
type RouterHandler func(w http.ResponseWriter, req *http.Request)

// Basic RoutingEntry
type routingEntry struct {
	URL     string
	Handler RouterHandler
}

var Routes []routingEntry = []routingEntry{
	routingEntry{
		URL:     "params",
		Handler: databaseParameters,
	},
}

type SettingsHandler struct{}

func (self *SettingsHandler) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	request := strings.Split(req.URL.Path, "/")
	for _, route := range Routes {
		if request[1] == route.URL {
			route.Handler(w, req)
		}
	}
}

type JSONMessage struct {
	Rows     [][]string
	Metadata map[string]map[string][]string
}

// Helper method to clean up syntax of adding new rows
// to the map
func (self *JSONMessage) AddRow(row []string) {
	self.Rows = append(self.Rows, row)
}

// Adds metadata to the JSONMessage object
func (self *JSONMessage) AddMetadata(heading, option string, data []string) {
	if _, ok := self.Metadata[heading]; !ok {
		self.Metadata[heading] = make(map[string][]string)
	}
	self.Metadata[heading][option] = data
}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	var out io.Writer = w
	w.Header().Set("Content-type", "application/json")
	var jsonMap JSONMessage

	err := json.NewEncoder(out).Encode(jsonMap)
	if err != nil {
		fmt.Println(err)
	}
}

func main() {
	http.HandleFunc("/params/", databaseParameters)

	s := http.Server{
		Addr:        ":12345",
		Handler:     &SettingsHandler{},
		ReadTimeout: 30 * time.Second,
	}

	err := s.ListenAndServe()
	if err != nil {
		fmt.Println(err)
	}
	err = http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
