//  Generate Compiler Status Log HTML from jam regression test output  -------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

#define BOOST_NO_CPP_MAIN_SUCCESS_MESSAGE
#include <boost/test/cpp_main.cpp>
#include <iostream>
#include <fstream>
#include <string>

using std::string;

int cpp_main( int argc, char ** argv )
{
  std::cout << 
    "<html>\n"
    "<head>\n"
    "<title>Regression Test Log</title>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Regression Test Log</h1>\n"
    "(Broken bookmarks end up displaying this message,<br>"
    " a possible indication of a library build failure.)\n";


  string s;
  string text;
  string toolset;
  string last_test_reported;

  while ( std::getline( std::cin, s ) )
  {
    text += s;
    text += "\n";
    if ( s.find( "succeeded-test-file" ) != string::npos
      || s.find( "failed-test-file" ) != string::npos )
    {
      toolset = "";
      string::size_type pos = s.find( ".test" );
      if ( pos != string::npos )
      {
        pos += 6;
        toolset = s.substr( pos, s.find_first_of( "/\\", pos ) - pos );
      }
    }
    else if ( s == "*** output file follows ***" )
    {
      text.clear();
      text += s;
      text += "\n";
      while ( ( std::getline( std::cin, s )
        && s.find( "failed above test: " ) == string::npos ) )
      {
        text += s;
        text += "\n";
      }
      string::size_type pos = s.find( "failed above test: " );
      string test_name = s.substr( pos+19, s.find( " ", pos+19 ) - (pos+19) );
      
      std::cout << "<h2><a name=\""
                << test_name << " " << toolset << "\">"
                << test_name << " / " << toolset << "</a></h2>\n"
                << "<pre>" << text << "</pre>\n";  
    }
    else if ( s.find( "...failed " ) != string::npos
      && s.find( "...failed updating" ) == string::npos )
    {
      string test_name;
      string toolset_name;
      toolset_name = s.substr( 10, s.find( "-", 10 ) - 10 );
      string::size_type pos = s.find( "status\\bin\\" );
      if ( pos != string::npos ) pos  += 11;
      else if ( (pos=s.find( "build\\bin\\" )) !=  string::npos ) pos  += 10;
      test_name = s.substr( pos, s.find( ".", pos ) - pos );
      last_test_reported = test_name;

      std::cout << "<h2><a name=\""
                << test_name << " " << toolset_name << "\">"
                << test_name << " / " << toolset_name << "</a></h2>\n"
                << "<pre>" << text << "</pre>\n";  
    }
    else if ( s.find( "-action " ) != string::npos )
    {
      text.clear();
    }
    else if ( s.find( "failed above test: " ) != string::npos )
    {
      string::size_type pos = s.find( "failed above test: " );
      string test_name = s.substr( pos+19, s.find( " ", pos+19 ) - (pos+19) );
      
      if ( test_name != last_test_reported )
        std::cout << "<h2><a name=\""
                  << test_name << " " << toolset << "\">"
                  << test_name << " / " << toolset << "</a></h2>\n"
                  << "No further information available\n";  
    }
  }

  std::cout << 
    "</body>\n"
    "</html>\n";

  return 0;
}
