make
if [[ $? == 0 ]]; then
    echo ""
    ./bin/b-minor $1
    if [[ -f "./output.s" ]]; then
        gcc -ggdb -o output output.s
    fi
fi
