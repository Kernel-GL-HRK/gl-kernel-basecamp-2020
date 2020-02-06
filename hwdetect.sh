#!/bin/bash
choose=15
while [ $choose -gt 0 ]
do
    echo ""
    echo "Enter your choose"
    echo "***************************************************"
    echo "** 1) Output connected devices                   **"
    echo "** 2) Output information about connected devices **"
    echo "** 3) Exit                                       **"
    echo "***************************************************"
    read choose
    case $choose in
        1)
        echo ""
        echo /dev/sd*
        echo ""
        ;;
        2)
        df -h
        ;;
        3)
        choose=0
        ;;
    esac
done
