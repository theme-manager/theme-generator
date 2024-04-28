# theme-generator
Shell script which extracts colors from image and saves the hex codes as an color palette image, .css file to use with html or .css file to use with gtk.

## Usage
    theme-generator.sh [IMAGE_PATH] [OPTIONS]
 
Options: &emsp;&ensp;&nbsp;  
&emsp;-s&emsp;size&emsp;&emsp;&emsp;Size to which the image gets compromized before extracting colors.  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&ensp;The default value is 256. Values should be a power of 2 and ge 64 for best results.  
&emsp;-n&emsp;count&emsp;&emsp;Number of colors to extract from the image.  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&ensp;The default value is 5.  
&emsp;-f&emsp;formats&emsp;&nbsp;Output format Option. Default value is g. Can be multiple values  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;g for gtk-css  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;h for html-css  
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;p for png  
&emsp;-o&emsp;output&emsp;&ensp;Output directory. Default directory is $HOME/.config/theme-generator/output  

## Dependencies
ImageMagick: [https://github.com/ImageMagick/ImageMagick](https://github.com/ImageMagick/ImageMagick)