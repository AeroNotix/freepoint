package frontend

import (
<<<<<<< HEAD
	"connection_details"
	"fmt"
	"html/template"
	"io"
	"io/ioutil"
	"mime"
	"net/http"
	"os"
	"path/filepath"
	bk "settingsserver/backend"
)

func Must(filename string) *template.Template {
	file, err := template.ParseFiles(filename)
	if err != nil {
		panic(err)
	}
	return file
}

func Index(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	return Must(filepath.Join(connection_details.TemplateDirectory, "index.html")).Execute(w, nil)
=======
	"fmt"
	"io"
	"net/http"
	bk "settingsserver/backend"
)

func Index(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	io.WriteString(w, "Hello")
	return nil
>>>>>>> 3a916710f2a4ec1190fc0036c7be7bcbdfa2902a
}

func CreateUser(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	fmt.Println(bk.CreateUser("aero", "something"))
	io.WriteString(w, "done!")
	return nil
}
<<<<<<< HEAD

func StaticFiles(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	reqstr := req.URL.Path[len(connection_details.StaticURL):]
	ctype := mime.TypeByExtension(filepath.Ext(reqstr))
	fpath := filepath.Join(connection_details.StaticDirectory, reqstr)
	f, err := os.Open(fpath)
	if err != nil {
		w.WriteHeader(http.StatusNotFound)
		return err
	}
	b, err := ioutil.ReadAll(f)
	if err != nil {
		return err
	}
	w.Header().Set("Content-Type", ctype)
	io.WriteString(w, string(b))
	return nil
}
=======
>>>>>>> 3a916710f2a4ec1190fc0036c7be7bcbdfa2902a
