package settingsserver

import (
	"encoding/json"
	"net/http"
)

// When inserting data we have a very strong idea about what the data
// should look like. Therefore we put the data into a struct which
// has very specific fields.
type InsertData struct {
	Data []string `json:"DATA"`
	Table string `json:"TABLE"`
	Database string `json:"DATABASE"`
}

// HeadingData is composed of strings to interfaces
type HeadingData map[string]interface{}

// This type allows us to easily marshal incoming JSON data into
// the fields of a struct by statically mapping the values.
// Our JSON will be a map of maps to interface types.
type Metadata map[string]HeadingData

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

// Statically defined JSON struct to marshal real
// JSON bytes into. We define all fields which
// can be in the JSON string but not all JSON data
// will hold it. This is allow in the spec and does
// not throw errors.
type Row struct {
	Rownum  int `json:"ROWNUM"`
	Rowdata struct {
		Unique  bool     `json:"UNIQUE"`
		Type    string   `json:"TYPE"`
		Null    bool     `json:"NULL"`
		Len     int      `json:"LEN"`
		Choices []string `json:"CHOICES"`
	} `json:"ROWDATA"`
}

// This type allows us to unmarshal incoming JSON data
// into a struct which contains a Map of strings to
// Rows. This greatly simplifies the communication
// between the client and the server.
type Headers struct {
	Headings map[string]Row
}

