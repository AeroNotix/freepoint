package main

import (
	"connection_details"
	"encoding/json"
	"errors"
	"flag"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
	"log"
	"net/http"
	"regexp"
	"time"
	"settingsserver"
)

type Variable interface{}

type User struct {
	User     string
	Password string
}

// This type defines what signatures of functions can be used
// as request handlers.
type RouterHandler func(w http.ResponseWriter, req *http.Request)

// Basic RoutingEntry
type RoutingEntry struct {
	URL     *regexp.Regexp
	Handler RouterHandler
	Name    string
}

type SettingsHandler struct{}

type Metadata map[string]map[string][]interface{}

type JSONMessage struct {
	Rows     [][]string
	Metadata Metadata
}

// Routes is a slice of RoutingEntries, this allows
// us to hold a map (and subsequently iterate through
// it.)
var Routes []RoutingEntry = []RoutingEntry{
	RoutingEntry{
		URL:     regexp.MustCompile("^/getdb/[A-Za-z0-9._-]*/?$"),
		Handler: databaseParameters,
		Name:    "Parameters",
	},
	RoutingEntry{
		URL:     regexp.MustCompile("^/login/$"),
		Handler: userLogin,
		Name:    "Login",
	},
}

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

// Initializes a JSONMessage structure
func NewJSONMessage(md Metadata) JSONMessage {
	var message JSONMessage
	message.Metadata = md
	return message
}

// Helper method to clean up syntax of adding new rows
// to the map
func (self *JSONMessage) AddRow(row []string) {
	self.Rows = append(self.Rows, row)
}

// Adds metadata to the JSONMessage object
func (self *JSONMessage) AddMetadata(heading, option string, data []interface{}) {
	if _, ok := self.Metadata[heading]; !ok {
		self.Metadata[heading] = make(map[string][]interface{})
	}
	self.Metadata[heading][option] = data
}

// Simple server which is used to store and return parameters
// for particular databases.
func databaseParameters(w http.ResponseWriter, req *http.Request) {
	db, err := createConnection()
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
		sendJSON(w, "Table not found")
		return
	}
	if err != nil {
		log.Println(err)
		return
	}

	w.Header().Set("Content-type", "application/json")
	metadata := make(Metadata)
	err = json.Unmarshal(row[0].([]byte), &metadata)
	if err != nil {
		log.Println(err)
	}
	jsonMap := NewJSONMessage(metadata)
	err = json.NewEncoder(w).Encode(jsonMap)
	if err != nil {
		log.Println(err)
		return
	}
}

/* 
 This is a URL attached to ^login/?$ because then we can
 programmatically log in the user and also check if the
 user is login-able. Eventually userLogin will create a
 session row in the database and send the sessionid key
 back to the requester.
*/
func userLogin(w http.ResponseWriter, req *http.Request) {
	if !Login(w, req) {
		return
	}
	sendJSON(w, "Logged in!")
	return
}

// Function for checking if the user is a valid user
func Login(w http.ResponseWriter, req *http.Request) bool {

	if req.Method != "POST" {
		return false
	}

	w.Header().Set("Content-type", "application/json")

	username := req.FormValue("User")
	pass := req.FormValue("Password")
	if len(username) == 0 || len(pass) == 0 {
		sendJSON(w, "Missing username or password")
		return false
	}

	user := User{
		User:     username,
		Password: pass,
	}

	row, err := getUser(user.User)
	if err != nil {
		sendJSON(w, err)
		return false
	}

	// Create a User instance from the SQL Results.
	req_user := User{
		row.Str(1),
		row.Str(2),
	}

	// if we've got here, we either are logged in or not.
	return user == req_user
}

// Sends JSON using the message as the payload. Determines the
// underlying type and uses some specific handling for that type.
func sendJSON(w http.ResponseWriter, message interface{}) {

	// type switch on the message interface
	switch message.(type) {
	case error:
		json.NewEncoder(w).Encode(map[string]string{
			"Error": message.(error).Error(),
		})
	case string:
		json.NewEncoder(w).Encode(map[string]string{
			"Error": message.(string),
		})
	case bool:
		json.NewEncoder(w).Encode(map[string]bool{
			"Success": message.(bool),
		})
	}
	return
}

func createConnection() (mysql.Conn, error) {
	db := mysql.New(
		"tcp",
		"",
		connection_details.IP,
		connection_details.User,
		connection_details.Password,
		connection_details.Database,
	)
	err := db.Connect()
	if err != nil {
		return nil, err
	}
	return db, nil
}

func getUser(user string) (mysql.Row, error) {

	db, err := createConnection()
	if err != nil {
		log.Println(err)
		return nil, err
	}
	defer db.Close()
	// Prepare the statement and execute
	stmt, err := db.Prepare("SELECT * FROM tblusers WHERE userid=(?)")
	if err != nil {
		return nil, err
	}
	resp, err := stmt.Run(user)
	if err != nil {
		return nil, err
	}
	row, err := resp.GetRow()
	if err != nil {
		return nil, err
	}
	if len(row) == 0 {
		return nil, errors.New("1: Login Failure")
	}

	return row, nil
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
