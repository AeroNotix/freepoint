package settingsserver

import (
	"encoding/json"
	"net/http"
)

// This type allows us to easily marshal incoming JSON data into
// the fields of a struct by statically mapping the values.
// Our JSON will be a map of maps to interface types.
type Metadata map[string]map[string]interface{}

// A JSON message in our case will be a [][]string field
// which corresponds to the rows in a database call and
// we also need to get the metadata for that call so
// we need to have a metadata field in our message. This
// way we can easily marshal data into the json wire
// format.
type JSONMessage struct {
	Rows     [][]string
	Metadata Metadata
	Headings []string
}

// Initializes a JSONMessage structure
func NewJSONMessage(md Metadata) JSONMessage {
	var message JSONMessage
	message.Metadata = md
	return message
}

// Sends JSON using the message as the payload. Determines the
// underlying type and uses some specific handling for that type.
func SendJSON(w http.ResponseWriter, message interface{}) {
	w.Header().Set("Content-type", "application/json")
	// type switch on the message interface
	switch message.(type) {
	case string:
		json.NewEncoder(w).Encode(map[string]string{
			"Message": message.(string),
		})
	case bool:
		json.NewEncoder(w).Encode(map[string]bool{
			"Success": message.(bool),
		})
	}
	return
}

func SendJSONError(w http.ResponseWriter, message error) {
	w.Header().Set("Content-type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{
		"Error": message.Error(),
	})
}

// Helper method to clean up syntax of adding new rows
// to the map
func (self *JSONMessage) AddRow(row [][]byte) {
	str_add := []string{}
	for _, item := range row {
		str_add = append(str_add, string(item))
	}
	self.Rows = append(self.Rows, str_add)
}

// Adds metadata to the JSONMessage object
func (self *JSONMessage) AddMetadata(heading, option string, data interface{}) {
	if _, ok := self.Metadata[heading]; !ok {
		self.Metadata[heading] = make(map[string]interface{})
	}
	self.Metadata[heading][option] = data
}
