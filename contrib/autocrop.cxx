//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <argp.h>
#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

class Rect
{
public:
  Rect(int x1_, int y1_,
       int x2_, int y2_)
    : x1(x1_), y1(y1_),
      x2(x2_), y2(y2_)
  {}

  int x1;
  int y1;
  int x2;
  int y2;
};

class Color
{
public:
  int red;
  int green;
  int blue;

  Color ()
    : red (0), green (255), blue (0)
  {
  }

  Color (int r, int g, int b) 
    : red (r), green (g), blue (b)
  {    
  }
};

class Image
{
private:
  int m_width;
  int m_height;
  int row_bytes;

  std::vector<unsigned char> m_image;
  
public:
  Image (const Image& image)
    : m_width(image.m_width),
      m_height(image.m_height),
      row_bytes(image.row_bytes),
      m_image(image.m_image)
  {
  }

  Image(int width, int height) {
    m_width   = width;
    m_height  = height;
    row_bytes = width * 4;
    m_image.resize(row_bytes * height);
  }

  /** Load an image from a given png source */
  Image (const std::string& filename) 
  {
    FILE* fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 pwidth, pheight;
    int bit_depth, color_type, interlace_type, compression_type, filter_type;

    if ((fp = fopen(filename.c_str (), "rb")) == NULL)
      {
	perror (filename.c_str ());
	exit (EXIT_FAILURE);
      }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				     NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &pwidth, &pheight,
		 &bit_depth, &color_type, &interlace_type,
		 &compression_type, &filter_type);
    row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    
    if (0)
      {
        std::cout << "BitDepth:    " << bit_depth << "\n"
                  << "Colortype:   " << color_type << "\n"
                  << "Interlace:   " << interlace_type << "\n"
                  << "Filter Type: " << filter_type << "\n"
                  << "Row Bytes:   " << row_bytes << "\n"
                  << "Width:       " << pwidth << "\n"
                  << "Height:      " << pheight << "\n"
                  << std::endl;
      }

    // Create the 'data' array
    png_bytep row_pointers[pheight];
    for (unsigned int i = 0; i < pheight; ++i)
      row_pointers[i] = new png_byte[row_bytes];

    png_read_image(png_ptr, row_pointers);
    
    if (color_type != PNG_COLOR_TYPE_RGBA)
      {
        std::cout << "Unsupported color type" << std::endl;
        exit (EXIT_FAILURE);
      }

    m_width  = pwidth;
    m_height = pheight;

    m_image.resize (m_height * row_bytes);

    // Convert the png into our internal data structure
    for (int y = 0; y < m_height; ++y)
      for (int i = 0; i < row_bytes; ++i)
        {
          m_image[i + (row_bytes * y)] = row_pointers[y][i];
        }
	  
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose (fp);
  }
  
  ~Image () 
  {
  }

  Image crop(Rect rect) 
  {
    Image img(rect.x2 - rect.x1,
              rect.y2 - rect.y1);
    
    for(int y = 0; y < img.m_height; ++y)
      {
        for(int x = 0; x < img.row_bytes; ++x)
          {
            img.m_image[img.row_bytes * y + x] = m_image[(row_bytes * (y + rect.y1)) + x + rect.x1*4];
          }
      }
    
    return img;
  }

  Rect calc_crop_region() {
    Rect rect(m_width-1, m_height-1, 0, 0);
    
    for(int y = 0; y < m_height; ++y)
      {
        for(int x = 0; x < m_width; ++x)
          {
            if (m_image[4*(y * m_width + x)+3] != 0)
              { // Found non-transparent pixel
                rect.x1 = std::min(x, rect.x1);
                break;
              }
          }

        for(int x = m_width-1; x >=0; --x)
          {
            if (m_image[4*(y * m_width + x)+3] != 0)
              { // Found non-transparent pixel
                rect.x2 = std::max(x, rect.x2);
                break;
              }
          }
      }

    for(int x = 0; x < m_width; ++x)
      {
        for(int y = 0; y < m_height; ++y)
          {
            if (m_image[4*(y * m_width + x)+3] != 0)
              { // Found non-transparent pixel
                rect.y1 = std::min(y, rect.y1);
                break;
              }
          }

        for(int y = m_height-1; y >=0; --y)
          {
            if (m_image[4*(y * m_width + x)+3] != 0)
              { // Found non-transparent pixel
                rect.y2 = std::max(y, rect.y2);
                break;
              }
          }        
      }

    return rect;
  }
  
  void write_pnm(std::string filename)
  {
    FILE* fp;
    fp = fopen(filename.c_str (), "wb");
    if (fp == NULL)
      assert (false);

    fprintf(fp, "P3\n# CREATOR\n");
    fprintf(fp, "%d %d\n", m_width, m_height);
    fprintf(fp, "255\n");

    for (unsigned int i = 0; i < m_image.size (); ++i)
      fprintf(fp, "%d\n", m_image[i]);

    fclose(fp);
  }

  void write_png_file(std::string filename)
  {
    FILE* fp;
    fp = fopen(filename.c_str (), "wb");
    if (fp == NULL)
      assert (false);

    write_png(fp);

    fclose(fp);
  }

  void write_png (FILE* fp)
  {
    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, 
                 m_width, m_height, 8 /* bitdepth */,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, 
                 PNG_COMPRESSION_TYPE_BASE, 
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_uint_32 height = m_height;

    png_byte image[height * row_bytes];
    png_bytep row_pointers[height];
   
    // fill the image with data
    for (unsigned int i = 0; i < m_image.size (); ++i)
      image[i] = m_image[i];
    
    // generate row pointers
    for (unsigned int k = 0; k < height; k++)
      row_pointers[k] = image + (k * row_bytes);

    png_write_image(png_ptr, row_pointers);

    png_write_end(png_ptr, info_ptr);
  }

  void add_overlay(Image& mask, Image& image)
  {
    for (int y = 0; y < m_height; ++y)
      for (int x = 0; x < row_bytes; ++x)
        {
          int i = (y * row_bytes) + x;
          float alpha = (mask.m_image[i]/255.0);
          int new_color = int((m_image[i] * (1.0f - alpha)) + (image.m_image[i] * alpha));
          m_image[i] = new_color > 255 ? 255 : new_color;
        }
  }

  void rotate_90()
  {
    Image image(*this);
    
    for (int y = 0; y < m_height; ++y)
      for (int x = 0; x < m_width; ++x)
        {
          m_image[(y * row_bytes) + 3*x + 0] = image.m_image[((m_height - x - 1) * row_bytes) + 3*y + 0];
          m_image[(y * row_bytes) + 3*x + 1] = image.m_image[((m_height - x - 1) * row_bytes) + 3*y + 1];
          m_image[(y * row_bytes) + 3*x + 2] = image.m_image[((m_height - x - 1) * row_bytes) + 3*y + 2];
        }
  }

  void flip()
  {
    Image image(*this);
    
    for (int y = 0; y < m_height; ++y)
      for (int x = 0; x < m_width; ++x)
        {
          m_image[(y * row_bytes) + 3*x + 0] = image.m_image[((m_height - y - 1) * row_bytes) + 3*x + 0];
          m_image[(y * row_bytes) + 3*x + 1] = image.m_image[((m_height - y - 1) * row_bytes) + 3*x + 1];
          m_image[(y * row_bytes) + 3*x + 2] = image.m_image[((m_height - y - 1) * row_bytes) + 3*x + 2];
        }
  }
};

int main (int argc, char* argv[])
{
  if (argc != 2) 
    {
      puts("Usage: autocrop FILENAME");
    }
  else
    {
      Image image(argv[1]);
      Rect  crop = image.calc_crop_region();
      if (1)
        {
          std::cout << "Crop: " << crop.x1 << ", " << crop.y1 << ", "
                    << crop.x2 << ", " << crop.y2 << std::endl;
        }

      std::cout << "align: " << 512- crop.x1 << ", " << 384 - crop.y1 << std::endl;

      Image new_img = image.crop(crop);
      new_img.write_png_file("/tmp/bla.png");
    }
}


// EOF //
