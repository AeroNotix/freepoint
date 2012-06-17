package main

import (
	"fmt"
	"github.com/hoisie/web"
	mysql "github.com/ziutek/mymysql/mysql"
	_ "github.com/ziutek/mymysql/native"
)

func databaseParameters(ctx *web.Context, dbname string) string {

	db := mysql.New(
		"tcp",
		"",
		"127.0.0.1:3306",
		"aero",
		"--------",
		"db_freepoint",
	)
	err := db.Connect()
	if err != nil {
		panic(err)
	}

	// need to find the parameterization syntax
	sqlStr := "SELECT * FROM tbl_metadata WHERE `tablename`=\"" + dbname + "\""
	fmt.Println(sqlStr)
	rows, res, err := db.Query(sqlStr)

	if err != nil {
		panic(err)
	}

	if len(rows) != 1 {
		return dbname
	}

	for _, row := range rows {
		return (row.Str(res.Map("metadata")))
	}

	// the Go compiler doesn't detect that we will certainly have returned by
	// this point and throws an error so we have to return here
	return "error"
}

func main() {
	web.Get("/params/(.*)", databaseParameters)
	web.Run("0.0.0.0:12345")
}
