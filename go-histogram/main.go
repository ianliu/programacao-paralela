package main

import (
	"fmt"
	"flag"
)

var SHOW_HELP = flag.Bool("h", false, "Show help")
var DATA_LEN  = flag.Int("n", 0, "Number of samples")
var BUCK_LEN  = flag.Int("b", 0, "Number of buckets")
var THRE_LEN  = flag.Int("t", 0, "Number of threads")

func main() {
	flag.Parse()

	if h {
		usage()
	}
	if DATA_LEN <= 0 || BUCKET_LEN <= 0 || NUM_THREADS <= 0 {
		usage(argv[0]);
	}
}
