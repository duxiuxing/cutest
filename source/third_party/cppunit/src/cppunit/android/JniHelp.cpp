#include "cppunit/JniHelp.h"

CPPUNIT_NS_BEGIN

std::string
JniHelp::makeFilePathShorter( std::string path )
{
  std::string str = path;
  std::list<std::string> strList;
  int index = str.find( '/' );
  while ( index != -1 )
  {
    int length = str.length();
    if ( 0 != index )
    {
      // 把'/'左边的截断出来放到strlist
      std::string subStr = str.substr( 0, index );
      std::list<std::string>::reverse_iterator rit = strList.rbegin();
      if ( subStr == "." )
      {
        if ( rit == strList.rend() )
        {
          strList.push_back( subStr );
        }
      }
      else if ( subStr == ".." )
      {
        if ( rit == strList.rend() )
        {
          strList.push_back( subStr );
        }
        else if ( *rit == ".." )
        {
          strList.push_back( subStr );
        }
        else
        {
          strList.pop_back();
        }
      }
      else
      {
        strList.push_back( subStr );
      }
    }
    else
    {
      // 如果第一个字符就是'/'，则跳过
    }

    // 计算'/'右边的字符串长度
    int start = index + 1;
    length -= start;

    // 然后把'/'右边的字符串赋给str
    if ( length )
    {
      str = str.substr( start, length );
    }
    else
    {
      str = "";
    }

    index = str.find( '/' );
  }

  if ( !str.empty() )
  {
    strList.push_back( str );
  }

  str.clear();
  std::list<std::string>::iterator it = strList.begin();
  while ( it != strList.end() )
  {
    if ( !str.empty() )
    {
      str += '/';
    }

    str += *it;
    ++it;
  }

  return str;
}

CPPUNIT_NS_END
