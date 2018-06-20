have 2 server opened.

start server (e.g. "python chatserver.py localhost 9900")

compile .c on another server (e.g. "gcc chatclient.c")

start client (e.g. "a.out localhost 9900")

now client prompts "Your 'handle': ", input it.

then input your message.

server side would prompt the massage with client handle. (e.g. Heinz: hi)

then server responses, in client side it would say "Host A: brabrabra"

if server responses "\quit", then client have to run command again. server will be always listening.

if client reponses "\quit", then client have to run command again. server will be always listening.

otherwise, run "Ctrl+C" to terminate server and client.