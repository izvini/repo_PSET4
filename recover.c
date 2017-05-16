/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdint.h>
#include <stdio.h>

// structure for each block of 512 bytes data
typedef struct
{
    uint8_t array[512];
}
block;

// checks if new image starts in current block
int jpgF(block data);


int main(void)
{
    // open file for reading
    FILE * input = fopen("card.raw", "r");

    // declare output file pointer
    FILE * output;

    // temporary storage for data
    block data;

    // count of images recovered
    int count = 0;

    // filename
    char fName[8];

    // read till end of file
    while(fread(&data, sizeof(data), 1, input) == 1)
    {
        // if jpg is found
        if (jpgF(data) == 1)
        {
            // if it is the first jpg
            if (count == 0)
            {
                sprintf(fName, "%03d.jpg", count);
                output = fopen(fName, "w");
                fwrite(&data, sizeof(data), 1, output);
                count++;
            }
            else
            {
                fclose(output);
                sprintf(fName, "%03d.jpg", count);
                output = fopen(fName, "w");
                fwrite(&data, sizeof(data), 1, output);
                count++;
            }
        }
        else if(count > 0)
        {
            fwrite(&data, sizeof(data), 1, output);
        }
    }
 // close files
    fclose(input);
    fclose(output);

    // success
    return 0;
}

int jpgF(block data)
{
    // check for jpg magic numbers in each block 'data'
    if (data.array[0] == 0xff && data.array[1] == 0xd8 && data.array[2] == 0xff &&
        (data.array[3] == 0xe0 || data.array[3] == 0xe1 || data.array[3] == 0xe2
        || data.array[3] == 0xe3 || data.array[3] == 0xe4 || data.array[3] == 0xe5
        || data.array[3] == 0xe6 || data.array[3] == 0xe7 || data.array[3] == 0xe8
        || data.array[3] == 0xe9 || data.array[3] == 0xea || data.array[3] == 0xeb
        || data.array[3] == 0xec || data.array[3] == 0xed || data.array[3] == 0xee
        || data.array[3] == 0xef))

    {
        // found
        return 1;
    }

        // not found
    return 0;
}
