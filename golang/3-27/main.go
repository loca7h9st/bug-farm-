package main

import (
	"fmt"
)

func main() {
	// a := "明天出发"
	// c := a
	// fmt.Printf("%p,%v, %p, %v\n", &c, c, &a, a)
	// fmt.Println("\xe6\x98\x8e\xe5\xa4\xa9\xe5\x87\xba\xe5\x8f\x91")
	// for i, v := range a {
	// 	fmt.Println(i, v)
	// 	fmt.Printf("%T, %[1]v %[1]c\n", v)

	// }
	// src := rand.NewSource(time.Now().UnixNano())
	// gen := rand.New(src)
	// for i := 0; i < 10; i++ {
	// 	fmt.Println(gen.Intn(10))
	// }
	for i := 1; i < 10; i++ {
		for j := 1; j <= i; j++ {
			fmt.Printf("%2d * %d = %2d", j, i, i*j)
		}
		fmt.Println()
	}

}
