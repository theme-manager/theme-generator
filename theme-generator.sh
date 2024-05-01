#!/bin/sh

if [ "$1" = "" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "Usage:"
    echo "  theme-generator.sh [IMAGE_PATH] [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -f  formats     Output format Option. Default value is g. Can be multiple values"
    echo "                      g for gtk-css"
    echo "                      h for html-css"
    echo "                      p for png"
    echo "                      r for rgb-text-file"
    echo "                      t for hex-text-file"
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
saveHexFile=false
saveRgbFile=false

checkOutputDir() {
    if [ -d "$1" ]; then 
        output="$1"
    else
        echo "Specified output directory does not exist. Do you want to create the directory '$1'? (y/N)"
        read -r answer
        if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
            echo "$1"
            mkdir -p "$1" || echo "Failed to create directory $2" && exit 3
            output="$1"
            echo "$output created"
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
    "-o") checkOutputDir "$2" ;;
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

createColorFilesInTmp() {
    echo "Converting image to .css file for gtk..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "/tmp/colors.txt"
    tail -n +2 "/tmp/colors.txt" > "/tmp/colors_trim.txt"
    cut -d ' ' -f 4 "/tmp/colors_trim.txt" > "/tmp/colors_cut.txt" 
    uniq "/tmp/colors_cut.txt" | head -n "$num" > "/tmp/colors_hex.txt"
}

copyAndRemoveFilesFromTmp() {
    if "$saveHexFile"; then
        cp "/tmp/colors_hex.txt" "$output"colors-hex.txt
    fi; if [ "$saveRgbFile" ]; then
        cut -d ' ' -f 2 "/tmp/colors_trim.txt" > "/tmp/colors_rgb.txt"
        uniq "/tmp/colors_rgb.txt" | head -n "$num" | sed "s/(//g" | sed "s/)//g" | sed "s/,/ /g" > "/tmp/colors_rgb_uniq.txt"
        cp "/tmp/colors_rgb_uniq.txt" "$output"colors-rgb.txt
    fi
    rm "/tmp/colors.txt" "/tmp/colors_trim.txt" "/tmp/colors_cut.txt" "/tmp/colors_rgb.txt" "/tmp/colors_hex.txt" "/tmp/colors_rgb_uniq.txt"
}

convertAndSaveAsCssForGtk() {
    createColorFilesInTmp

    index=0
    echo "" > "$output"colors-gtk.css
    while IFS= read -r hexCode; do
        echo "@define-color color${index} ${hexCode};" >> "$output"colors-gtk.css
        index=$((index+=1))
    done < "/tmp/colors_hex.txt"

    copyAndRemoveFilesFromTmp
}

convertAndSaveAsCssForHtml() {
    createColorFilesInTmp

    index=0
    echo ":root {" > "$output"colors-html.css
    while IFS= read -r hexCode; do
        printf "\t--color%s: %s;\n" $index "$hexCode" >> "$output"colors-html.css
        index=$((index+=1))
    done < "/tmp/colors_hex.txt"
    echo "}" >> "$output"colors-html.css

    copyAndRemoveFilesFromTmp
}

convertAndSaveAsPNG() {
    echo "Converting image to .png image..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "$output"colors.png
}

if echo "$format" | grep -q "h"; then
    saveHexFile=true
fi; if echo "$format" | grep -q "r"; then
    saveRgbFile=true
fi; if echo "$format" | grep -q "g"; then
    convertAndSaveAsCssForGtk "$res" "$num"
fi; if echo "$format" | grep -q "h"; then
    convertAndSaveAsCssForHtml "$res" "$num"
fi; if echo "$format" | grep -q "p"; then
    convertAndSaveAsPNG "$res" "$num"
fi
echo "${output}/../wallpaper.png"
cp "$srcImage" "${output}wallpaper.png"
