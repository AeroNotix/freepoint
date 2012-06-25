package settingsserver

import (
	"encoding/json"
	"net/http"
)

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
