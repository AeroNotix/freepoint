package settingsserver

import (
	"encoding/json"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
	"connection_details"
)

type User struct {
	User     string
	Password string
}

func CreateConnection() (mysql.Conn, error) {
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

// Given a user string we grab the user from
// the database, if the user exists we throw
// back a single row, if not an error
func GetUser(user string) (mysql.Row, error) {

	db, err := CreateConnection()
	if err != nil {
		return nil, err
	}
	// multiple return paths so we just defer closing till then
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
		return nil, LoginError
	}
	return row, nil
}

func GetMetadata(table string) (Metadata, error) {
	db, err := CreateConnection()
	if err != nil {
		return nil, err
	}

	// multiple return paths so we defer the database close
	defer db.Close()

	stmt, err := db.Prepare("SELECT metadata FROM metadata WHERE tablename=(?)")
	if err != nil {
		return nil, err
	}
	resp, err := stmt.Run(table)
	if err != nil {
		return nil, err
	}
	row, err := resp.GetRow()
	if len(row) != 1 {
		return nil, InvalidTable
	}
	if err != nil {
		return nil, err
	}

	metadata := make(Metadata)
	err = json.Unmarshal(row[0].([]byte), &metadata)
	if err != nil {
		return nil, err
	}

	return metadata, nil
}