reference:
https://docs.python.org/release/2.6.5/library/socketserver.html#socketserver-tcpserver-example
http://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html

***the aasignment implement two extra credits
***one is password
***one is change dir on server

open two putty for example.
1 for ftserver.c and 2 for ftclient.py

check 1 and 2 hostname
if both are the same change one of them.

for example 1 on flip2, 2 on flip1.

--
access file "server"
run "cat long2.txt"
notice that the last line is 123

run "gcc ftserver.c"
run "a.out 9900"
the server will be listening

--
1	switch to 2
run "cat long2.txt"
notice that the last line is 1234567890

2	run "python ftclient.py flip2 9900 -x 9991"
intput password "silversun" (should prompt cmd err and exit)

3	run "python ftclient.py flip2 9900 -l 9991"
intput password "111" (should prompt password err and exit)

4	run "python ftclient.py flip2 9900 -l 9991"
intput password "silversun" (should prompt 4 files and 1 dir, then ask change dir)
say "no" to exit

5	run "python ftclient.py flip2 9900 -g long2.txt 9991"
input pw
run "cat long2.txt" (notice that the last line turns to 123)

6	run "python ftclient.py flip2 9900 -l 9991"
pw
say "yes" to change dir
say "./x" to go to new dir (server give new dir)

7	run "python ftclient.py flip2 9900 -l 9991"
pw
prompt 1 file, which is "long1.txt"
no change dir

8	run "python ftclient.py flip2 9900 -g 11111111111.txt 9991"
input pw (prompt file not exists and exit)

9	run "python ftclient.py flip2 9900 -g long1.txt 9991"
input pw
run "cat long1.txt" (get lots of a, =, m)

10	run "python ftclient.py flip2 9900 -l 9991"
pw
say "yes" to change dir
say ".." to go back

11	run "python ftclient.py flip2 9900 -g sho.txt 9991"
input pw
run "cat sho.txt" (few chars)

12	run "ls" (got 3 files, long1 long2 and sho)

13	terminates server.