//  Generate Compiler Status HTML from jam regression test output  -----------//

//  (C) Copyright Beman Dawes 2002. Permission to copy,
//  use, modify, sell and distribute this software is granted provided this
//  copyright notice appears in all copies. This software is provided "as is"
//  without express or implied warranty, and with no claim as to its
//  suitability for any purpose.

/*******************************************************************************

    This program was designed to work unchanged on all platforms and
    configurations.  All output which is platform or configuration dependent
    is obtained from external sources such as the status/Jamfile, the residue
    from jam execution, the tools/build/xxx-tools.jam files, or the output
    of the config_info tests.

    Please avoid adding platform or configuration dependencies during
    program maintenance.

*******************************************************************************/

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "tiny_xml.hpp"
namespace fs = boost::filesystem;
namespace xml = boost::tiny_xml;

#include <cstdlib>  // for abort
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>

using std::string;
using std::cout;

const string pass_msg( "Pass" );
const string warn_msg( "<font color=\"#FF9900\"><i>Warn</i></font>" );
const string fail_msg( "<font color=\"#FF0000\">"
                      "<big><i>Fail</i></big>"
                       "</font>" );
const string missing_residue_msg( "<i>Missing</i>" );

const std::size_t max_compile_msg_size = 10000;

namespace
{
  bool ignore_pass;
  bool no_warn;

  fs::directory_iterator end_itr;

  // It's immportant for reliability that we find the same compilers for each
  // test, and that they match the column header.  So save the names at the
  // time column headings are generated.
  std::vector<string> toolsets;

  fs::ifstream jamfile;

  string specific_compiler; // if running on one toolset only

  string html_log_name = "status_log.html";
  fs::ofstream html_log_file;

  const string empty_string;

//  convert backslashes to forward slashes -----------------------------------//

  void convert_backslashes( string & s )
  {
    for ( string::iterator itr = s.begin(); itr != s.end(); ++itr )
      if ( *itr == '\\' ) *itr = '/';
  }

//  find_file  ---------------------------------------------------------------//
//  given a directory to recursively search

  bool find_file( const fs::path & dir_path, const string & name,
    fs::path & path_found, const string & ignore_dir_named="" )
  {
    if ( !fs::exists( dir_path ) ) return false;
    for ( fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
      if ( fs::directory( *itr )
        && itr->leaf() != ignore_dir_named )
      {
        if ( find_file( *itr, name, path_found ) ) return true;
      }
      else if ( itr->leaf() == name )
      {
        path_found = *itr;
        return true;
      }
    return false;
  }

//  platform_desc  -----------------------------------------------------------//
//  from boost-root/status/bin/config_info.test/xxx/.../config_info.output

  string platform_desc( const fs::path & boost_root_dir )
  {
    string result;
    fs::path dot_output_path;

    // the gcc config_info "Detected Platform" sometimes reports "cygwin", so
    // prefer any of the other compilers.
    if ( find_file( boost_root_dir << "status/bin/config_info.test",
      "config_info.output", dot_output_path, "gcc" )
      || find_file( boost_root_dir << "status/bin/config_info.test",
      "config_info.output", dot_output_path ) )
    {
      fs::ifstream file( dot_output_path );
      if ( file )
      {
        while( std::getline( file, result ) )
        {
          if ( result.find( "Detected Platform: " ) == 0 )
          {
            result.erase( 0, 19 );
            return result;
          }
        }
        result.clear();
      }
    }
    return result;
  }

//  version_desc  ------------------------------------------------------------//
//  from boost-root/status/bin/config_info.test/xxx/.../config_info.output

  string version_desc( const fs::path & boost_root_dir,
    const string & compiler_name )
  {
    string result;
    fs::path dot_output_path;
    if ( find_file( boost_root_dir << "status/bin/config_info.test"
      << compiler_name, "config_info.output", dot_output_path ) )
    {
      fs::ifstream file( dot_output_path );
      if ( file )
      {
        if( std::getline( file, result ) )
        {
          string::size_type pos = result.find( "version " );
          if ( pos != string::npos )
          {
            result.erase( 0, pos+8 );
          }
          else result.clear();
        }
      }
    }
    return result;
  }

//  compiler_desc  -----------------------------------------------------------//
//  from boost-root/tools/build/xxx-tools.jam

  string compiler_desc( const fs::path & boost_root_dir,
    const string & compiler_name )
  {
    string result;
    fs::path tools_path( boost_root_dir << "tools/build" << compiler_name
      + "-tools.jam" );
    fs::ifstream file( tools_path );
    if ( file )
    {
      while( std::getline( file, result ) )
      {
        if ( result.substr( 0, 3 ) == "#//" )
        {
          result.erase( 0, 3 );
          return result;
        }
      }
      result.clear();
    }
    return result;
  }

//  test_type_desc  ----------------------------------------------------------//
//  from boost-root/status/Jamfile

  string test_type_desc( const string & test_name )
  {
    // adding "/" and ".c" eliminates a couple of corner cases.
    // ".c" rather than ".cpp" because regex library includes some .c tests
    string search_name1( "/" + test_name + ".c" );
    string search_name2( " " + test_name + " " );

    string result;
    if ( jamfile.is_open() )
    {
      jamfile.clear();
      jamfile.seekg(0);
      string line;
      while( std::getline( jamfile, line ) )
      {
        if ( line.find( ".c" ) != string::npos )
        {
          if ( line.find( "run " ) != string::npos )
            result = "run";
          else if ( line.find( "run-fail " ) != string::npos )
            result = "run-fail";
          else if ( line.find( "link " ) != string::npos )
            result = "link";
          else if ( line.find( "link-fail " ) != string::npos )
            result = "link-fail";
          else if ( line.find( "compile " ) != string::npos )
            result = "compile";
          else if ( line.find( "compile-fail " ) != string::npos )
            result = "compile-fail";
        }
        if ( result.size() &&
          ( line.find( search_name1 ) != string::npos
          || line.find( search_name2 ) != string::npos ) ) return result;
      }
      result.clear();
    }
    return result;
  }
  
//  target_directory  --------------------------------------------------------//
//  this amounts to a request to find a unique leaf directory

  fs::path target_directory( const fs::path & root )
  {
    if ( !fs::exists( root ) ) return fs::path("no-such-path");
    fs::path child;
    for ( fs::directory_iterator itr( root ); itr != end_itr; ++itr )
    {
      if ( fs::directory( *itr ) )
      {
        if ( child.empty() ) child = *itr;
        else throw std::runtime_error(
          string( "two target possibilities found: \"" )
            + child.generic_path() + "\" and \""
            + (*itr).generic_path() + "\"" );
      }
    }
    if ( child.empty() ) return root; // this dir has no children
    return target_directory( child );
  }

//  element_content  ---------------------------------------------------------//

  const string & element_content(
    const xml::element_ptr & root, const string & name )
  {
    static string empty_string;
    xml::element_list::iterator itr;
    for ( itr = root->elements.begin();
          itr != root->elements.end() && (*itr)->name != name;
          ++itr ) {}
    return itr != root->elements.end() ? (*itr)->content : empty_string;
  }

//  generate_report  ---------------------------------------------------------//

  bool generate_report( const xml::element_ptr & db,
                        const string & test_name,
                        const string & toolset,
                        bool pass )
  {
    // compile msgs sometimes modified, so make a local copy
    string compile( (pass && no_warn) 
      ? empty_string :  element_content( db, "compile" ) );

    const string & link( pass ? empty_string : element_content( db, "link" ) );
    const string & run( pass ? empty_string : element_content( db, "run" ) );

    // some compilers output the filename even if there are no errors or
    // warnings; detect this if one line of output and it contains no space.
    string::size_type pos = compile.find( '\n', 1 );
    if ( pos != string::npos && compile.size()-pos <= 2 
        && compile.find( ' ' ) == string::npos ) compile.clear();

    if ( compile.empty() && link.empty() && run.empty() ) return false;

    // limit compile message length
    if ( compile.size() > max_compile_msg_size )
    {
      compile.erase( max_compile_msg_size );
      compile += "...\n   (remainder deleted because of excessive size)\n";
    }

    html_log_file << "<h2><a name=\""
                  << test_name << " " << toolset << "\">"
                  << test_name << " / " << toolset << "</a></h2>\n";

    if ( !compile.empty() )
      html_log_file << "<h3>Compiler output:</h3><pre>"
        << compile << "</pre>\n";  
    if ( !link.empty() )
      html_log_file << "<h3>Linker output:</h3><pre>" << link << "</pre>\n";  
    if ( !run.empty()  )
      html_log_file << "<h3>Run output:</h3><pre>" << run << "</pre>\n";  
    return true;
  }

  //  do_cell  -----------------------------------------------------------------//

  bool do_cell( const fs::path & test_dir,
    const string & test_name,
    const string & toolset,
    string & target )
  // return true if any results except pass_msg
  {
    fs::path target_dir( target_directory( test_dir << toolset ) );
    bool pass = false;

    // missing jam residue
    if ( fs::exists( target_dir << (test_name + ".success") ) ) pass = true;
    else if ( !fs::exists( target_dir << (test_name + ".failure") ) )
    {
      target += "<td>" + missing_residue_msg + "</td>";
      return true;
    }

    // missing jam_log.xml
    fs::ifstream file( target_dir << "test_log.xml" );
    if ( !file )
    {
      std::cerr << "Missing jam_log.xml in target \""
        << target_dir.generic_path() << "\"\n";
      target += "<td>";
      target += pass ? pass_msg : fail_msg;
      target += "</td>";
      return pass;
    }
    xml::element_ptr db = xml::parse( file );

    // generate bookmarked report of results, and link to it
    bool anything_generated
      = generate_report( db, test_name, toolset, pass );

    target += "<td>";
    if ( anything_generated )
    {
      target += "<a href=\"";
      target += html_log_name;
      target += "#";
      target += test_name;
      target += " ";
      target += toolset;
      target += "\">";
      target += pass ? warn_msg : fail_msg;
      target += "</a>";
    }
    else  target += pass ? pass_msg : fail_msg;
    target += "</td>";
    return anything_generated || !pass;
  }

//  do_row  ------------------------------------------------------------------//

  void do_row( const fs::path & boost_root_dir,
    const fs::path & test_dir, // "c:/boost_root/status/bin/any_test.test"
    const string & test_name, // "any_test"
    string & target )
  {
    // get the path to the test program from the .test file contents
    string test_path( test_name ); // test_name is default if missing .test
    fs::path file_path;
    if ( find_file( test_dir, test_name + ".test", file_path ) )
    {
      fs::ifstream file( file_path );
      if ( file )
      {
        std::getline( file, test_path );
        if ( test_path.size() )
        {
          if ( test_path[0] == '\"' ) // added for non-Win32 systems
          {
            test_path = test_path.erase( 0, 1 ); // strip "
            test_path.erase( test_path.find( "\"" ) );
          }
          // test_path is now a disk path, so convert to URL style path
          convert_backslashes( test_path );
          string::size_type pos = test_path.find( "/libs/" );
          if ( pos != string::npos ) test_path.replace( 0, pos, ".." );
        }
      }
    }

    // extract the library name from the test_path
    string lib_name( test_path );
    string::size_type pos = lib_name.find( "/libs/" );
    if ( pos != string::npos )
    {
      lib_name.erase( 0, pos+6 );
      pos = lib_name.find( "/" );
      if ( pos != string::npos ) lib_name.erase( pos );
    }
    else lib_name.clear();

    // find the library documentation path
    string lib_docs_path( "../libs/" + lib_name + "/" );
    fs::path cur_lib_docs_path( boost_root_dir << "libs" << lib_name );
    if ( fs::exists( cur_lib_docs_path << "index.htm" ) )
      lib_docs_path += "index.htm";
    else if ( fs::exists( cur_lib_docs_path << "index.html" ) )
      lib_docs_path += "index.html";
    else if ( fs::exists( cur_lib_docs_path << "doc/index.htm" ) )
      lib_docs_path += "doc/index.htm";
    else if ( fs::exists( cur_lib_docs_path << "doc/index.html" ) )
      lib_docs_path += "doc/index.html";
    else if ( fs::exists( cur_lib_docs_path << (lib_name + ".htm") ) )
      lib_docs_path += lib_name + ".htm";
    else if ( fs::exists( cur_lib_docs_path << (lib_name + ".html") ) )
      lib_docs_path += lib_name + ".html";
    else if ( fs::exists( cur_lib_docs_path << "doc" << (lib_name + ".htm") ) )
      lib_docs_path += lib_name + ".htm";
    else if ( fs::exists( cur_lib_docs_path << "doc" << (lib_name + ".html") ) )
      lib_docs_path += lib_name + ".html";

    // generate the library name, test name, and test type table data
    string::size_type row_start_pos = target.size();
    target += "<tr><td><a href=\"" + lib_docs_path + "\">"  + lib_name  + "</a></td>";
    target += "<td><a href=\"" + test_path + "\">" + test_name + "</a></td>";
    string test_type = test_type_desc( test_name );
    target += "<td>" + test_type + "</td>";

    bool no_warn_save = no_warn;
    if ( test_type.find( "fail" ) != string::npos ) no_warn = true;

    // for each compiler, generate <td>...</td> html
    bool anything_to_report = false;
    for ( std::vector<string>::const_iterator itr=toolsets.begin();
      itr != toolsets.end(); ++itr )
    {
      anything_to_report |= do_cell( test_dir, test_name, *itr, target );
    }

    target += "</tr>";
    if ( ignore_pass && !anything_to_report ) target.erase( row_start_pos );
    no_warn = no_warn_save;
  }

//  do_table_body  -----------------------------------------------------------//

  void do_table_body(
    const fs::path & boost_root_dir, const fs::path & build_dir )
  {
    // rows are held in a vector so they can be sorted, if desired.
    std::vector<string> results;
    
    // each test directory
    for ( fs::directory_iterator itr( build_dir ); itr != end_itr; ++itr )
    {
      results.push_back( std::string() ); // no sort required, but leave code
                                          // in place in case that changes
      do_row( boost_root_dir, *itr,
              itr->leaf().substr( 0, itr->leaf().size()-5 ),
              results[results.size()-1] );
    }

    std::sort( results.begin(), results.end() );

    for ( std::vector<string>::iterator v(results.begin());
      v != results.end(); ++v )
      { cout << *v << "\n"; }
  }

//  do_table  ----------------------------------------------------------------//

  void do_table( const fs::path & boost_root_dir )
  {
    fs::path build_dir( boost_root_dir << "status" << "bin" );

    cout << "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">\n";
     
    // generate the column headings

    cout << "<tr><td>Library</td><td>Test Name</td>\n"
      "<td><a href=\"compiler_status.html#test-type\">Test Type</a></td>\n";

    fs::directory_iterator itr( build_dir );
    if ( itr != end_itr )
    {
      fs::directory_iterator compiler_itr( *itr );
      std::clog << "Using " << itr->generic_path() << " to determine compilers\n";
      std::clog << "Requested compiler is " << specific_compiler << "\n";
      for (; compiler_itr != end_itr; ++compiler_itr )
      {
        if ( fs::directory( *compiler_itr )  // check just to be sure
          && compiler_itr->leaf() != "test" ) // avoid strange directory (Jamfile bug?)
        {
          if ( specific_compiler.size() != 0
            && specific_compiler != compiler_itr->leaf() ) continue;
          std::clog << "  " << compiler_itr->leaf() << "\n";
          toolsets.push_back( compiler_itr->leaf() );
          string desc( compiler_desc( boost_root_dir,
                                                 compiler_itr->leaf() ) );
          string vers( version_desc( boost_root_dir,
                                                compiler_itr->leaf() ) );
          cout << "<td>"
               << (desc.size() ? desc : compiler_itr->leaf())
               << (vers.size() ? (string( "<br>" ) + vers ) : string( "" ))
               << "</td>\n";
        } 
      }
    }

    cout << "</tr>\n";

    // now the rest of the table body

    do_table_body( boost_root_dir, build_dir );

    cout << "</table>\n";
  }

} // unnamed namespace

//  main  --------------------------------------------------------------------//

#define BOOST_NO_CPP_MAIN_SUCCESS_MESSAGE
#include <boost/test/cpp_main.cpp>

int cpp_main( int argc, char * argv[] ) // note name!
{
  while ( argc > 1 && *argv[1] == '-' )
  {
    if ( argc > 2 && std::strcmp( argv[1], "--compiler" ) == 0 )
    {
      specific_compiler = argv[2];
      argc -= 2;
      argv += 2;
    }
    else if ( std::strcmp( argv[1], "--ignore-pass" ) == 0 )
      { ignore_pass = true; --argc; ++argv; }
    else if ( std::strcmp( argv[1], "--no-warn" ) == 0 )
      { no_warn = true; --argc; ++argv; }
  }

  fs::path boost_root_dir;
  if ( argc == 2 )
    boost_root_dir = fs::path( argv[1], fs::system_specific );
  else
  {
    std::cerr << "usage: compiler_status [--compiler name] boost-root-directory\n"
      "  options: --compiler name     Run for named compiler only\n"
      "           --ignore-pass       Do not report tests which pass all compilers\n"
      "           --no-warn           Warnings not reported if test passes\n";
    return 1;
  }

  html_log_file.open( html_log_name );
  if ( !html_log_file )
  {
    std::cerr << "Could not open HTML log output file: "
      << html_log_name << std::endl;
    return 1;
  }

  jamfile.open( boost_root_dir << "status/Jamfile" ); // may fail; that's OK

  char run_date[128];
  std::time_t tod;
  std::time( &tod );
  std::strftime( run_date, sizeof(run_date),
    "%X UTC, %A %d %B %Y", std::gmtime( &tod ) );

  cout << "<html>\n"
          "<head>\n"
          "<title>Boost Compiler Status Automatic Test</title>\n"
          "</head>\n"
          "<body bgcolor=\"#ffffff\" text=\"#000000\">\n"
          "<table border=\"0\">\n"
          "<tr>\n"
          "<td><img border=\"0\" src=\"../c++boost.gif\" width=\"277\" "
          "height=\"86\"></td>\n"
          "<td>\n"
          "<h1>Compiler Status: " + platform_desc( boost_root_dir ) + "</h1>\n"
          "<b>Run Date:</b> "
       << run_date
       << "\n</td>\n</table>\n<br>\n"
       ;

  do_table( boost_root_dir );

  cout << "</body>\n"
          "</html>\n"
          ;

  return 0;
}