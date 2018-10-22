#!/Users/guoweizhe/Documents/ECE-357/myshell/myshell/myshell/myshell

# another example, say it is called test2.sh
#you invoked it with
#./test2.sh <input.txt
cat >cat2.out
#since you invoked the shell script (via the system shell such as bash)
#with stdin redirected, your shell runs cat which gets stdin from input.txt
exit
#the above exit had no specified return value, so your shell exited with 0
#because the last child spawned, cat, would have returned 0
#again, test this with echo $?
