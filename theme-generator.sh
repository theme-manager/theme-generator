#!/bin/sh

printUsage() {
    echo "Usage:
    theme-generator.sh [IMAGE_PATH] [OPTIONS]
    
Options:
    -h  --help              Show this help message
    -n          <number>    number of colors to extract from the image.
                            The default value is 5.
    -o          <output>    Output directory. Default directory is $HOME/.config/theme-generator/output
    -s          <size>      Size to which the image gets compromized before extracting colors.
                            The default value is 256. Values should be a power of 2 and ge 64 for best results.
    -f          [formats]   Output format Option. Default value is g. Can be multiple values
                                g for gtk-css
                                h for html-css
                                p for png
                                r for rgb-text-file
                                t for hex-text-file"
}

# error codes
# 0 - success
# 1 - missing argument(s)
# 2 - wrong argument(s)
# 3 - missing dependecy
# 4 - wrong configuration file
# 5 - internal error


if ! [ -f "$1" ]; then
    echo "Specified image file does not exist"
    exit 1
else 
    srcImage="$1"
    shift
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
    echo "Converting image to theme color stripe..."
    convert "$srcImage" -geometry "${res}x${res}" -colors "$num" -unique-colors -scale 4000% "$output"colors.png
}

if [ "$output" = "" ]; then
    output="$HOME/.config/theme-generator/"
    echo "Using the default output directory: $output"
    checkOutputDir "$output"
fi

while [ "$#" -gt 0 ]; do
    case "$1" in
        "-s")   if [ "$2" != "" ]; then 
                    res="$2"
                    shift 2
                else 
                    echo "Option $1 requires an argument";
                    exit 2
                fi ;;
        "-n")   if [ "$2" != "" ]; then 
                    num="$2"
                    shift 2
                else 
                    echo "Option $1 requires an argument" 
                    exit 2
                fi ;;
        "-f")   if [ "$2" != "" ]; then 
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
                    shift 2
                else 
                    echo "Option $1 requires at least one argument";
                    exit 2
                fi ;;
        "-o") checkOutputDir "$2" ;;
        *)
            echo "Invalid option ${1}"
            exit 2 ;;
    esac
done

[ $saveHexFile ] && echo "Saving colors as hex in text file..."
[ $saveRgbFile ] && echo "Saving colors as rgb in text file..." 
echo

fileFormat=$(echo "$srcImage" | rev | cut -d '/' -f1 | cut -d '.' -f1 | rev)
rm -r "${output:?}../Wallpaper/"
mkdir -p "$output../Wallpaper"
cp "$srcImage" "$output../Wallpaper/wallpaper.$fileFormat"
