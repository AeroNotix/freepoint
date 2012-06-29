package settingsserver

import (
	"connection_details"
	"encoding/json"
	"fmt"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
)

// Basic struct to hold User data.
// 
// This was created so we have better semantics for marshaling/unmarshaling data which
// holds the fields for the User data.
type User struct {
	User     string
	Password string
}

// This struct will be used to create jobs which can be push into async worker queues
// to asynchronously process database writes and have them write to disk sequentially.
type AsyncUpdate struct {
	Database string
	Table string
	Column string
	Data string
	Id string
	ReturnPath chan error
}

// Function which takes a database name and connects to that database using the details
// defined in the connection module.
func CreateConnection(dbname string) (mysql.Conn, error) {
	db := mysql.New(
		"tcp",
		"",
		connection_details.IP,
		connection_details.User,
		connection_details.Password,
		dbname,
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

	db, err := CreateConnection(connection_details.SettingsDatabase)
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

// GetMetadata connects to our database which
func GetMetadata(table string) (Metadata, error) {
	db, err := CreateConnection(connection_details.SettingsDatabase)
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

func GetRows(database, table string) ([]mysql.Row, error) {
	db, err := CreateConnection(database)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	sqlStr := fmt.Sprintf("SELECT * FROM %s", table)
	rows, _, err := db.Query(sqlStr)
	if err != nil {
		return nil, err
	}
	return rows, nil
}

func GetHeadings(database, table string) ([]string, error) {
	db, err := CreateConnection(database)
	defer db.Close()
	defer db.Query("DROP TABLE temp")
	if err != nil {
		return nil, err
	}
	sqlStr := fmt.Sprintf(`SHOW COLUMNS FROM %s`, table)
	rows, _, err := db.Query(sqlStr)
	if err != nil {
		return nil, err
	}

	headers := []string{}
	for _, row := range rows {
		headers = append(headers, string(row[0].([]byte)))
	}
	return headers, nil
}

// AsyncUpdater monitors a channel of type AsyncUpdate and blocks until it receives
// on it. Once it has received a job, it will process by calling ChangeData on the
// job and sending it's return value down the ReturnPath associated with the job.
func AsyncUpdater(jobqueue chan AsyncUpdate) {
	for {
		job := <-jobqueue
		job.ReturnPath <- ChangeData(job)
	}
}

func ChangeData(job AsyncUpdate) error {
	db, err := CreateConnection(job.Database)
	if err != nil {
		return err
	}

	defer db.Close()

	sqlStr := fmt.Sprintf(
		`UPDATE %s
		 SET %s="%s"
		 WHERE id=%s`,
		job.Table, job.Column, job.Data, job.Id,
	)

	_, _, err = db.Query(sqlStr)
	if err != nil {
		return err
	}
	return nil
}
