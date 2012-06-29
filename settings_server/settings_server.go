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
	"settingsserver"
	"time"
)

// Routes is a slice of RoutingEntries, this allows
// us to hold a map (and subsequently iterate through
// it.)
var Routes []settingsserver.RoutingEntry = []settingsserver.RoutingEntry{
	settingsserver.RoutingEntry{
		URL:     regexp.MustCompile("^/getdb/[A-Za-z0-9._-]*/?$"),
		Handler: databaseParameters,
		Name:    "Parameters",
	},
	settingsserver.RoutingEntry{
		URL:     regexp.MustCompile("^/login/$"),
		Handler: userLogin,
		Name:    "Login",
	},
	settingsserver.RoutingEntry{
		URL:     regexp.MustCompile("^/update/$"),
		Handler: changeTable,
		Name:    "Change Table Data",
	},
}

// Struct so that we may assign ServeHTTP to something to satisfy the
// server interface
type SettingsHandler struct{}

// This is the main 'event loop' for the web server. All requests are
// sent to this handler, which checks the incoming request against
// all the routes we have setup if it finds a match it will invoke
// the handler which is attached to that match.
func (self *SettingsHandler) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	request := req.URL.Path
	for _, route := range Routes {
		matches := route.URL.FindAllStringSubmatch(request, 1)
		if len(matches) > 0 {
			log.Println("Request:", route.Name)
			err := route.Handler(w, req)
			if err != nil {
				log.Println(err)
			}
			return
		}
	}
	http.NotFound(w, req)
}

// Simple server which is used to return parameters for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) error {

	// We add the content type so the browser can accept it properly if
	// we get requested via a browser.
	w.Header().Set("Content Type", "application/json")

	// Get the metadata associated with the table.
	metadata, err := settingsserver.GetMetadata(req.URL.Path[len("/getdb/"):])
	if err != nil {
		val, ok := err.(settingsserver.AppError)
		if !ok {
			settingsserver.SendJSONError(w, err)
			return err
		}
		if val.Code() != 3 {
			settingsserver.SendJSONError(w, err)
			return err
		}
	}

	// Marshal our metadata into a struct and encode.
	jsonMap := settingsserver.NewJSONMessage(metadata)

	// Get the rows from the database and encode them into JSON.
	database_name := req.FormValue("Database")
	table_name := req.FormValue("Table")
	rows, _ := settingsserver.GetRows(
		database_name,
		table_name,
	)

	headings, err := settingsserver.GetHeadings(database_name, table_name)
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
func userLogin(w http.ResponseWriter, req *http.Request) error {
	if ok, err := Login(w, req); !ok {
		settingsserver.SendJSONError(w, err)
		return err
	}
	settingsserver.SendJSON(w, true)
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
		return false, settingsserver.RequestError
	}

	username := req.FormValue("User")
	pass := req.FormValue("Password")
	if len(username) == 0 || len(pass) == 0 {
		return false, settingsserver.LoginError
	}
	user := settingsserver.User{
		User:     username,
		Password: pass,
	}
	row, err := settingsserver.GetUser(user.User)
	if err != nil {
		return false, err
	}
	// Create a User instance from the SQL Results.
	req_user := settingsserver.User{
		row.Str(1),
		row.Str(2),
	}
	// if we've got here, we either are logged in or not.
	success := user == req_user
	if !success {
		return success, settingsserver.LoginError
	}
	return success, nil
}

// This is a handler which checks the POST for the data required to make
// a change to the database.
//
// This function will have one of two possible side effects which are
// writing a JSON response to the caller.
func changeTable(w http.ResponseWriter, req *http.Request) error {
	err := settingsserver.ChangeData(
		req.FormValue("Database"),
		req.FormValue("Table"),
		req.FormValue("Column"),
		req.FormValue("Data"),
		req.FormValue("ID"),
	)
	if err != nil {
		settingsserver.SendJSON(w, false)
		return err
	}
	settingsserver.SendJSON(w, true)
	return nil
}

func main() {
	var addr string

	asyncWriterChannel := make(chan settingsserver.AsyncUpdate, 10)
	go func() {
		settingsserver.AsyncUpdater(asyncWriterChannel)
	}()
	flag.StringVar(&addr, "port", ":12345",
		"The port on which the server should run",
	)
	flag.Parse()
	log.Println("Serving on", addr)
	s := http.Server{
		Addr:        addr,
		Handler:     &SettingsHandler{},
		ReadTimeout: 30 * time.Second,
	}
	s.ListenAndServe()
}