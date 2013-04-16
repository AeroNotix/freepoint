/*
 This file works as the server-side implementation for a mysql database layer
 which marshals access rights, metadata, selects and rewrites.
*/

package main

import (
	"crypto/sha512"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"regexp"
	ss "settingsserver"
	"time"
)

func updateMetadata(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {
	job, err := ss.NewAsyncMetadataUpdate(req)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}
	err = self.UpdateEntry(job)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}
	ss.SendJSON(w, true)
	return nil
}

// This is a handler which checks the POST for the data required to make
// a change to the database.
//
// This function will have one of two possible side effects which are
// writing a JSON response to the caller.
func changeTable(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	job, err := ss.NewAsyncJob(req)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}

	err = self.WriteEntry(job)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}

	ss.SendJSON(w, true)
	return nil
}

// createTable serves as the back-end for creating new tables. The request holds the
// form data which is needed to create an SQL String which is then executed and the
// error status returned to the client.
func createTable(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	job, err := ss.NewAsyncCreate(req)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}

	err = self.CreateEntry(job)
	if err != nil {
		ss.SendJSON(w, false)
		return err
	}

	ss.SendJSON(w, true)
	return nil
}

// InsertData server as the back-end for handling data insertion into existing tables.
// The request holds all the relevent data encoded into rows.
func insertData(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	job, err := ss.NewAsyncInsert(req)
	if err != nil {
		ss.SendJSONError(w, err)
		return err
	}

	err = self.InsertEntry(job)
	if err != nil {
		ss.SendJSONError(w, err)
		return err
	}

	ss.SendJSON(w, true)
	return nil
}

// deleteRows handles the deletion of a group of rows from a table.
func deleteRows(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	job, err := ss.NewAsyncDelete(req)
	if err != nil {
		ss.SendJSONError(w, err)
		return err
	}
	err = self.DeleteEntries(job)
	if err != nil {
		ss.SendJSONError(w, err)
	}

	ss.SendJSON(w, true)
	return nil
}

// Simple server which is used to return parameters for particular databases.
func databaseParameters(self *ss.AppServer, w http.ResponseWriter, req *http.Request) error {

	// We add the content type so the browser can accept it properly if
	// we get requested via a browser.
	w.Header().Set("Content Type", "application/json")

	// We retrieve the JSON string and encode it
	// into the proper JSON request struct.
	json_dec := json.NewDecoder(req.Body)
	dbreq := new(ss.DatabaseRequest)
	err := json_dec.Decode(&dbreq)
	if err != nil {
		ss.SendJSON(w, false)
		log.Println(err)
		return err
	}

	// Get the metadata associated with the table.
	metadata, err := ss.GetMetadata(dbreq)
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

	rows, err := ss.GetRows(dbreq)
	if err != nil {
		return err
	}
	headings, err := ss.GetHeadings(dbreq)
	if err != nil {
		log.Println(err)
		return err
	}
	// Add the column names to the jsonMap
	jsonMap.Headings = headings

	// Iterate through the rows, create a [][]byte to hold a row.
	// Then we encode the row by asserting it to []byte and appending
	// it to the [][]byte row. We then AddRow that row.
	for _, row := range rows {
		var newrow = [][]byte{}
		for _, item := range row {
			data, ok := item.([]byte)
			if ok {
				newrow = append(newrow, data)
			}
		}
		jsonMap.AddRow(newrow)
	}

	// Send the JSON to the client.
	err = json.NewEncoder(w).Encode(jsonMap)
	return err
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
	session, err := ss.CreateMySQLSession()
	if err != nil {
		fmt.Println("Error creating cookie: " + err.Error())
	} else {
		http.SetCookie(w, &http.Cookie{
			Name:   "session",
			Value:  string(session.Key),
			Domain: req.Host,
		})
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
	// We retrieve the JSON string and encode it
	// into the proper JSON request struct.
	json_dec := json.NewDecoder(req.Body)
	userdata := new(ss.User)
	err := json_dec.Decode(&userdata)
	if err != nil {
		ss.SendJSON(w, false)
		log.Println(err)
		return false, err
	}
	h := sha512.New()
	io.WriteString(h, userdata.Password)
	userdata.Password = fmt.Sprintf("%x", h.Sum(nil))
	row, err := ss.GetUser(userdata.Username)
	if err != nil {
		log.Println(err)
		return false, err
	}
	// Create a User instance from the SQL Results.
	req_user := ss.User{
		row.Str(1),
		row.Str(2),
	}
	// if we've got here, we either are logged in or not.
	success := *userdata == req_user
	if !success {
		log.Println(ss.LoginError)
		return success, ss.LoginError
	}
	log.Printf("%s logged in.\n", req_user.Username)
	return success, nil
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
				regexp.MustCompile("^/getdb/$"),
				databaseParameters,
				"Parameters",
				true,
			),
			ss.NewRoute(
				regexp.MustCompile("^/login/$"),
				userLogin,
				"Login",
				false,
			),
			ss.NewRoute(
				regexp.MustCompile("^/update/$"),
				changeTable,
				"Change Table Data",
				true,
			),
			ss.NewRoute(
				regexp.MustCompile("^/create/$"),
				createTable,
				"Create table",
				true,
			),
			ss.NewRoute(
				regexp.MustCompile("^/insert/$"),
				insertData,
				"Inserting data",
				true,
			),
			ss.NewRoute(
				regexp.MustCompile("^/delete/$"),
				deleteRows,
				"Deleting data",
				true,
			),
			ss.NewRoute(
				regexp.MustCompile("^/meta/$"),
				updateMetadata,
				"Updating metadata",
				true,
			),
		},
	)
	Settings.Authenticator = func(w http.ResponseWriter, req *http.Request) bool {
		fmt.Println(req.Cookies())
		return true
	}
	s := http.Server{
		Addr:        addr,
		Handler:     &Settings,
		ReadTimeout: 30 * time.Second,
	}
	err := s.ListenAndServe()
	if err != nil {
		log.Println(err)
	}
}
