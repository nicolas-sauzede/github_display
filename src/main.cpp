/*    This file is part of github_clock
      Copyright (C) 2017  Julien Thevenon ( julien_thevenon at yahoo.fr )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "github_display.h"
#include "quicky_exception.h"
#include "my_bmp.h"

#include <iostream>

int main(void)
{
  try
    {
      lib_bmp::my_bmp l_bmp_file("color.bmp");
      if(l_bmp_file.get_width() != 53 && l_bmp_file.get_height() != 8)
	{
	  throw quicky_exception::quicky_logic_exception("Bad bmp file dimensions (" + std::to_string(l_bmp_file.get_width()) + "," + std::to_string(l_bmp_file.get_height()) + ")",__LINE__,__FILE__);
	}
      std::map<lib_bmp::my_color_alpha,github_display::t_level> l_color_map =
	{
	  {lib_bmp::my_color_alpha(0xEE, 0xEE, 0xEE,0), github_display::t_level::L0},
	  {lib_bmp::my_color_alpha(0xd6, 0xe6, 0x85,0), github_display::t_level::L1},
	  {lib_bmp::my_color_alpha(0x8c, 0xc6, 0x65,0), github_display::t_level::L1},
	  {lib_bmp::my_color_alpha(0x44, 0xa3, 0x40,0), github_display::t_level::L1},
	  {lib_bmp::my_color_alpha(0x1e, 0x68, 0x23,0), github_display::t_level::L1}
	};
      github_display l_display;
      for(unsigned int l_y = 0;l_y < 7; ++l_y)
	{
	  for(unsigned int l_x = 0;l_x < 52; ++l_x)
	    {
	      lib_bmp::my_color_alpha l_color = l_bmp_file.get_pixel_color(l_x,l_y);
	      std::map<lib_bmp::my_color_alpha,github_display::t_level>::const_iterator l_iter = l_color_map.find(l_color);
	      if(l_color_map.end() != l_iter)
		{
		  l_display.set_pixel(l_x,l_y,l_iter->second);
		}
	      else
		{
		  std::stringstream l_stream;
		  l_stream << l_color;
		  throw quicky_exception::quicky_logic_exception("Bad color in bmp file (" + l_stream.str() +")",__LINE__,__FILE__);
		}
	    }
	}
      l_display.display();
    }
  catch(quicky_exception::quicky_runtime_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  catch(quicky_exception::quicky_logic_exception & e)
    {
      std::cout << "ERROR : " << e.what() << std::endl ;
      return(-1);
    }
  catch(std::exception & e)
    {
      std::cout << "ERROR from std::exception : " << e.what() << std::endl ;
      return(-1);
    }
  return EXIT_SUCCESS;
}
// EOF
