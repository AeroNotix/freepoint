package templates

import (
	"html/template"
	"log"
	"os"
	"path/filepath"
)

func Must(filename string) *template.Template {
	file, err := template.ParseFiles(filename)
	if err != nil {
		panic(err)
	}
	return file
}

func ParseDirectory(directory string, recursive bool) []*template.Template {
	return ParseFiles(buildFilepaths(directory, recursive))
}

func ParseFiles(filenames []string) []*template.Template {
	out := make([]*template.Template, len(filenames))
	for _, filename := range filenames {
		if template, err := New(filename); err == nil {
			out = append(out, template)
		} else {
			log.Println(err)
		}
	}
	return out
}

func New(name string) (*template.Template, error) {
	realname := filepath.Base(name)[:len(filepath.Base(name))-len(filepath.Ext(name))]
	t := template.New(realname)
	return t.Parse(name)
}

// Recursively builds a slice of filepaths underneath a base directory.
func buildFilepaths(base string, recursive bool) []string {

	files, err := readDir(base)
	if err != nil {
		return []string{}
	}

	out := make([]string, 0, len(files))

	for _, file := range files {
		if file.IsDir() {
			if recursive {
				out = append(out, buildFilepaths(filepath.Join(base, file.Name()), recursive)...)
			}
		} else {
			out = append(out, filepath.Join(base, file.Name()))
		}
	}
	return out
}

// Opens a directory and returns a []os.FileInfo or an error
func readDir(dirname string) ([]os.FileInfo, error) {
	f, err := os.Open(dirname)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	list, err := f.Readdir(-1)
	if err != nil {
		return nil, err
	}
	return list, nil
}
