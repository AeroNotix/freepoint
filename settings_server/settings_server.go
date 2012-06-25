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
			route.Handler(w, req)
			return
		}
	}
}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	db, err := settingsserver.CreateConnection()
	if err != nil {
		log.Println(err)
		return
	}

	// multiple return paths so we defer the database close
	defer db.Close()
	stmt, err := db.Prepare("SELECT metadata FROM metadata WHERE tablename=(?)")
	if err != nil {
		log.Println(err)
		return
	}
	resp, err := stmt.Run(req.URL.Path[len("/getdb/"):])
	if err != nil {
		log.Println(err)
		return
	}
	row, err := resp.GetRow()
	if len(row) != 1 {
		settingsserver.SendJSON(w, "Table not found")
		return
	}
	if err != nil {
		log.Println(err)
		return
	}

	w.Header().Set("Content-type", "application/json")
	metadata := make(settingsserver.Metadata)
	err = json.Unmarshal(row[0].([]byte), &metadata)
	if err != nil {
		log.Println(err)
	}
	jsonMap := settingsserver.NewJSONMessage(metadata)
	err = json.NewEncoder(w).Encode(jsonMap)
	if err != nil {
		log.Println(err)
		return
	}
}

// This is a URL attached to ^login/?$ because then we can
// programmatically log in the user and also check if the
// user is login-able. Eventually userLogin will create a
// session row in the database and send the sessionid key
// back to the requester.
func userLogin(w http.ResponseWriter, req *http.Request) {
	if !Login(w, req) {
		settingsserver.SendJSON(w, "Error logging in!")
		return
	}
	settingsserver.SendJSON(w, "Logged in!")
	return
}

// Checks if the user is a valid user.
// If the user is not, then we will tell the user ourselves
// and return false. We return the error message ourself
// because other with we would have a string return type
// or multiple return types which need to be parsed out
// or error checks on them and this seems cleaner.
func Login(w http.ResponseWriter, req *http.Request) bool {

	if req.Method != "POST" {
		return false
	}

	w.Header().Set("Content-type", "application/json")

	username := req.FormValue("User")
	pass := req.FormValue("Password")
	if len(username) == 0 || len(pass) == 0 {
		settingsserver.SendJSON(w, "Missing username or password")
		return false
	}

	user := settingsserver.User{
		User:     username,
		Password: pass,
	}

	row, err := settingsserver.GetUser(user.User)
	if err != nil {
		settingsserver.SendJSON(w, err)
		return false
	}

	// Create a User instance from the SQL Results.
	req_user := settingsserver.User{
		row.Str(1),
		row.Str(2),
	}

	// if we've got here, we either are logged in or not.
	return user == req_user
}

func main() {
	var addr string
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
