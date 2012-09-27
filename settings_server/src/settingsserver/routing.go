package settingsserver

import (
	"log"
	"net/http"
	"regexp"
)

// Struct so that we may assign ServeHTTP to something to satisfy the
// server interface
type AppServer struct {
	Routes     []RoutingEntry
	dbWriter   chan AsyncUpdate
	dbCreator  chan AsyncCreate
	dbInserter chan AsyncInsert
	dbDeleter  chan AsyncDelete
}

// NewAppServer allows us to take the steps required for running the server.
//
// As the server is asynchronous in nature, we have chosen to include
// synchronous writes. Simply because this makes the complexity of read/
// write locks much easier to deal with. Having synchronous writes also
// allows us to add-on functionality in the future using patterns such as
// the observer pattern.
//
// NewAppServer takes a []RoutingEntry which are the various functions/URLS
// which the Server will iterate through and serve once a request has been
// recieved. We then kick off three goroutines which monitor a *specific*
// channel for certain job types in order to process them synchronously.
func NewAppServer(routes []RoutingEntry) AppServer {
	app := AppServer{
		Routes:     routes,
		dbWriter:   make(chan AsyncUpdate, 10),
		dbCreator:  make(chan AsyncCreate, 10),
		dbInserter: make(chan AsyncInsert, 10),
		dbDeleter:  make(chan AsyncDelete, 10),
	}

	go AsyncUpdater(app.dbWriter)
	go AsyncCreator(app.dbCreator)
	go AsyncInserter(app.dbInserter)
	go AsyncDeleter(app.dbDeleter)

	return app
}

// This is the main 'event loop' for the web server. All requests are
// sent to this handler, which checks the incoming request against
// all the routes we have setup if it finds a match it will invoke
// the handler which is attached to that match.
func (self *AppServer) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	request := req.URL.Path
	for _, route := range self.Routes {
		matches := route.URL.FindAllStringSubmatch(request, 1)
		if len(matches) > 0 {
			log.Println("Request:", route.Name)
			err := route.Handler(self, w, req)
			if err != nil {
				log.Println(err)
			}
			return
		}
	}
	http.NotFound(w, req)
}

// The next three functions {Write|Create|Insert}Entry are utility functions
// which allow the API to not leak implementation details to the user. They
// simply pass the correct type to this function and check the return value
// all the while they will internally deal with the details of sending and
// recieving to/from channels.
//
// Each are methods of the AppServer and return an error.
func (self *AppServer) WriteEntry(job AsyncUpdate) error {
	self.dbWriter <- job
	return <-job.ReturnPath
}
func (self *AppServer) CreateEntry(job AsyncCreate) error {
	self.dbCreator <- job
	return <-job.ReturnPath
}
func (self *AppServer) InsertEntry(job AsyncInsert) error {
	self.dbInserter <- job
	return <-job.ReturnPath
}
func (self *AppServer) DeleteEntries(job AsyncDelete) error {
	self.dbDeleter <- job
	return <-job.ReturnPath
}

// This type defines what signatures of functions can be used
// as request handlers.
type RouterHandler func(self *AppServer, w http.ResponseWriter, req *http.Request) error

// RoutingEntry takes a URL, a RouterHandler and a String
// we define the URL as a regex because this way we can
// have a rich url interface to our application.
//
// For example we can have something like:
// domain/getdb/[A-Za-z]/ automatically retrieve the database
// which the group matches without parsing the url manually.
// RouterHandler is a type which is defined in this package.
// Name simply allows us to have a 'pretty' version of the function
// name.
type RoutingEntry struct {
	URL     *regexp.Regexp
	Handler RouterHandler
	Name    string
}

// NewRoute returns a RoutingEntry instance.
func NewRoute(r *regexp.Regexp, h RouterHandler, name string) RoutingEntry {
	route := RoutingEntry{
		URL:     r,
		Handler: h,
		Name:    name,
	}
	return route
}
