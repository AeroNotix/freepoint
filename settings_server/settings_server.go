package main

import (
	"encoding/json"
	"fmt"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
	"io"
	"log"
	"net/http"
	"strings"
	"time"
)

type User struct {
	User     string
	Password string
}

// This type defines what signatures of functions can be used
// as request handlers.
type RouterHandler func(w http.ResponseWriter, req *http.Request)

// Basic RoutingEntry
type RoutingEntry struct {
	URL     string
	Handler RouterHandler
}

type SettingsHandler struct{}

type JSONMessage struct {
	Rows     [][]string
	Metadata map[string]map[string][]string
}

var Routes []RoutingEntry = []RoutingEntry{
	RoutingEntry{
		URL:     "params",
		Handler: databaseParameters,
	},
	RoutingEntry{
		URL:     "login",
		Handler: userLogin,
	},
}

func (self *SettingsHandler) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	request := strings.Split(req.URL.Path, "/")
	for _, route := range Routes {
		if request[1] == route.URL {
			route.Handler(w, req)
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
		fmt.Println(err)
	}
}

// Function for logging in the user
func userLogin(w http.ResponseWriter, req *http.Request) {

	if req.Method != "POST" {
		return
	}

	db := mysql.New(
		"tcp",
		"",
		"127.0.0.1:3306",
		"root",
		"root",
		"db_freepoint",
	)

	w.Header().Set("Content-type", "application/json")

	if err := db.Connect(); err != nil {
		json.NewEncoder(w).Encode(map[string]bool{"success": false})
		fmt.Println(err)
		return
	}

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

	// Prepare the statement and execute
	stmt, _ := db.Prepare("SELECT * FROM tblusers WHERE userid=(?)")
	resp, _ := stmt.Run(user.User)
	row, _ := resp.GetRow()
	if len(row) == 0 {
		sendJSON(w, "user not found")
		return
	}
	// Create a User instance from the SQL Results.
	req_user := User{
		row.Str(1),
		row.Str(2),
	}

	var success bool = user == req_user
	json.NewEncoder(w).Encode(map[string]bool{"success": success})
	return
}

func sendJSON(w http.ResponseWriter, message string) {
	json.NewEncoder(w).Encode(map[string]string{"error": message})
	return
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
