package main

import (
	"connection_details"
	"encoding/json"
	"errors"
	"flag"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
	"io"
	"log"
	"net/http"
	"regexp"
	"time"
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

type JSONMessage struct {
	Rows     [][]string
	Metadata map[string]map[string][]string
}

var Routes []RoutingEntry = []RoutingEntry{
	RoutingEntry{
		URL:     regexp.MustCompile("^/params/[A-Za-z_-]*/?$"),
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
			log.Println("Matched:", route.Name)
			route.Handler(w, req)
			return
		}
	}
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
		log.Println(err)
	}
}

// Function for logging in the user
func userLogin(w http.ResponseWriter, req *http.Request) {

	if req.Method != "POST" {
		return
	}

	w.Header().Set("Content-type", "application/json")

	username := req.FormValue("User")
	pass := req.FormValue("Password")
	if len(username) == 0 || len(pass) == 0 {
		sendJSON(w, "Missing username or password")
		return
	}

	user := User{
		User:     username,
		Password: pass,
	}

	row, err := getUser(user.User)
	if err != nil {
		sendJSON(w, err)
		return
	}

	// Create a User instance from the SQL Results.
	req_user := User{
		row.Str(1),
		row.Str(2),
	}

	// if we've got here, we either are logged in or not.
	sendJSON(w, user == req_user)
	return
}

// Sends JSON using the message as the payload. Determines the
// underlying type and uses some specific handling for that type.
func sendJSON(w http.ResponseWriter, message interface{}) {

	// type switch on the message interface
	switch message.(type) {
	default:
		return
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

func createConnection() mysql.Conn {
	db := mysql.New(
		"tcp",
		"",
		connection_details.IP,
		connection_details.User,
		connection_details.Password,
		connection_details.Database,
	)
	return db
}

func getUser(user string) (mysql.Row, error) {

	db := createConnection()
	defer db.Close()
	if err := db.Connect(); err != nil {
		return nil, err
	}

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
		return nil, errors.New("No users found with that userid")
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
