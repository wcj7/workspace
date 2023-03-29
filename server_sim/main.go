package main

import (
	"os"
	"runtime"
	"server_sim/msg"
	"server_sim/server"
	"server_sim/trace"
	"strconv"
	"strings"
)

func init() {
	trace.StartTrace(3, "my.log")
}

func main() {
	var lenEnd int
	if runtime.GOOS == "linux" {
		lenEnd = 1
	} else { //windows
		lenEnd = 2
	}
	var buf [128]byte
	trace.TraceOut("App gets startup ...at %s,  lenght of end：%d", runtime.GOOS, lenEnd)
	server.StartServer(server.GetServerAddr())
	defer server.StopServer()
	trace.TraceOut("Server is on: %s", server.GetServerAddr())
	for {
		length, err := os.Stdin.Read(buf[:])
		if err != nil {
			println("Error:", err.Error(), length)
		} else {
			trace.TraceOut(">>>>", string(buf[0:length-lenEnd]), length)
			if length > lenEnd {
				sec := strings.Split(string(buf[0:length-lenEnd]), " ")
				switch sec[0] {
				case "exit":
					goto END
				case "send":
					if len(sec) != 3 {
						trace.TraceOut("Get wrong sending pattern: %s ", string(buf[0:length-lenEnd]))
					} else {
						index, err := strconv.Atoi(sec[2])
						if err == nil {
							server.SendMsg([]byte(msg.GetMsg(index)), sec[1])
						} else {
							trace.TraceOut("send failed")
						}

					}

				case "list":
					println("-------------------listening port 9999----------------------")
					server.ShowLinks()
					println("-------------------------cmd----------------------------------")
					println("exit: terminate app")
					println("send: send msg to client in form [send trainNO msgfile]")
					println("list: show information and update msgfile list")
					println("-------------------------msg files---------------------------")
					msg.UpdateMsgs(true)
				default:
					//send to client
				}

			}

		}
	}
END:
	trace.TraceOut("go to end ")
}
