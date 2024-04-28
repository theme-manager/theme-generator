#!/bin/sh

if [ "$1" = "" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "Usage:"
    echo "  theme-generator.sh [IMAGE_PATH] [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -f  formats     Output format Option. Default value is g. Can be multiple values"
    echo "                      p for png"
    echo "                      g for gtk-css"
    echo "                      h for html-css"
    echo "                      t for text-file"
    echo "  -h  --help      Show this help message"
    echo "  -n  count       number of colors to extract from the image."
    echo "                  The default value is 5."
    echo "  -o  output      Output directory. Default directory is $HOME/.config/theme-generator/output"
    echo "  -s  size        Size to which the image gets compromized before extracting colors."
    echo "                  The default value is 256. Values should be a power of 2 and ge 64 for best results."
    exit 0
fi

if ! [ -f "$1" ]; then
    echo "Specified image file does not exist"
    exit 1
else 
    srcImage="$1"
fi

res=256
num=5
format="g"
saveTextFile=false

checkOutputDir() {
    if [ -d "$1" ]; then 
        output="$1"
    else
        echo "Specified output directory does not exist. Do you want to create the directory '$1'? (y/N)"
        read -r answer
        if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
            mkdir -p "$1" || echo "Failed to create directory $2" && exit 3
            output="$1"
        else 
            echo "Aborting..."
            exit 3
        fi
    fi
}


setNextFlag() {
    if [ "$2" = "" ]; then
        echo "Option ${1} requires an argument"
        exit 2
    fi
    case "$1" in
    "-s") res="$2";;
    "-n") num="$2";;
    "-f") format="$2";;
    "-o") 
        echo "$1 $2" 
        checkOutputDir "$2" ;;
    *)
        echo "Invalid option ${1}"
        exit 2 ;;
    esac 
}

if [ "$2" != "" ]; then
    setNextFlag "$2" "$3"
fi; if [ "$4" != "" ]; then
    setNextFlag "$4" "$5"
fi; if [ "$6" != "" ]; then
    setNextFlag "$6" "$7"
fi; if [ "$8" != "" ]; then
    setNextFlag "$8" "$9"
fi

if [ "$output" = "" ]; then
    output="$HOME/.config/theme-generator/"
    echo "Using the default output directory: $output"
    checkOutputDir "$output"
fi

convertAndSaveAsCssForGtk() {
    echo "Converting image to .css file for gtk..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "/tmp/colors.txt"
    tail -n +2 "/tmp/colors.txt" > "/tmp/colors_trim.txt"
    cut -d ' ' -f 4 "/tmp/colors_trim.txt" > "/tmp/colors_cut.txt" 
    uniq "/tmp/colors_cut.txt" | head -n "$num" > "/tmp/colors_hex.txt"
    
    declare -i index=0
    echo "" > /tmp/generated-gtk.css
    while IFS= read -r hexCode; do
        echo "@define-color color${index} ${hexCode};" >> "$output"generated-gtk.css
        index+=1
    done < "/tmp/colors_hex.txt"

    if "$saveTextFile"; then
        cp "/tmp/colors_hex.txt" "$output"colors_hex.txt
    fi
    rm "/tmp/colors.txt" "/tmp/colors_trim.txt" "/tmp/colors_cut.txt"
}

convertAndSaveAsCssForHtml() {
    echo "Converting image to .css file for html..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "/tmp/colors.txt"
    tail -n +2 "/tmp/colors.txt" > "/tmp/colors_trim.txt"
    cut -d ' ' -f 4 "/tmp/colors_trim.txt" > "/tmp/colors_cut.txt" 
    uniq "/tmp/colors_cut.txt" | head -n "$num" > "/tmp/colors_hex.txt"

    index=0
    echo ":root {" > /tmp/generated-html.css
    while IFS= read -r hexCode; do
        printf "\t--color%s: %s;\n" $index "$hexCode" >> "$output"generated-html.css
        index=$((index+=1))
    done < "/tmp/colors_hex.txt"
    echo "}" >> "$output"generated-html.css

    if "$saveTextFile"; then
        cp "/tmp/colors_hex.txt" "$output"colors_hex.txt
    fi
    rm "/tmp/colors.txt" "/tmp/colors_trim.txt" "/tmp/colors_cut.txt"
}

convertAndSaveAsPNG() {
    echo "Converting image to .png image..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "$output"generated.png
}

echo "$format, $res, $num, $output"

# Does only execute one of the three functions, but all of them
#case "$format" in
#*g*) convertAndSaveAsCssForGtk "$res" "$num" ;;
#*h*) convertAndSaveAsCssForHtml "$res" "$num" ;;
#*p*) convertAndSaveAsPNG "$res" "$num" ;;
#*) echo "Invalid format" ;;
#esac

if [[ "$format" == *"t"* ]]; then
    saveTextFile=true
fi; if [[ "$format" == *"g"* ]]; then
    convertAndSaveAsCssForGtk "$res" "$num"
fi; if [[ "$format" == *"h"* ]]; then
    convertAndSaveAsCssForHtml "$res" "$num"
fi; if [[ "$format" == *"p"* ]]; then
    convertAndSaveAsPNG "$res" "$num"
fi
