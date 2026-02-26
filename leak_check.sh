#!/bin/bash

# Check if at least two arguments are provided (the program and the file)
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 ./program [args...]"
    exit 1
fi

LOG_FILE="valgrind_log.txt"
PROGRAM=$1
shift

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=$LOG_FILE \
         "$PROGRAM" "$@"

# After the program exits, show the summary
echo ""
echo "------------------------------------------------"
echo -e "|             \e[34m Valgrind finished \e[0m              |"
echo "------------------------------------------------"
echo ""

# Check if there were 'definitely lost' bytes
LEAKS=$(grep "definitely lost:" $LOG_FILE)

if [[ $LEAKS == *" 0 bytes in 0 blocks"* ]]; then
    echo -e "\e[32m------------------------------------------------\e[0m"
    echo -e "\e[1;32m✅ No Leaks\e[0m"
    echo -e "\e[32m------------------------------------------------\e[0m"
    echo "$LEAKS"
    echo ""
else
    echo -e "\e[31m------------------------------------------------\e[0m"
    echo -e "\e[1;31m❌ LEAKS! '$LOG_FILE' -> details.\e[0m"
    echo -e "\e[31m------------------------------------------------\e[0m"
    echo "$LEAKS"
    echo ""
fi