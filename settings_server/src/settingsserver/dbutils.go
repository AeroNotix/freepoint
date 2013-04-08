// Package groups together useful functions when dealing with databases
// it also defines the Asynchronous types and the job queues which deal with
// them.

package settingsserver

import (
	"connection_details"
	"encoding/json"
	"fmt"
	stressql "github.com/AeroNotix/stressql"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
	"log"
	"net/http"
	"strings"
)

// This struct will be used to create jobs which can be push into async worker queues
// to asynchronously process database writes and have them write to disk sequentially.
type AsyncUpdate struct {
	Database   string `json:"DATABASE"`
	Table      string `json:"TABLE"`
	Column     string `json:"COLUMN"`
	Data       string `json:"DATA"`
	Id         string `json:"ID"`
	ReturnPath chan error
}

// AsyncCreate holds the data from the New Table part of the API
// The data is parsed into an SQL string which is then executed.
type AsyncCreate struct {
	Database   string
	Table      string
	SQLString  string
	Metadata   string
	ReturnPath chan error
}

// AsyncInsert holds the data for inserting a new row into an existing
// table.
type AsyncInsert struct {
	InsertData
	ReturnPath chan error
}

// AsyncDelete holds the data for deleting rows from an existing
// table.
type AsyncDelete struct {
	DeleteData
	ReturnPath chan error
}

// Using the Request form values we create a job and return it.
// We do it like this so that the error channel doesn't have to
// be create by the user and they just pass the request form to
// this.
func NewAsyncJob(req *http.Request) (AsyncUpdate, error) {

	json_dec := json.NewDecoder(req.Body)
	updateRequest := new(AsyncUpdate)
	err := json_dec.Decode(&updateRequest)
	if err != nil {
		log.Println(err)
		return *updateRequest, err
	}
	updateRequest.ReturnPath = make(chan error)
	return *updateRequest, err
}

// NewAsyncCreate returns a basic representation of what an
// AsyncCreate instance might look like. We have a NewCreate()
// function as to not enforce the user to see implementation
// details about how the asynchronous nature of the update/create
// is accomplished
func NewAsyncCreate(req *http.Request) (out AsyncCreate, e error) {

	createRequest := new(CreateRequest)
	json_dec := json.NewDecoder(req.Body)
	err := json_dec.Decode(&createRequest)
	if err != nil {
		log.Println(err)
		return
	}

	headings := make([]string, len(createRequest.Headers))
	for rowname, rowdata := range createRequest.Headers {
		headings[rowdata.Rownum] = rowname
	}

	sqlstr := fmt.Sprintf("CREATE TABLE `%s` (\n", createRequest.Table)
	sqlstr += "`id` int(11) NOT NULL AUTO_INCREMENT,\n"
	for _, name := range headings {
		sqlstr += genSQLCreateString(createRequest.Headers[name], name)
		sqlstr += ",\n"
	}
	sqlstr += "PRIMARY KEY (id)\n);"
	out = AsyncCreate{
		createRequest.Database,
		createRequest.Table,
		sqlstr,
		createRequest.Payload,
		make(chan error),
	}
	return
}

// NewAsyncInsert allows users of the dbutils lib to recieve an
// AsyncInsert instance with the correct data without having to
// be totally aware of the implementation details.
func NewAsyncInsert(req *http.Request) (insert AsyncInsert, err error) {

	js := json.NewDecoder(req.Body)
	indata := new(InsertData)

	err = js.Decode(&indata)
	if err != nil {
		log.Println(err)
		return insert, err
	}

	insert = AsyncInsert{
		*indata,
		make(chan error),
	}
	return insert, err
}

func NewAsyncDelete(req *http.Request) (del AsyncDelete, err error) {
	js := json.NewDecoder(req.Body)
	indata := new(DeleteData)

	err = js.Decode(&indata)
	if err != nil {
		log.Println(err)
		return del, err
	}

	del = AsyncDelete{
		*indata,
		make(chan error),
	}
	return del, err
}

// Function which takes a row of the incoming json data from a create request
// and tries to extract a CREATE TABLE row instruction out of it.
//
// This function takes a map of strings->interfaces and returns a formatted
// SQL string.
func genSQLCreateString(rowdata Row, rowname string) string {

	rowmap := rowdata.Rowdata

	rowtype := rowmap.Type
	isnull := rowmap.Null
	isunique := rowmap.Unique

	switch rowtype {
	case "VARCHAR":
		return stressql.NewText(rowname, isnull, isunique, rowmap.Len).CreateString()
	case "DATE":
		return stressql.NewDate(rowname, isnull, isunique).CreateString()
	case "TIME":
		return stressql.NewTime(rowname, isnull, isunique).CreateString()
	case "CHOICE":
		return stressql.NewEnum(rowname, rowmap.Choices).CreateString()
	case "CURR":
		return stressql.NewCurrency(rowname, isnull, isunique).CreateString()
	default:
		panic("Unknown field type: " + rowtype)
	}
	panic("Unreachable")
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

// GetMetadata connects to our database which returns a Metadata instance.
// Metadata is an instance which holds JSON data.
func GetMetadata(dbreq *DatabaseRequest) (Metadata, error) {
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
	resp, err := stmt.Run(fmt.Sprintf("%s.%s", dbreq.Database, dbreq.Table))
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

// GetRows takes a database and a table as a string and returns
// a slice of mysql.Rows. This is literally a select * on any
// given table which will handle errors and give us a simpler API
// to work with.
func GetRows(dbreq *DatabaseRequest) ([]mysql.Row, error) {
	db, err := CreateConnection(dbreq.Database)
	if err != nil {
		return nil, err
	}
	defer db.Close()

	sqlStr := fmt.Sprintf("SELECT * FROM %s", dbreq.Table)
	rows, _, err := db.Query(sqlStr)
	if err != nil {
		return nil, err
	}
	return rows, nil
}

// GetHeadings is a function which returns a []string which contains
// the headings names and an error indicating the status of the function
// call.
func GetHeadings(dbreq *DatabaseRequest) ([]string, error) {
	db, err := CreateConnection(dbreq.Database)
	defer db.Close()
	defer db.Query("DROP TABLE temp")
	if err != nil {
		return nil, err
	}
	sqlStr := fmt.Sprintf(`SHOW COLUMNS FROM %s`, dbreq.Table)
	rows, _, err := db.Query(sqlStr)
	if err != nil {
		return nil, err
	}

	headers := []string{}
	for _, row := range rows {
		data, ok := row[0].([]byte)
		if ok {
			headers = append(headers, string(data))
		}
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

// AsyncCreator monitors a channel of type AsyncCreate and blocks until it receives
// on it. Once it has received a job, it will process by calling CreateTable on the
// job and sending it's return value down the ReturnPath associated with the job.
func AsyncCreator(jobqueue chan AsyncCreate) {
	for {
		job := <-jobqueue
		job.ReturnPath <- CreateTable(job)
	}
}

// AsyncInserter monitors a channel of type AsyncCreate and blocks until it receives
// on it. Once it has received a job, it will process by calling InsertRow on the
// job and sending it's return value down the ReturnPath associated with the job.
func AsyncInserter(jobqueue chan AsyncInsert) {
	for {
		job := <-jobqueue
		job.ReturnPath <- InsertRow(job)
	}
}

// AsyncInserter monitors a channel of type AsyncDelete and blocks until it receives
// on it. Once it has received a job, it will process by calling DeleteRows on the
// job and sending it's return value down the ReturnPath associated with the job.
func AsyncDeleter(jobqueue chan AsyncDelete) {
	for {
		job := <-jobqueue
		job.ReturnPath <- DeleteRows(job)
	}
}

// ChangeData is a function which connects to the database and makes an update to a
// table column using the data inside the AsyncUpdate instance. This shouldn't be
// called directly because we have an asynchronous queue which is looking for jobs
// such as this in it's channel.
func ChangeData(job AsyncUpdate) error {
	db, err := CreateConnection(job.Database)
	if err != nil {
		return err
	}
	defer db.Close()

	sqlStr := fmt.Sprintf(
		`UPDATE %s
		 SET`+"`%s`"+`="%s"
		 WHERE id=%s`,
		job.Table, job.Column, job.Data, job.Id,
	)

	_, _, err = db.Query(sqlStr)
	if err != nil {
		return err
	}
	return nil
}

// CreateTable will take an AsyncCreate job and handle the required
// such as connecting to the database and handling specific errors
// when creating a table.
func CreateTable(job AsyncCreate) error {
	db, err := CreateConnection(job.Database)
	metadb, metaerr := CreateConnection(connection_details.SettingsDatabase)
	defer func() {
		db.Close()
		metadb.Close()
	}()

	if err != nil {
		return err
	}
	if metaerr != nil {
		return metaerr
	}

	_, err = db.Start(job.SQLString)

	if err != nil {
		return err
	}

	sqlstmt, err := metadb.Prepare(fmt.Sprintf(
		"INSERT INTO `metadata` VALUES(NULL, \"%s.%s\", ?)",
		job.Database, job.Table,
	))

	if err != nil {
		fmt.Println(err)
		return err
	}

	_, err = sqlstmt.Run(job.Metadata)
	if err != nil {
		return err
	}
	return err
}

// InsertRow takes an instance of type AsyncInsert and takes the required
// steps to insert that data into the database. Handling things like making
// the connection and handling errors along the way.
func InsertRow(job AsyncInsert) error {
	db, err := CreateConnection(job.Database)
	if err != nil {
		return err
	}
	defer db.Close()

	// Subject to change. I don't think this will pass tests
	for idx, val := range job.Data {
		if val == "" {
			job.Data[idx] = "NULL"
		} else {
			job.Data[idx] = `"` + val + `"`
		}
	}
	sqlStr := fmt.Sprintf("INSERT INTO `%s` VALUES(NULL, %s)",
		job.Table, strings.Join(job.Data[1:len(job.Data)], ", "),
	)

	// we're not concerned with the returned data from an insert
	// merely the error status.
	_, _, err = db.Query(sqlStr)
	return err
}

func DeleteRows(job AsyncDelete) error {
	db, err := CreateConnection(job.Database)
	if err != nil {
		return err
	}
	defer db.Close()
	stmt, err := db.Prepare(
		fmt.Sprintf("DELETE FROM %s WHERE `id`=?", job.Table),
	)
	if err != nil {
		return err
	}
	for _, rowid := range job.Data {
		_, err := stmt.Run(rowid)
		if err != nil {
			log.Println(err)
			return nil
		}
	}
	return nil
}

// This function will execute a create table string.
// It connects to whichever database the SettingsDatabase is labelled
// as in the connection_details module but really it doesn't matter
// since it's a CREATE DATABASE query.
func ExecuteCreate(querystr string) error {

	db, err := CreateConnection(connection_details.SettingsDatabase)
	if err != nil {
		return err
	}
	defer db.Close()
	_, _, err = db.Query(querystr)
	return nil
}
