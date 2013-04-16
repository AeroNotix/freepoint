package settingsserver

import (
	"bytes"
	"fmt"
	mrand "math/rand"
	"strings"
	"time"
)

func randomBytes(howmuch int) []byte {
	b := new(bytes.Buffer)
	rng := mrand.New(mrand.NewSource(time.Now().Unix()))
	for x := 0; x < howmuch; x++ {
		/* ASCII range */
		n := rng.Intn(26) + 65
		b.Write([]byte(string(rune(n))))
	}
	return b.Bytes()
}

type Session interface {
	Encode() []byte
	Add(key string, values []string)
	Remove(key string)
	Delete()
	Save() error
}

type MySQLSession struct {
	Key       []byte
	RawValues []byte
	Values    map[string]string
}

func CreateMySQLSession() (*MySQLSession, error) {
	randombytes := randomBytes(128)
	db, err := CreateConnection("db_freepoint")
	if err != nil {
		return nil, err
	}
	defer db.Close()
	stmt, err := db.Prepare("INSERT INTO sessions VALUES (?, ?)")
	if err != nil {
		return nil, err
	}
	_, err = stmt.Run(randombytes, "")
	if err != nil {
		return nil, err
	}
	return &MySQLSession{
		Key:    randombytes,
		Values: make(map[string]string),
	}, nil
}

func OpenMySQLSession(key []byte) (*MySQLSession, error) {
	db, err := CreateConnection("db_freepoint")
	if err != nil {
		return nil, err
	}
	defer db.Close()
	stmt, err := db.Prepare("SELECT sessionvalue FROM sessions WHERE sessionkey=(?)")
	fmt.Println(string(key))
	if err != nil {
		return nil, err
	}
	result, err := stmt.Run(string(key))
	if err != nil {
		return nil, err
	}
	row, err := result.GetRow()
	if err != nil {
		return nil, err
	}
	m, err := ParseValues(row[0].([]uint8))
	if err != nil {
		return nil, err
	}
	session := &MySQLSession{
		Key:       key,
		RawValues: row[0].([]uint8),
		Values:    m,
	}
	return session, nil
}

func (m *MySQLSession) Add(key string, value string) {
	m.Values[key] = value
	m.Save()
}

func (m *MySQLSession) Remove(key string) {
	delete(m.Values, key)
	m.Save()
}

func (m *MySQLSession) Save() error {
	db, err := CreateConnection("db_freepoint")
	if err != nil {
		return err
	}
	defer db.Close()
	b := bytes.NewBufferString("")
	for key, value := range m.Values {
		b.WriteString(
			fmt.Sprintf(
				"%s:=%s;",
				key, value,
			),
		)
	}
	stmt, err := db.Prepare("UPDATE sessions SET sessionvalue=(?) WHERE sessionkey=(?)")
	if err != nil {
		return err
	}
	_, err = stmt.Run(b.String(), m.Key)
	if err != nil {
		return err
	}
	return nil
}

func ParseValues(raw []byte) (map[string]string, error) {
	m := make(map[string]string)
	if len(raw) == 0 {
		return m, nil
	}
	keyvals := strings.Split(string(raw), ";")
	for _, entry := range keyvals {
		split := strings.Split(entry, ":=")
		if len(split) == 2 {
			m[split[0]] = split[1]
		}
	}
	fmt.Println(m)
	return m, nil
}
