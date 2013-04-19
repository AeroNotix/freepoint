package sqlsettingsserver

import (
	"log"
	"settingsserver"
)

var (
	MetadataTable = `CREATE TABLE metadata (
					 id int(11) NOT NULL AUTO_INCREMENT,
					 tablename varchar(255) NOT NULL,
					 metadata text NOT NULL,
					 PRIMARY KEY (id),
					 UNIQUE KEY tablename (tablename)
					 )
					 `

	UserTable = `CREATE TABLE tblusers (
				 id int(11) NOT NULL AUTO_INCREMENT,
				 userid varchar(255) NOT NULL,
				 password varchar(255) NOT NULL,
				 PRIMARY KEY (id),
				 UNIQUE KEY userid (userid)
				 )
				 `

	SessionsTable = `CREATE TABLE sessions (
                     sessionkey text,
                     sessionvalue text,
                     expiry datetime DEFAULT NULL
                     )`
)

func init() {
	for _, SQLStr := range []string{MetadataTable, UserTable, SessionsTable} {
		err = settingsserver.ExecuteCreate(SQLStr)
		if err != nil {
			log.Println(err)
		}
	}
}
