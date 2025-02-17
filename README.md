# e-Paper Infoboard
 An infoboard with e-Paper display. Builded on a Waveshare's ESP32 dev board with 7.5" e-Paper display.
 This device just displays an image obtained from a remote server over HTTPS. There is periodic checking of a new image.

### Creating an image
1. Draw a black and white image and save it to an uncompressed file. PNG or GIF is okay.
2. Convert that image to XBM format, for example, using the ImageMagick 'convert' tool.
```
convert image.png -monochrome xbm:image.h
```
>[!NOTE]
> XBM is a very old format of the grafic files. It is just a C-code piece.
3. Convert an XBM file to a binary form that can be loaded to the infoboard with a next small program.
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include "image.h"

#define OUTFILE "image.bin"

extern int errno;
char wrkfile[256];
int of,rc;
int pict_len = ( image_width * image_height ) / 8;

char byte_mirror(char c){
  int i=0;
  char d=0;
  for (i=0; i < 8; i++){
    if ( c & ( 1 << i ) ) {
      d = d | ( 0x80 >> i );
    }
  }
  return(d);
}

int main(int argc, char *argv[]){
  int i=0;

  for ( i=0; i < pict_len; i++ ) {
    image_bits[i]=byte_mirror(image_bits[i]);
  }

  bzero(&wrkfile, sizeof(wrkfile));
  strlcpy(wrkfile, OUTFILE, sizeof(wrkfile));

  if ((of=open(wrkfile,O_WRONLY|O_CREAT,0644)) < 0){
    fprintf(stderr,"can't open file %s: %s",wrkfile,strerror(errno));
    exit(1);
  }

  if ((rc=write(of,image_bits,pict_len)) == -1){
    fprintf(stderr,"can't write to file %s: %s",wrkfile,strerror(errno));
    exit(1);
  }else{
    if (rc != pict_len){
      fprintf(stderr,"should be written %i bytes, but recorded %i (file %s)",pict_len,rc,wrkfile);
    }
  }

  if (close(of)) { // just for laugh
    fprintf(stderr,"I do not believe it: can't close the file %s: %s",wrkfile,strerror(errno));
  }

  exit(0);
}
```
>[!NOTE]
> A function called 'byte_mirror' is needed because Waveshare's 7.5" e-Paper displays have reversed bit order in a byte.

### Command line
To enter Commadline mode, you need to press the button more than 2 seconds right after booting.
Communication parameters for terminal: 115200,8N1.

| Command *arg* | Explanation |
| --- | --- |
| name *word* | Set device name (doing nothing) |
| ssid *word* | Set WiFi SSID |
| passw *word* | Set WiFi password |
| host *word* | Set host where an image is ( hostname or IPv4 ) |
| port *number* | Set destination port |
| uri1 *word* | Set URI of the info file ( like "/infoboard/image.info" )|
| uri2 *word* | Set URI of the image file ( like "/infoboard/image.bin" )|
| auth *word* | Set HTTP(S) authorization (0/1, 1=enable) |
| huser *word* | Set HTTP(S) username |
| hpassw *word* | Set HTTP(S) passwor d|
| debug *number* | Set level of debug messages |
| show | Show current configuration |
| save | Save configuration to EEPROM |
| reboot | Reboot |
| help | Get help |

