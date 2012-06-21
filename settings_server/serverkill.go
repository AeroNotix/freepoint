package main

import (
	"net/http"
	"fmt"
)

func main() {
	ch := make(chan int, 10)
	for x := 0; x < 50000; x++ {
		fmt.Println(x)
		go func() {
			http.Get("http://localhost:12345/params/")
			ch <- 1
		}()
	}
	for x := 0; x < 50000; x++ {
		fmt.Println(x)
		<- ch
	}
}