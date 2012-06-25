package settingsserver

import (
	"regexp"
	"net/http"
)

// This type defines what signatures of functions can be used
// as request handlers.
type RouterHandler func(w http.ResponseWriter, req *http.Request)

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