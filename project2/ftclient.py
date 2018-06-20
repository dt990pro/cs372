import socket
import sys

def creat_data_sock(DATA_PORT):
    new_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    new_sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    new_sock.bind((socket.gethostname(), int(DATA_PORT)))
    new_sock.listen(3);
    data_sock, addr = new_sock.accept()
    return data_sock

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print "Invalid command.\n"
        sys.exit()

    HOST, PORT = sys.argv[1] + ".engr.oregonstate.edu", int(sys.argv[2])

    # list
    if sys.argv[3] == "-l":
        DATA_PORT = sys.argv[4]

    # get file
    if sys.argv[3] == "-g":
        DATA_PORT = sys.argv[5]

    # Create a socket (SOCK_STREAM means a TCP socket)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect to server
    sock.connect((HOST, PORT))

    # input password
    print "<Input Password>"
    pw = raw_input()
    sock.send(pw)
    received = sock.recv(1024)
    if received == "pwerr":
        print "password err, close client."
        sock.close()
        sys.exit()

    # valid pw then send cmd
    sock.send(sys.argv[3])

    # Receive data from the server and shut down
    received = sock.recv(1024)
    if received == "cmderr":
        print "cmd err, close client."
        sock.close()
        sys.exit()

    # list dir
    if received == "showdataport":
        sock.send(DATA_PORT)

        received = sock.recv(1024)
        if received == "datahostname":
            sock.send(socket.gethostname())

        received = sock.recv(1024)
        if received == "ok":
            # creat data socket
            data_sock = creat_data_sock(DATA_PORT)
            print "\nRecv dir:"
            while 1:
                var = data_sock.recv(1024);
                if var == "--end":
                    print "----------------------------\n"
                    break
                else:
                    print var

            # chdir??
            print "Change dir?? (yes/no-any key)"
            var = raw_input()
            if var == "yes":
                sock.send("chdir")
                print "Input dir:"
                var = raw_input()
                sock.send(var)
            else:
                sock.send("nochdir")

            data_sock.close()

    # get file
    elif received == "filename":
        sock.send(sys.argv[4])
        received = sock.recv(1024)

        # filename valid
        if received == "fnameinvalid":
            print "file not exists."
            sys.exit()

        # send portnum
        if received == "showdataport":
            sock.send(DATA_PORT)

        # send hostname
        received = sock.recv(1024)
        if received == "datahostname":
            sock.send(socket.gethostname())

        # prepare creat data connection
        received = sock.recv(1024)
        if received == "ok":
            # creat data socket
            data_sock = creat_data_sock(DATA_PORT)

            # write file
            file = open(sys.argv[4], "w")

            print "\nRecv file"
            while 1:
                var = data_sock.recv(1024)
                print "receiving!!"
                if var == "\\quit":
                    break
                file.write(var)
            data_sock.close()

            print "file transfer complete\n----------------------------\n"

    sock.close()
    sys.exit()