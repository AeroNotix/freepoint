package settingsserver

import (
	"encoding/json"
	"net/http"
)

// This type allows us to easily marshal incoming JSON data into
// the fields of a struct by statically mapping the values.
// Our JSON will be a map of maps to interface types.
type Metadata map[string]map[string][]interface{}

// Initializes a JSONMessage structure
func NewJSONMessage(md Metadata) JSONMessage {
	var message JSONMessage
	message.Metadata = md
	return message
}

// Sends JSON using the message as the payload. Determines the
// underlying type and uses some specific handling for that type.
func SendJSON(w http.ResponseWriter, message interface{}) {

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

// Helper method to clean up syntax of adding new rows
// to the map
func (self *JSONMessage) AddRow(row []string) {
	self.Rows = append(self.Rows, row)
}

// A JSON message in our case will be a [][]string field
// which corresponds to the rows in a database call and
// we also need to get the metadata for that call so
// we need to have a metadata field in our message. This
// way we can easily marshal data into the json wire
// format.
type JSONMessage struct {
	Rows     [][]string
	Metadata Metadata
}

// Adds metadata to the JSONMessage object
func (self *JSONMessage) AddMetadata(heading, option string, data []interface{}) {
	if _, ok := self.Metadata[heading]; !ok {
		self.Metadata[heading] = make(map[string][]interface{})
	}
	self.Metadata[heading][option] = data
}
