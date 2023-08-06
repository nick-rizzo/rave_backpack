#opens a png, extracts the pixel data and returns an array of pixel values
#I am using it to convert black and white images into c style arrays

from PIL import Image, ImageOps
import numpy as np
import sys
np.set_printoptions(threshold=sys.maxsize)  # sys module should be imported

def main():
    #Open image 
    img = Image.open('font_8x10.png').convert('L')
    img_inverted = ImageOps.invert(img)

    np_img = np.array(img_inverted)
    np_img[np_img > 0] = 1

    f = open("../Core/Src/fonts.h", "a")
    f.write("const uint16_t Font8x10[] = {\n\t")
    for i, line in enumerate(np_img):
        f.write("0b")
        for val in line:
            f.write(" ".join(str(val)))
        f.write(", ")
        if ((i+1)%8==0):
            f.write("\n\t")
        
    f.write("};")
    f.close()
if __name__ == "__main__":
    main()