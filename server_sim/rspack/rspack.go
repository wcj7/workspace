package rspack

import (
	"encoding/binary"
	"strings"
	"time"
)

// packhead 4(packethead)+4(packetsize)+2(sqnum)+4(mssage size)+14(time)+3(type) + length of msg
func PackMsg(headMark uint32, sq int, mtype []byte, msg string) []byte {
	msgSize := len(msg)
	var pr = make([]byte, 4+4+2+4+14+3+msgSize)

	/**(*uint32)(unsafe.Pointer(&pr[0])) = headMark //head
	*(*uint32)(unsafe.Pointer(&pr[4])) = uint32(msgSize + 31)
	*(*uint16)(unsafe.Pointer(&pr[8])) = uint16(sq)
	*(*uint32)(unsafe.Pointer(&pr[10])) = uint32(msgSize)*/

	binary.BigEndian.PutUint32(pr[0:4], headMark)
	binary.BigEndian.PutUint32(pr[4:8], uint32(msgSize+31))
	binary.BigEndian.PutUint16(pr[8:10], uint16(sq))
	binary.BigEndian.PutUint32(pr[10:14], uint32(msgSize))

	tm := time.Now().String()
	tm = strings.Split(tm, ".")[0]
	tm = strings.Replace(tm, "-", "", 3)
	tm = strings.Replace(tm, " ", "", 3)
	tm = strings.Replace(tm, ":", "", 3)

	copy(pr[14:28], tm[:])
	copy(pr[28:31], mtype[:])
	copy(pr[31:], msg[:])
	return pr
}
