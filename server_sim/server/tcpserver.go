package server

import (
	"net"
	"os"
	"regexp"
	"server_sim/parsexml"
	"server_sim/rspack"
	"server_sim/trace"
	"time"
)

type Handler interface {
	Handle(d string, sq int, typeinfo string)
}

type LinkTrain struct {
	State  int
	TrainN string
	Sq     int

	Con net.Conn
}

func (obj *LinkTrain) Handle(d string, sq int, typeinfo string) {

	switch typeinfo {
	case "INF":
		var ackMsg string
		tn, ok := parsexml.ParseTrainInfo(d)
		if ok {
			obj.State = 1
			obj.TrainN = tn
			trace.TraceOut("train num get success: %s", tn)
			ackMsg = parsexml.GenerateACK(sq, 0)
		} else {
			trace.TraceOut("train num get failed")
			ackMsg = parsexml.GenerateACK(sq, 8)
		}
		obj.Con.Write(rspack.PackMsg(0xff, sq, []byte("ACK"), ackMsg))
	case "ACK":
		sq, ercode, ok := parsexml.ParseACK(d)
		trace.TraceOut("Get ACK : sq %s, errorcode %s", sq, ercode)
		if !ok {
			trace.TraceOut("ACK parse failed")
		}
	default:
		trace.TraceOut("get unknow type : %s, NACK", typeinfo)
		ackMsg := parsexml.GenerateACK(sq, 8)
		obj.Con.Write(rspack.PackMsg(0xff, sq, []byte("ACK"), ackMsg))
	}
}

var (
	trainMap map[net.Conn]*LinkTrain

	localAddr string
	listener  net.Listener
	err       error

	blistenning bool
	numSend     int
)

func init() {
	blistenning = false
	numSend = 0
	trainMap = make(map[net.Conn]*LinkTrain)
}

func StartServer(addr string) {
	if blistenning {
		trace.TraceOut("Server is having started as %s")
		return
	}
	blistenning = true
	listener, err = net.Listen("tcp", addr)
	if err != nil {
		trace.TraceOut("Local：%s   Listening error: %s", addr, err.Error())
		blistenning = false
		return
	}
	localAddr = addr

	go func() {
		for {
			c, err := listener.Accept()
			if err == nil {
				//connectors = append(connectors, c)
				trainMap[c] = &LinkTrain{0, "", 0, c}
				go receiveData(c)
			} else {
				trace.TraceOut("Local：%s   Accept error: %s", addr, err.Error())
				return
			}
		}
	}()
}
func SendMsg(data []byte, trainNum string) {
	for _, x := range trainMap {
		if x.TrainN == trainNum {
			if x.State == 1 {
				x.Con.Write(rspack.PackMsg(0xff, x.Sq, []byte("TXT"), string(data)))
				x.Sq++
				return
			}
			break
		}
	}
	trace.TraceOut("Link is not ready to send!!!!")
}

func StopServer() {
	listener.Close()
	for _, x := range trainMap {
		x.Con.Close()
	}
	time.Sleep(50 * time.Microsecond)
}

func receiveData(con net.Conn) {
	var buf [1024]byte
	bufLen := 0
	numSend++
	for {
		length, e := con.Read(buf[bufLen:])
		if e != nil {
			trace.TraceOut("Read error：%s", e.Error())
			delete(trainMap, con)
			numSend--
			return
		}
		bufLen += length
		//data handling
		if bufLen >= 31 {
			packt, xmlt, ok := rspack.Unpack(0xff, buf[0:bufLen])
			if ok {
				copy(buf[:], buf[packt.PackSize:bufLen])
				bufLen -= int(packt.PackSize)
				println(xmlt)
				trace.TraceOut("unpack msg: pack size:%d, packType:%s, msgLen %d, sq %d leftdata %d", packt.PackSize, packt.MsgType, packt.MsgSize, packt.SqNum, bufLen)
				trainMap[con].Handle(xmlt, int(packt.SqNum), packt.MsgType)
			} else {
				trace.TraceOut("unpack failed ")
			}
		}
	}
}
func ShowLinks() {
	println("-----------Link information start line----------------")
	for _, x := range trainMap {
		println(x.Con.RemoteAddr().String(), "  state=", x.State, "   Train NO=", x.TrainN)
	}
}
func GetServerAddr() string {
	fd, err := os.OpenFile("profile.ini", os.O_RDONLY, 0666)
	if err != nil {
		trace.TraceOut("Open config file failure")
		return ""
	}
	defer fd.Close()
	var buf [512]byte
	length, e := fd.Read(buf[:])
	if e != nil {
		trace.TraceOut("Read config file failure")
		return ""
	}

	data_sver := regexp.MustCompile(`ServerAddr="(.*?)"`)
	strs := data_sver.FindStringSubmatch(string(buf[0:length]))
	if 2 > len(strs) {
		trace.TraceOut("match  failure")
		return ""
	}
	return strs[1]
}
