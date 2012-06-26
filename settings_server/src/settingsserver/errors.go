package settingsserver

type AppError struct {
	errorString string
	errorCode   int
}

func (e AppError) Error() string {
	return e.errorString
}

func (e AppError) Code() int {
	return e.errorCode
}

var (
	LoginError   = AppError{"Login Failure", 1}
	RequestError = AppError{"Incorrect request mode", 2}
	InvalidTable = AppError{"Table not found", 3}
)
