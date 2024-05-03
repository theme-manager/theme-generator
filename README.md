# theme-generator
Shell script which extracts colors from image and saves the hex codes as an color palette image, .css file to use with html or .css file to use with gtk. Also saves a text-file containing all extracted hex-codes.

## Installation
The installation is easy. Just put the shell script in your ```$HOME/.config/theme-manager``` directory.

## Usage
    theme-generator.sh [IMAGE_PATH] [OPTIONS]
 
    Options:
        -f      formats     Output format Option. Default value is g. Can be multiple values  
                                g for gtk-css  
                                h for html-css  
                                p for png  
                                r for rgb-text-file (will only work when combined with 'g' or 'h')
                                t for hex-text-file (will only work when combined with 'g' or 'h')
        -n      count       Number of colors to extract from the image.  
                            The default value is 5.  
        -o      output      Output directory. Default directory is $HOME/.config/theme-generator/output  
        -s      size        Size to which the image gets compromized before extracting colors.  
                            The default value is 256. Values should be a power of 2 and ge 64 for best results.  

## Dependencies
ImageMagick: [https://github.com/ImageMagick/ImageMagick](https://github.com/ImageMagick/ImageMagick)