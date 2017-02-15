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
#ifndef GITHUB_DISPLAY_H
#define GITHUB_DISPLAY_H

#include "quicky_exception.h"
#include "sh_file.h"
#include <cassert>
#include <sstream>
#include <map>
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cerrno>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#if __GNUC__ < 5
namespace std
{
  std::string put_time(struct tm * p_tm, const char * p_format)
  {
    char l_result[100];
    strftime(l_result,100, p_format,p_tm);

    return l_result;
  }
}
#endif // __GNUC__ < 5

class github_display
{
public:
  github_display(void);
  typedef enum class level {L0 = 0, L1 = 1, L2 = 9, L3 = 17, L4 = 28} t_level;
  inline void set_pixel(unsigned int p_x,
			unsigned int p_y,
			t_level p_level
			);
  inline void display(void) const;
private:
  void execute_script(const std::string & p_file_name) const;
  void write_commit_command(std::ostream & p_stream,
			    const std::string & p_comment,
			    const std::string & p_date) const;

  static const unsigned int m_nb_column = 53;
  static const unsigned int m_nb_lines = 7;
  static const unsigned int m_nb_colors = 5;
  static std::map<std::string, unsigned int> m_day_to_line;
  typedef std::array<t_level,m_nb_lines> t_column;
  typedef std::array<t_column,m_nb_column> t_pixel_array;

  t_pixel_array m_pixels;
};

//------------------------------------------------------------------------------
std::ostream & operator<<(std::ostream & p_stream, const github_display::t_level & p_level)
{
  switch(p_level)
    {
    case github_display::t_level::L0:
      p_stream << "L0";
      break;
    case github_display::t_level::L1:
      p_stream << "L1";
      break;
    case github_display::t_level::L2:
      p_stream << "L2";
      break;
    case github_display::t_level::L3:
      p_stream << "L3";
      break;
    case github_display::t_level::L4:
      p_stream << "L4";
      break;
    default:
      throw quicky_exception::quicky_logic_exception("Unkown github_display::t_level " +  std::to_string((int)p_level),
						       __LINE__,
						       __FILE__
						       );
    }
  return p_stream;
}

std::map<std::string, unsigned int> github_display::m_day_to_line =
  {
    {"Sun", 0},
    {"Mon", 1},
    {"Tue", 2},
    {"Wed", 3},
    {"Thu", 4},
    {"Fri", 5},
    {"Sat", 6}
  };

//------------------------------------------------------------------------------
github_display::github_display(void)
{
  std::string l_email("ets.thevenon.gerard@free.fr");
  std::string l_remote("git@github.com-githubdisplay:githubdisplay/test.git");
  std::string l_user_name("githubdisplay");
  for(unsigned int l_y = 0; l_y < m_nb_lines ; ++l_y)
    {
      for(unsigned int l_x = 0; l_x < m_nb_column ; ++l_x)
	{
	  m_pixels[l_x][l_y] = t_level::L0;
	}
    }
  DIR * l_git_dir = opendir(".git");
  if(nullptr == l_git_dir)
    {
      std::cout << "Create local git repo" << std::endl;
      sh_file l_create_repo_file;
      std::string l_file_name("./create_repo.sh");
      l_create_repo_file.open(l_file_name);
      l_create_repo_file << "git init" << std::endl ;
      l_create_repo_file << "git config --local user.name \"" << l_user_name << "\"" << std::endl ;
      l_create_repo_file << "git config --local user.email \"" << l_email << "\"" << std::endl ;
      l_create_repo_file << "\\rm -rf display_file README" << std::endl ;
      l_create_repo_file << "touch display_file" << std::endl;
      l_create_repo_file << "echo \"# Display file\" >> display_file" << std::endl;
      l_create_repo_file << "touch README" << std::endl;
      l_create_repo_file << "echo \"Generated with https://github.com/quicky2000/github_display\" >> README" << std::endl;
      l_create_repo_file << "git add display_file README" << std::endl;

      // Compute current time
      std::chrono::time_point<std::chrono::system_clock> l_now_tp = std::chrono::system_clock::now();

      std::chrono::hours l_nb_hours_delay(24 * (1 + m_nb_column) * m_nb_lines);

      // Compute the date of the day
      std::chrono::time_point<std::chrono::system_clock> l_day_tp = l_now_tp - l_nb_hours_delay;
      std::time_t l_day_time = std::chrono::system_clock::to_time_t(l_day_tp);

      // Compute string representation
      std::stringstream l_stream;
      l_stream << std::put_time(std::localtime(&l_day_time), "%c %Z");

      write_commit_command(l_create_repo_file,"Initial import",l_stream.str());

      //      l_create_repo_file << "git remote add origin ssh://gitolite@codex.cro.st.com/adas-verif/u/julien_thevenon/toto/TEST/initial.git" << std::endl;
      l_create_repo_file << "git remote add origin " << l_remote << std::endl;
      l_create_repo_file << "git remote update" << std::endl;
      l_create_repo_file << "git push -fu origin master" << std::endl;

      l_create_repo_file.close();
      execute_script(l_file_name);
    }
  else
    {
      closedir(l_git_dir);
    }
}

//------------------------------------------------------------------------------
void github_display::set_pixel(unsigned int p_x,
			       unsigned int p_y,
			       t_level p_level
			       )
{
  assert(p_x < m_nb_column);
  assert(p_y < m_nb_lines);
  m_pixels[p_x][p_y] = p_level;
}

//------------------------------------------------------------------------------
void github_display::execute_script(const std::string & p_file_name) const
{
  // Set execution rights on script
  chmod(p_file_name.c_str(),S_IRUSR | S_IWUSR | S_IXUSR);
  int l_pid = fork();
  switch(l_pid)
    {
    case 0:
      {
	std::cout << "In child" << std::endl ;
	int l_ret;
	l_ret = execlp(p_file_name.c_str(),
		       p_file_name.c_str(),
		       (char *)0
		       );
	if(-1 == l_ret)
	  {
	    perror(nullptr);
	    exit(EXIT_FAILURE);
	  }
	else
	  {
	    exit(0);
	  }
      }
      break;
    case -1:
      std::cout << "Creation of child failed" << std::endl;
      break;
    default:
      int l_status = 0;
      std::cout << "Waiting for end of child execution" << std::endl;
      waitpid(l_pid,&l_status,0);
      if(WIFEXITED(l_status))
	{
	  if(WEXITSTATUS(l_status))
	    {
	      throw quicky_exception::quicky_runtime_exception("Error during child execution : "+ std::to_string(l_status),__LINE__,__FILE__);
	    }
	}
      else
	{
	  throw quicky_exception::quicky_runtime_exception("Error child do not exit correctly execution",__LINE__,__FILE__);
	}
    }
}

//------------------------------------------------------------------------------
void github_display::write_commit_command(std::ostream & p_stream,
					  const std::string & p_comment,
					  const std::string & p_date) const
{
  p_stream << "GIT_COMMITTER_DATE=\"" << p_date << "\" git commit -q --date \"" << p_date << "\" -m \"" << p_comment << "\"" << std::endl ;
}


//------------------------------------------------------------------------------
void github_display::display(void) const
{
  // Compute current time
  std::chrono::time_point<std::chrono::system_clock> l_now_tp = std::chrono::system_clock::now();
  std::time_t l_now_time = std::chrono::system_clock::to_time_t(l_now_tp);

  // Convert it to string
  std::stringstream l_stream;
  l_stream << std::put_time(std::localtime(&l_now_time), "%c %Z");
  std::string l_time_str = l_stream.str();

  // Get Day
  assert(l_time_str.size() > 3);
  std::string l_day = l_time_str.substr(0,3);

  // Compute current line according to day
  std::map<std::string,unsigned int>::const_iterator l_iter = m_day_to_line.find(l_day);
  assert(m_day_to_line.end() != l_iter);
  unsigned int l_current_line = l_iter->second;
  std::cout << "Current line : " << l_current_line << std::endl;

  // Compute a reference time at the beginning of the day
  std::tm * l_reference_tm = std::localtime(&l_now_time);
  l_reference_tm->tm_sec = 0;
  l_reference_tm->tm_min = 1;
  l_reference_tm->tm_hour = 0;
  std::time_t l_reference_time = mktime(l_reference_tm);
  std::chrono::time_point<std::chrono::system_clock> l_reference_tp = std::chrono::system_clock::from_time_t(l_reference_time);
  std::cout << std::put_time(std::localtime(&l_reference_time), "%c %Z") << std::endl;

  const std::string l_file_name("./update_repo.sh");
  sh_file l_file;
  l_file.open(l_file_name.c_str());

  // Reset Git repo to first commit
  l_file << "git reset --hard `git log | grep commit | tail -1 | cut -d\" \" -f2`" << std::endl ;
  for(unsigned int l_x = 0; l_x < m_nb_column ; ++l_x)
    {
      unsigned int l_line_max = l_x != m_nb_column - 1 ? m_nb_lines : l_current_line + 1;
      for(unsigned int l_y = 0; l_y < l_line_max ; ++l_y)
	{

	  // Compute difference between current reference time and time corresponding to this pixel
	  t_level l_level = m_pixels[l_x][l_y];
	  l_file << "#(" << l_x << "," << l_y << ") = " << l_level << " => " << (unsigned int) l_level << " commits" << std::endl ;
	  if(t_level::L0 != l_level)
	    {
	      // Compute number of days between day represented by this pixel and current data
	      unsigned int l_nb_day_delay = m_nb_lines * (m_nb_column - 1 - l_x) + l_current_line - l_y;
	      // Convert it to duration in hours ( hour is the maximum unit time
	      std::chrono::hours l_nb_hours_delay(24 * l_nb_day_delay);

	      // Compute the date of the day
	      std::chrono::time_point<std::chrono::system_clock> l_day_tp = l_reference_tp - l_nb_hours_delay;
	      std::time_t l_day_time = std::chrono::system_clock::to_time_t(l_day_tp);
	      l_file << "# " << l_nb_day_delay << " Day ago  : " << std::put_time(std::localtime(&l_day_time), "%c %Z") << std::endl;
	      for(unsigned int l_commit_index = 1; l_commit_index <= (unsigned int)l_level ; ++l_commit_index)
		{
		  std::chrono::time_point<std::chrono::system_clock> l_commit_tp = l_day_tp + std::chrono::minutes(l_commit_index - 1);
		  std::time_t l_commit_time = std::chrono::system_clock::to_time_t(l_commit_tp);
		  l_file << "echo \"# " << l_nb_day_delay << " Day ago  : " << std::put_time(std::localtime(&l_commit_time), "%c %Z") << "\" >> display_file" << std::endl;
		  l_file << "git add display_file"  << std::endl;

		  // Convert commit time to string
		  std::stringstream l_stream;
		  l_stream << std::put_time(std::localtime(&l_commit_time), "%c %Z");
		  std::string l_commit_time_str = l_stream.str();

		  //		  GIT_COMMITTER_DATE="`date -R`" git commit --date "`date -R`"
		  std::stringstream l_commit_stream;
		  l_commit_stream << "(" << l_x << "," << l_y << ") : Commit " << l_commit_index << " / " << (unsigned int)l_level;
		  write_commit_command(l_file,l_commit_stream.str(),l_commit_time_str);
		}
	    }
	}
    }
  l_file << "git push -f origin master" << std::endl;
  l_file.close();
  execute_script(l_file_name);
}

#endif // GITHUB_DISPLAY_H
// EOF
