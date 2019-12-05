// This is free and unencumbered software released into the public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <https://unlicense.org>

using System;
using System.IO;


class SRTM_reader
{
    public String SRTM_folder = "";
    public SRTM_reader(String folder)
    {
        this.SRTM_folder = folder;
    }

    public int GetElevation(double latitude, double longitude)
    {
        // Generate file name
        String filename = SRTM_folder;
        filename += "\\";
        filename += ((latitude >= 0) ? 'N' : 'S');
        filename += ((int)(Math.Floor(latitude))).ToString("D2");
        filename += ((longitude >= 0) ? 'E' : 'W');
        filename += ((int)(Math.Abs(Math.Floor(longitude)))).ToString("D3");
        filename += ".hgt";

        try
        {
            using (FileStream fs = File.Open(filename, FileMode.Open))
            {
                // Determine if the database is SRTM1 or SRTM3 by checking
                // the file size.
                int blockSize = (fs.Length > 1201 * 1201 * 2) ? 3601 : 1201;

                // Calculate location in the file.
                int lat_row = (int)((latitude - Math.Floor(latitude)) * (blockSize - 1));
                int lon_row = (int)((longitude - Math.Floor(longitude)) * (blockSize - 1));
                lat_row = blockSize - 1 - lat_row;

                // Read the data from the file.
                fs.Seek((lat_row * blockSize + lon_row) * 2, SeekOrigin.Begin);
                Byte[] data = new byte[2];
                fs.Read(data, 0, 2);

                // Convert to little endian.
                return ((int)(data[0]) << 8) | ((int)data[1]);
            }
        } catch (Exception E) {
            return -32767;
        }
    }
}