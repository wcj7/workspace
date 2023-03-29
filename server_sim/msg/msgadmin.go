package msg

import (
	"os"
	"path/filepath"
	"server_sim/trace"
	"strings"
)

var fileMap []string

func GetMsg(index int) string {
	var buf [1024]byte
	if len(fileMap) <= index {
		trace.TraceOut("No file has index as %d", index)
		return ""
	}
	trace.TraceOut("Send msg in file:%s", fileMap[index])
	fd, err := os.OpenFile(fileMap[index], os.O_RDONLY, 0666)
	if err != nil {
		trace.TraceOut("open file failure")
		return ""
	}
	defer fd.Close()
	length, err := fd.Read(buf[:])
	if err != nil {
		trace.TraceOut(" file read err")
		return ""
	}
	return string(buf[:length])
}

func UpdateMsgs(bshow bool) {
	fileMap = make([]string, 0)
	filepath.Walk("config/", func(path string, info os.FileInfo, err error) error {
		if strings.HasSuffix(path, "xml") {
			fileMap = append(fileMap, path)
		}
		return nil
	})
	if !bshow {
		return
	}
	for i, x := range fileMap {
		println("index:", i, "  file", x)
	}
}
