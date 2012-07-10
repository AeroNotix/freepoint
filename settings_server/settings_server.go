/*
 This file works as the server-side implementation for a mysql database layer
 which marshals access rights, metadata, selects and rewrites.
*/

package main

import (
	"encoding/json"
	"flag"
	"log"
	"net/http"
	"regexp"
	ss "settingsserver"
	"time"
)

func createTable(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {
	job := ss.NewAsyncCreate(req)
	err := self.CreateEntry(job)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}

	ss.SendJSON(w, true)
	return nil
}

func insertData(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {
	log.Println(w)
	js := json.NewDecoder(req.Body)
	mapper := new(ss.InsertData) // New causes mapper to be a pointer
	err := js.Decode(&mapper)
	if err != nil {
		log.Println(err)
		return err
	}
	job := ss.NewAsyncInsert(*mapper) // We need a dereferenced job here
	err = self.InsertEntry(job)
	if err != nil {
		log.Println(err)
		return err
	}
	return nil
}

// Simple server which is used to return parameters for particular databases.
func databaseParameters(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	// We add the content type so the browser can accept it properly if
	// we get requested via a browser.
	w.Header().Set("Content Type", "application/json")

	// Get the metadata associated with the table.
	metadata, err := ss.GetMetadata(req.URL.Path[len("/getdb/"):])
	if err != nil {
		val, ok := err.(ss.AppError)
		if !ok {
			ss.SendJSONError(w, err)
			return err
		}
		if val.Code() != 3 {
			ss.SendJSONError(w, err)
			return err
		}
	}

	// Marshal our metadata into a struct and encode.
	jsonMap := ss.NewJSONMessage(metadata)
	log.Println(metadata)

	// Get the rows from the database and encode them into JSON.
	database_name := req.FormValue("Database")
	table_name := req.FormValue("Table")
	rows, _ := ss.GetRows(
		database_name,
		table_name,
	)

	headings, err := ss.GetHeadings(database_name, table_name)
	if err != nil {
		return err
	}
	// Add the column names to the jsonMap
	jsonMap.Headings = headings
	for _, row := range rows {
		var newrow = [][]byte{}
		for _, item := range row {
			newrow = append(newrow, item.([]byte))
		}
		jsonMap.AddRow(newrow)
	}

	// Send the JSON to the client.
	err = json.NewEncoder(w).Encode(jsonMap)
	if err != nil {
		return err
	}

	return nil
}

// This is a URL attached to ^login/?$ because then we can
// programmatically log in the user and also check if the
// user is login-able. Eventually userLogin will create a
// session row in the database and send the sessionid key
// back to the requester.
func userLogin(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {
	if ok, err := Login(w, req); !ok {
		ss.SendJSONError(w, err)
		return err
	}
	ss.SendJSON(w, true)
	return nil
}

// Checks if the user is a valid user.
// If the user is not, then we will tell the user ourselves
// and return false. We return the error message ourself
// because other with we would have a string return type
// or multiple return types which need to be parsed out
// or error checks on them and this seems cleaner.
func Login(w http.ResponseWriter, req *http.Request) (bool, error) {

	if req.Method != "POST" {
		return false, ss.RequestError
	}

	username := req.FormValue("User")
	pass := req.FormValue("Password")
	if len(username) == 0 || len(pass) == 0 {
		return false, ss.LoginError
	}
	user := ss.User{
		User:     username,
		Password: pass,
	}
	row, err := ss.GetUser(user.User)
	if err != nil {
		return false, err
	}
	// Create a User instance from the SQL Results.
	req_user := ss.User{
		row.Str(1),
		row.Str(2),
	}
	// if we've got here, we either are logged in or not.
	success := user == req_user
	if !success {
		return success, ss.LoginError
	}
	return success, nil
}

// This is a handler which checks the POST for the data required to make
// a change to the database.
//
// This function will have one of two possible side effects which are
// writing a JSON response to the caller.
func changeTable(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	job := ss.NewAsyncJob(req)
	err := self.WriteEntry(job)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}
	ss.SendJSON(w, true)
	return nil
}

func main() {
	var addr string
	flag.StringVar(&addr, "port", ":12345",
		"The port on which the server should run",
	)
	flag.Parse()
	log.Println("Serving on", addr)

	Settings := ss.NewAppServer(
		// Routes is a slice of RoutingEntries, this allows
		// us to hold routes and subsequently iterate through
		// them to find which we are currently serving.
		[]ss.RoutingEntry{
			ss.NewRoute(
				regexp.MustCompile("^/getdb/[A-Za-z0-9._-]*/?$"),
				databaseParameters,
				"Parameters",
			),
			ss.NewRoute(
				regexp.MustCompile("^/login/$"),
				userLogin,
				"Login",
			),
			ss.NewRoute(
				regexp.MustCompile("^/update/$"),
				changeTable,
				"Change Table Data",
			),
			ss.NewRoute(
				regexp.MustCompile("^/create/$"),
				createTable,
				"Create table",
			),
			ss.NewRoute(
				regexp.MustCompile("^/insert/$"),
				insertData,
				"Inserting data",
			),
		},
	)

	s := http.Server{
		Addr:        addr,
		Handler:     &Settings,
		ReadTimeout: 30 * time.Second,
	}
	s.ListenAndServe()
}
