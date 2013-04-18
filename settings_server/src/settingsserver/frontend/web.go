package frontend

import (
	"fmt"
	"io"
	"net/http"
	bk "settingsserver/backend"
)

func Index(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	io.WriteString(w, "Hello")
	return nil
}

func CreateUser(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	fmt.Println(bk.CreateUser("aero", "something"))
	io.WriteString(w, "done!")
	return nil
}
