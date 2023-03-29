package parsexml

import (
	"os"
	"server_sim/trace"
	"strconv"
	"strings"
)

func ParseTrainInfo(data string) (string, bool) {
	istart := strings.Index(data, "<TrainID>")

	iend := strings.Index(data, "</TrainID>")
	if istart == -1 || iend == -1 || iend <= istart {
		return "", false
	}
	istart += len("<TrainID>")
	str := data[istart:iend]
	return str, true
}

func ParseACK(data string) (string, string, bool) {
	istart := strings.Index(data, "<Sequence>")
	iend := strings.Index(data, "</Sequence>")
	if istart == -1 || iend == -1 || iend <= istart {
		return "", "", false
	}
	istart += len("<Sequence>")
	sq := data[istart:iend]

	istart = strings.Index(data, "<ErrorCode>")
	iend = strings.Index(data, "</ErrorCode>")
	if istart == -1 || iend == -1 || iend <= istart {
		return sq, "", false
	}
	istart += len("<ErrorCode>")
	er := data[istart:iend]
	return sq, er, true
}

func GenerateACK(seq, errCode int) string {
	fd, err := os.OpenFile("ack.xml", os.O_RDONLY, 0666)
	if err != nil {
		trace.TraceOut("open file failure")
		return ""
	}

	defer fd.Close()
	var buf [256]byte
	length, er := fd.Read(buf[:])
	if er != nil {
		trace.TraceOut("read file failure")
		return ""
	}
	str := string(buf[0:length])
	str = strings.Replace(str, "SREPLACE", strconv.Itoa(seq), 1)
	str = strings.Replace(str, "EREPLACE", strconv.Itoa(errCode), 1)
	return str
}
