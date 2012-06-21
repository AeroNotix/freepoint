package main

import (
	"net/http"
	"fmt"
)

func main() {
	ch := make(chan int, 10)
	for _, x := range [...]int{1, 2, 3, 4, 5, 6, 7, 8, 9, 10} {
		fmt.Println(x)
		go func() {
			http.Get("http://localhost:12345/params/")
			ch <- 1
		}()
	}
	for _, x := range [...]int{1, 2, 3, 4, 5, 6, 7, 8, 9, 10} {
		fmt.Println(x)
		<- ch
	}
}