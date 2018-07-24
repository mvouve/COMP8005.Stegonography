# Stegonography Project

This project was built as part of a project for COMP8005. It is capible of hiding a image in a larger image file. I'll be making minor changes to it in the next few weeks to make it more user friendly.

It was written in C++/Qt.

## How to use it.

### Encoding image

1. Open the program. A window will appear with a large area in the middle.
2. There is a field on the bottom of the image. A password can be set to encrypt the image using XOR encryption.
3. Drag a carrier image into the carrier image area. This image will be the host image for the covert image.
4. Drag the hidden image into the window outside the carrier image, this image will be written into the host image in the LSB. A prompt will show up asking what to save the new image as. (make sure to save it as a BMP.)
5. Save the image.

### Decoding image

1. Open the program.
2. Place image with hidden package in the carrier image area.
3. Enter the password you entered in the first step into the password field.
4. Press decode, pick a place to save the covert image.