package trace

import (
	"log"
	"os"
	"runtime"
	"strconv"
)

type Trace struct {
	logers      []*log.Logger
	isInitiated bool
}

var t Trace

func init() {
	t.isInitiated = false
}
func TraceOut(para string, other ...any) {
	_, file, line, ok := runtime.Caller(1)
	if ok {
		println(file, line)
		para = file + ":" + strconv.Itoa(line) + ": " + para
	} else {
		para = "??? " + para
	}

	for _, x := range t.logers {
		x.Printf(para, other...)
	}
}

func StartTrace(flag uint32, filename string) {
	if t.isInitiated {
		return
	}

	t.isInitiated = true
	if flag&1 == 1 {
		out := log.New(os.Stdout, "", log.Ldate|log.Ltime) //|log.Lshortfile
		t.logers = append(t.logers, out)
	}

	if flag&2 == 2 {
		fh, err := os.OpenFile(filename, os.O_CREATE|os.O_RDWR|os.O_APPEND, 0666)
		if err == nil {
			out := log.New(fh, "", log.Ldate|log.Ltime)
			t.logers = append(t.logers, out)
		}
	}
}
