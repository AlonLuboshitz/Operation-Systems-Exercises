# This script handles pgn moves and prints them
# It reads the pgn file split it into meta data (print its)
# Use py parser to parse the moves into a list.

# It need to have build in board of the game 8x8 rows - abcdefgh columns - 12345678
# The board have initial state - starting state
# It will print the board after each move

# It will use a pointer the move and act by the user input in order to move on the list
# The scripts wait for user input:
# d - next (p++), a - previous(p--), w - start(p=0), s -end(p=len), q-auit 
declare -A letter_to_num # Set a dictionary to convert letters to numbers a=0,b=1...
for i in {a..h}; do
    letter_to_num[$i]=$(( $(printf "%d" "'$i") - 97 ))
done
pgn_path="$1"
if [ ! -e "$pgn_path" ]; then
    echo "File does not exist: '$input_file'"
    exit 1
fi
# Read file
line_count=0
moves=""
echo Metadata from PGN file:
while IFS= read -r line; do
    if [ $line_count -eq 1 ]; then
        if [ -n "$line" ]; then
            moves="$moves$line "
        else
            line_count=0
            break
        fi
        continue
    fi
    echo "$line"
    if [ -z "$line" ]; then
        ((line_count++))
    fi
done < "$pgn_path"
paresed_moves=$(python3 parse_moves.py "$moves")
declare -a board # declare borad array
# Split the string into individual moves
IFS=' ' read -r -a moves_array <<< "$paresed_moves"
total_moves=${#moves_array[@]}
current_move=0
# Read file argument
start_board () {
    row_8=("r" "n" "b" "q" "k" "b" "n" "r")
    row_7=("p" "p" "p" "p" "p" "p" "p" "p")
    row_6=("." "." "." "." "." "." "." ".")
    row_5=("." "." "." "." "." "." "." ".")
    row_4=("." "." "." "." "." "." "." ".")
    row_3=("." "." "." "." "." "." "." ".")
    row_2=("P" "P" "P" "P" "P" "P" "P" "P")
    row_1=("R" "N" "B" "Q" "K" "B" "N" "R")
    board=(
        "${row_8[@]}"
        "${row_7[@]}"
        "${row_6[@]}"
        "${row_5[@]}"
        "${row_4[@]}"
        "${row_3[@]}"
        "${row_2[@]}"
        "${row_1[@]}"
        )
}

print_board () {
    echo " a b c d e f g h"
    for j in {0..7}; do
    row=$((8-$j))
    echo "$row ${board[@]:j*8:8} $row"
    done
    echo " a b c d e f g h"
    
   
}
move_foward () {
    
    move=${moves_array[$current_move]} # The move is a string of 4 characters given as an argument to the fucntion
    
    # Get coords
    column_before=${letter_to_num[${move:0:1}]}
    row_before=${move:1:1}
    column_after=${letter_to_num[${move:2:1}]}
    row_after=${move:3:1}
    # Get piece
    index_before=$((8-$row_before))*8+$column_before
    piece=${board[$index_before]}
    if [[ "$piece" = "K" && ( "$move" = "e1g1" || "$move" = "e1c1" ) ]]; then
        white_castling "$move"
    elif [[ "$piece" = "k" && ( "$move" = "e8g8" || "$move" = "e8c8" ) ]]; then
        black_castling "$move"
    fi
       
    # Move piece
    index_after=$((8-$row_after))*8+$column_after
    board[$index_after]=$piece
    if [ ${#move} -eq 5 ]; then
        # Check if last number in move is 1 then black promotion so keep small letter
        if [ $row_after = "1" ]; then
            board[$index_after]=${move:4:1}
        else # row is 8
            piece=${move:4:1}
            piece="${piece^^}"
            board[$index_after]=$piece
        fi
    fi
    
    
    board[$index_before]="."
    # Check Castling
     
    ((current_move++))
}
white_castling () {
    if [ "$1" = "e1g1" ];    then
        board[61]="R"
        board[63]="."
    elif [ "$1" = "e1c1" ]; 
    then
        board[59]="R"
        board[56]="."
    fi
}
black_castling () {
    if [ "$1" = "e8g8" ];    then
        board[5]="r"
        board[7]="."
    elif [ "$1" = "e8c8" ]; 
    then
        board[3]="r"
        board[0]="."
    fi
}
# is_promotion () {

# }
go_to_end () {
    for ((i=current_move; i<total_moves; i++)); do
        move_foward
    done
}
go_back () {
    if [ $current_move -eq 0 ]; then
        return
    fi
    start_board
    limit=$((current_move-1))
    current_move=0
    for ((; current_move<limit; i++)); do
        move_foward
    done
}
print_options () {
    echo -n "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit: "
}
print_game () {
    echo "Move $current_move/$total_moves"
    print_board
}
exit_game () {
    echo "Exiting."
    echo "End of game."
    exit 0
}
game () {
    while true; do
    print_options
    read -n 1 -p "" input
    if [[ $input == "d" ]]; then
        echo
        if [ $current_move -eq $total_moves ]; then
        echo "No more moves available."
        continue
        fi
        move_foward
        print_game
    elif [[ $input == "a" ]]; then
        echo
        if [ $current_move -eq 0 ]; then
           print_game
           continue
        fi        
        go_back
        print_game
    elif [[ $input == "w" ]]; then
        echo
        start_board
        current_move=0
        print_game
    elif [[ $input == "s" ]]; then
        echo
        go_to_end
        print_game 
    elif [[ $input == "q" ]]; then
        echo
        exit_game
    else
        echo
        echo "Invalid key pressed: $input"
    fi
done
}

start_board
print_game
game
