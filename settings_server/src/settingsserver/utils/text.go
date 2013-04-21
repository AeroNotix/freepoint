package utils

import (
	"strings"
)

func EscapeQuotes(s string) string {
	r := strings.NewReplacer(`"`, `\"`, `'`, `\'`)
	return r.Replace(s)
}
