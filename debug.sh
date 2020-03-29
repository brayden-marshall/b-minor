make
if [[ $? == 0 ]]; then
    gdb --args ./bin/b-minor $1
fi
