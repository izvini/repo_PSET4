/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // store resizing factor n as int
    int n = atoi(argv[1]);

    // check for range of n
    if (n < 1 || n > 100)
    {
        printf("n must be in [1, 100]\n");
        return 1;
    }

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is  a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // create outputfile's in the BITMAPINFOHEADER
    BITMAPINFOHEADER bi_resized;

    // change image height, width and size of the image
    bi_resized.biSize = bi.biSize;
    bi_resized.biWidth = bi.biWidth * n;
    bi_resized.biHeight = bi.biHeight * n;
    bi_resized.biPlanes = bi.biPlanes;
    bi_resized.biBitCount = bi.biBitCount;
    bi_resized.biCompression = bi.biCompression;

    // determine padding for scanlines so that it fits the
    int padding_infile =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_resized = (4 - (bi_resized.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi_resized.biSizeImage = (bi_resized.biWidth * sizeof(RGBTRIPLE) + padding_resized) * abs(bi_resized.biHeight);
    bi_resized.biXPelsPerMeter = bi.biXPelsPerMeter;
    bi_resized.biYPelsPerMeter = bi.biYPelsPerMeter;
    bi_resized.biClrUsed = bi.biClrUsed;
    bi_resized.biClrImportant = bi.biClrImportant;

    // create outfile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf_resized;

    // change outfile's size and copy other info intact
    bf_resized.bfType = bf.bfType;
    bf_resized.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bi_resized.biSizeImage;
    bf_resized.bfReserved1 = bf.bfReserved1;
    bf_resized.bfReserved2 = bf.bfReserved2;
    bf_resized.bfOffBits = bf.bfOffBits;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_resized, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_resized, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int k = 0, biHeight = abs(bi.biHeight); k < biHeight; k++)
    {
        for(int i = 0; i < n; i++)
        {
            // iterate over pixels in scanline to print the image
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // copy each pixel n times in each scanline
                for(int a = 0; a < n; a++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            if (i < n - 1)
            {
                // take file position indicator back one scanline
                fseek(inptr, -1 * bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
            }
            else
            {
                // skip over padding, if any
                fseek(inptr, padding_infile, SEEK_CUR);
            }

            // add padding in resized_image
            for (int r = 0; r < padding_resized; r++)
            {
                fputc(0x00, outptr);
            }
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

         // that's all folks
    return 0;
}
