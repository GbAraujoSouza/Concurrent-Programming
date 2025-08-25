package main

import (
	"fmt"
	"os"
	"strconv"
	"sync"
)

const NUM_THREADS int = 4

func main() {
	if (len(os.Args) < 2) {
		fmt.Println("Usage: ./incrementArray <array_size>")
		os.Exit(1)
	}
	
	arraySize, _ := strconv.Atoi(os.Args[1])
	if arraySize % NUM_THREADS != 0 {
		fmt.Printf("ERROR: Array size should be a multiple of %d\n", NUM_THREADS)
		os.Exit(2)
	} 

	mainArray := make([]int, arraySize)

	var wg sync.WaitGroup
	wg.Add(NUM_THREADS)

	indexCounter := -1
	for range(NUM_THREADS) {
		go func() {
			indexCounter++
			IncrementVector(&mainArray, indexCounter)
			wg.Done()
		}()
	}
	wg.Wait()

	fmt.Println(mainArray)
}

func IncrementVector(vector *[]int, index int) {
	numPos := len(*vector) / NUM_THREADS
	startPos := index * numPos
	// fmt.Println(index)

	for i := startPos; i < startPos+numPos; i++ {
		(*vector)[i] += 2
	}
}
