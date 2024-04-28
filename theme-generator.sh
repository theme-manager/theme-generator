#!/bin/bash

if [ "$1" == "" ]; then
    echo "Usage: theme_switcher.sh <path_to_image.png> <compromizing_size> <number_of_colors>"
    echo "compromizing_size: 4, 8, 16, 32, 48, 64, 128, <256>"
    echo "number_of_colors: default is 5"
    exit 1
fi

if ! [ -f "$1" ]; then
    echo "Specified image file does not exist"
    exit 2
fi

srcImage="$1"

if [ "$2" == "" ]; then
    res=256
else 
    res="$2"; fi

if [ "$3" == "" ]; then
    num=5
else
    num="$3"; fi


convertAndSaveAsCss() {
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "tmp/colors.txt"
    tail -n +2 "tmp/colors.txt" > "tmp/colors_trim.txt"
    cut -d ' ' -f 4 "tmp/colors_trim.txt" > "tmp/colors_cut.txt" 
    uniq "tmp/colors_cut.txt" | head -n "$num" > "tmp/colors_hex.txt"
    echo "" > output/generated.css
    declare -i index=0
    for hexCode in $(cat tmp/colors_hex.txt); do
        echo "@define-color color${index}: ${hexCode};" >> output/generated.css
        index+=1
    done
}

createPNG() {
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "output/generated.png"
}

convertAndSaveAsCss "$res" "$num"
createPNG "$res" "$num"
