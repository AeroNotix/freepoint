package backend

import (
	"crypto/sha512"
	"fmt"
	"io"
)

func CreateUser(email, password string) error {
	db, err := CreateConnection("db_freepoint")
	h := sha512.New()
	io.WriteString(h, email)
	io.WriteString(h, password)
	hashedpassword := fmt.Sprintf("%x", h.Sum(nil))
	stmt, err := db.Prepare("INSERT INTO tblusers VALUES (NULL, ?, ?)")
	if err != nil {
		return err
	}
	_, err = stmt.Run(email, hashedpassword)
	return err
}
