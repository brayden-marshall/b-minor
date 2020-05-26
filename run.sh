make
if [[ $? == 0 ]]; then
    echo ""
    ./bin/b-minor $1
    if [[ $? != 0 ]]; then
        exit 1
    fi

    if [[ -f "./output.s" ]]; then
        gcc -ggdb -o output output.s
    fi
fi
