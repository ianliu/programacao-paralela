package main

import (
	"flag"
	"fmt"
	"io"
	"math"
	"os"
	"syscall"
)

var SHOW_HELP = flag.Bool("h", false, "Show help")
var DATA_LEN  = flag.Int("n", 0, "Number of samples")
var BUCK_LEN  = flag.Int("b", 0, "Number of buckets")
var THRE_LEN  = flag.Int("t", 1, "Number of threads")
var INPUT_FL  = flag.String("i", "", "Input file")

var ns, bl, th int
var in string

func usage(prog string) {
	fmt.Fprintf(os.Stderr, "Usage:\n")
	fmt.Fprintf(os.Stderr, "  %s -h\n", prog)
	fmt.Fprintf(os.Stderr, "  %s -n DATA_SIZE -b BUCKET_SIZE [-t NUM_THREADS]\n", prog)
	os.Exit(1)
}

func parseOpts() {
	flag.Parse()

	ns = *DATA_LEN
	bl = *BUCK_LEN
	th = *THRE_LEN
	in = *INPUT_FL

	if *SHOW_HELP || ns < 1 || bl < 1 || th < 1 {
		usage(os.Args[0]);
	}
}

func tick() uint64 {
	var tv syscall.Timeval
	err := syscall.Gettimeofday(&tv)
	if err != nil {
		return 0
	}
	return uint64(tv.Sec)
}

func utick() uint64 {
	var tv syscall.Timeval
	err := syscall.Gettimeofday(&tv)
	if err != nil {
		return 0
	}
	return uint64(tv.Usec)
}

func getData() ([]float64, float64, float64) {
	var fp io.Reader

	if len(in) > 0 {
		file, err := os.Open(in)
		if err != nil {
			fmt.Fprintf(os.Stderr, err.Error())
			return []float64{}, 0, 0
		}
		fp = file
	} else {
		fp = os.Stdin
	}

	t0 := tick()
	fmt.Print("Start reading file...")
	data := make([]float64, ns)
	fmt.Fscanf(fp, "%f", &data[0])
	min := data[0]
	max := data[0]

	for i := 1; i < ns; i++ {
		fmt.Scanf("%f", &data[i])
		if data[i] > max {
			max = data[i]
		} else if data[i] < min {
			min = data[i]
		}
	}
	fmt.Println("Done.", tick() - t0)

	return data, min, max
}

func worker(start, end int, data []float64, min, max float64, bchan chan []int) {
	bucket := make([]int, bl)
	for i := start; i < end; i++ {
		r := float64(bl) * (data[i] - min) / (max - min)
		b := int(math.Floor(r))
		if b == bl {
			b--
		}
		if b >= 0 && b < bl {
			bucket[b]++
		}
	}

	bchan <- bucket
}

func main() {
	parseOpts()

	data, min, max := getData()

	if len(data) < 1 {
		os.Exit(1)
	}

	r := ns % th
	k := ns / th
	start := 0
	end := 0

	next := func() {
		start = end
		end += k
		if r > 0 {
			end++
			r--
		}
	}

	bucket := make([]int, bl)
	bchan := make(chan []int)

	t0 := utick()
	for i := 0; i < th; i++ {
		next()
		go worker(start, end, data, min, max, bchan)
	}

	for i := 0; i < th; i++ {
		b := <-bchan
		fmt.Println("Received", b, "from Goroutine!")
		for j := 0; j < bl; j++ {
			bucket[j] += b[j]
		}
	}
	fmt.Println("Result", bucket, utick() - t0)
}
