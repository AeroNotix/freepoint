package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
)

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {

	if req.Method != "GET" {
		return
	}

	database := req.FormValue("db")
	if len(database) < 1 {
		return
	}

	/* 
		 This is where we will get the parameters for the table.
		 I want this to be a standard API for table parameters. So it will be
		 more than likely JSON because I like JSON and a tonne of languages have
		 a JSON decoder built-into their standard library.

		 An example output would be something like:

		json = {
			heading_name: {
				TYPE: COMBO-BOOL
			},
			heading_name2: {
				TYPE: COMBO-OPTION,
				OPTIONS: [Open, Closed, Error]
			}
		}

		 An example call:
		 getDatabaseParameters(database)
	*/

	// write some test json to the db
	io.WriteString(w, "{\"user_id\":}")
}

func Root(w http.ResponseWriter, req *http.Request) {
	io.WriteString(w, "pong")
}

func main() {
	http.HandleFunc("/", Root)
	http.HandleFunc("/databaseParameters", databaseParameters)

	err := http.ListenAndServe(":12345", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
