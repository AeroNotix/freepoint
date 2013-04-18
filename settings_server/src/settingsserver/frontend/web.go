package frontend

import (
	"connection_details"
	"fmt"
	"io"
	"io/ioutil"
	"mime"
	"net/http"
	"os"
	"path/filepath"
	bk "settingsserver/backend"
	tmpl "settingsserver/templates"
)

func init() {
	tmpl.ParseDirectory(connection_details.TemplateDirectory, true)
}
func Index(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	return tmpl.Must(
		filepath.Join(connection_details.TemplateDirectory, "index.html"),
	).Execute(w, map[string]string{"STATIC_URL": connection_details.StaticURL})
}

func DirectToTemplate(templatename string) bk.RouterHandler {
	filename := filepath.Join(connection_details.TemplateDirectory, templatename)
	if !exists(filename) {
		panic(fmt.Sprintf("Template does not exist: %s", filename))
	}
	return func(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
		return tmpl.Must(filename).Execute(w, nil)
	}
}

func CreateUser(self *bk.AppServer, w http.ResponseWriter, req *http.Request) error {
	fmt.Println(bk.CreateUser("aero", "something"))
	io.WriteString(w, "done!")
	return nil
}

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

func exists(filename string) bool {
	if _, err := os.Stat(filename); err == nil {
		return true
	}
	return false
}
