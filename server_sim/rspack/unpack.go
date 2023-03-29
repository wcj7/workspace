package rspack

import (
	"encoding/binary"
	"server_sim/trace"
)

type Packagehead struct {
	Head     uint32
	PackSize uint32
	SqNum    uint16
	MsgSize  uint32
	Time     string
	MsgType  string
}

func Unpack(headMark uint32, data []byte) (*Packagehead, string, bool) {
	var (
		headinfo Packagehead
		msg      string
		ok       bool = false
	)
	/*headinfo.head = *(*uint32)(unsafe.Pointer(&data[0]))
	headinfo.packSize = *(*uint32)(unsafe.Pointer(&data[4]))
	headinfo.sqNum = *(*uint16)(unsafe.Pointer(&data[8]))
	headinfo.msgSize = *(*uint32)(unsafe.Pointer(&data[10]))*/
	headinfo.Head = binary.BigEndian.Uint32(data[0:4])
	headinfo.PackSize = binary.BigEndian.Uint32(data[4:8])
	headinfo.SqNum = binary.BigEndian.Uint16(data[8:10])
	headinfo.MsgSize = binary.BigEndian.Uint32(data[10:14])

	headinfo.Time = string(data[14:28])
	headinfo.MsgType = string(data[28:31])
	ok = (headinfo.PackSize <= uint32(len(data)) && headMark == headinfo.Head)
	if ok {
		msg = string(data[31:headinfo.PackSize])
	} else {
		trace.TraceOut("Error, parse package head failure")
	}
	return &headinfo, msg, ok
}
