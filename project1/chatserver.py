# retrieved from 
# https://docs.python.org/release/2.6.5/library/socketserver.html#socketserver-tcpserver-example

import SocketServer
import sys

class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # default, always open
        while True:
            
            # self.request is the TCP socket connected to the client
            self.data = self.request.recv(1024).strip()

            # if client requests quit
            if "\\quit" in self.data:
                print "Host B (%s) quit\n" % self.client_address[0]
                break
            else:
                print self.data + "\n"
                # just send back data
                print "Response: "
                var = raw_input();
                while (len(var) > 500 or len(var) < 1 ):
                    print "1 - 500 chars, input again: "
                    var = raw_input();
                if var == "\\quit":
                    print "you quit.\n"
                    self.request.send(var)
                    break
                self.request.send("Host A: " + var)
                print "++++++++++++++++++++++++++++++++\n"


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Invalid command. (python .py hostname portnum)\n"
        sys.exit()

    HOST, PORT = sys.argv[1], int(sys.argv[2])
    
    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()