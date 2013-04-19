/* This file works as the server-side implementation for a mysql
database layer which marshals access rights, metadata, selects and
rewrites.  */

package main

import (
	"connection_details"
	"flag"
	"log"
	"net/http"
	"os"
	"regexp"
	bk "settingsserver/backend"
	fe "settingsserver/frontend"
	"time"
)

var logfile *log.Logger = log.New(os.Stderr, "", log.Ldate|log.Llongfile)

func main() {
	var addr string
	flag.StringVar(&addr, "port", ":12345",
		"The port on which the server should run",
	)
	flag.Parse()
	log.Println("Serving on", addr)

	Settings := bk.NewAppServer(
		// Routes is a slice of RoutingEntries, this allows
		// us to hold routes and subsequently iterate through
		// them to find which we are currently serving.
		[]bk.RoutingEntry{
			bk.NewRoute(
				regexp.MustCompile("^/$"),
				fe.Index,
				"Index",
				false,
			),
			bk.NewRoute(
				regexp.MustCompile("^"+connection_details.StaticURL),
				fe.StaticFiles,
				"Static Files",
				false,
			),
			bk.NewRoute(
				regexp.MustCompile("^/user/$"),
				fe.DirectToTemplate("user.html"),
				"CreateUser",
				false,
			),
			bk.NewRoute(
				regexp.MustCompile("^/getdb/$"),
				fe.DatabaseParameters,
				"Parameters",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/login/$"),
				fe.UserLogin,
				"Login",
				false,
			),
			bk.NewRoute(
				regexp.MustCompile("^/loginfail/$"),
				fe.DirectToTemplate("loginfail.html"),
				"Failed login",
				false,
			),
			bk.NewRoute(
				regexp.MustCompile("^/update/$"),
				fe.ChangeTable,
				"Change Table Data",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/create/$"),
				fe.CreateTable,
				"Create table",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/insert/$"),
				fe.InsertData,
				"Inserting data",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/delete/$"),
				fe.DeleteRows,
				"Deleting data",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/meta/$"),
				fe.UpdateMetadata,
				"Updating metadata",
				true,
			),
			bk.NewRoute(
				regexp.MustCompile("^/createnewuser/$"),
				fe.CreateNewUser,
				"Create new user",
				true,
			),
		},
	)
	Settings.Authenticator = func(w http.ResponseWriter, req *http.Request) bool {
		if !bk.ValidateSession(req.Cookies()) {
			return false
		}
		switch req.URL.String() {
		case "/createnewuser/":
			return bk.CanCreateUsers(req.Cookies())
		}
		return true
	}
	s := http.Server{
		Addr:        addr,
		Handler:     &Settings,
		ReadTimeout: 30 * time.Second,
	}
	err := s.ListenAndServe()
	if err != nil {
		logfile.Println(err)
	}
}
