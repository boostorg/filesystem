//  program to probe for valid file and directory names  ---------------------//

//  (C) Copyright Beman Dawes 2002. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  This program uses various less-that-perfect coding practices in the hopes of
//  being portable to older, pre-standard, compilers.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdio>

#ifndef NO_DIRECT_H
#include <direct.h>
#endif

// poor coding practice; done to make it easier if encountering an older library
namespace std {}
using namespace std;

const char punctuation[] = " !\"#%&'();<=>?[\\]*+,-./:^_{|}~@`$";

void print_c( int c )
{
  if ( strchr( "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", c ) != 0 )
    cout << (char)c;
  else if ( c == ' ' )
    cout << "space";
  else if ( strchr( punctuation, c ) != 0 ) 
    cout << (char)c;
  else
    cout << "x" << hex << c << dec;
}

bool rangeable( int c )
  { return strchr( punctuation, c ) == 0; }

class lister
{
  bool range;
  int last_c;
public:
  lister() : range(false), last_c( 0 ) {}
  ~lister() { if ( range ) print_c( last_c ); }
  void include( int c )
  {
    if ( range )
    {
      if ( c == last_c+1 && rangeable( c ) && c != 'a' && c != 'A' )
        { last_c = c; return; }
      range = false;
      print_c( last_c );
      cout << " ";
    }
    // detect sequentially contiguous 2nd element of range
    else if ( c == last_c+1 && rangeable( last_c )
      && rangeable( c ) && c != 'a' && c != 'A' )
    {
      cout << "-";
      range = true;
      last_c = c;
      return;
    }
    else cout << " ";

    print_c( c );
    last_c = c;
  }
};

bool file_ok( const char * name )
{
  ofstream f( name );
  bool result = f.is_open();
  if ( result )
  {
    f.close();
    ifstream f2( name );
    if ( !f2.is_open() )
    {
      cerr << "*** Failed to open \"" << name << "\"" << endl;
      result = false;
    }
    else f2.close();
    if ( remove( name ) != 0 )
      cerr << "*** Failed to remove \"" << name << "\"" << endl;
  }
  return result;
}

void specific_file_test( const char * name )
{
  cout << "\nIs file name \"" << name << "\" valid: "
    << (file_ok( name ) ? "yes" : "no");
}

void file_case_sense( )
{
  cout << "\nAre file names case sensitive: ";
  remove( "ABC" );
  remove( "abc" );
  ofstream f( "ABC" );
  if ( !f.is_open() ) { cout << "could not complete test\n"; }
  f.close();
  ifstream f2( "abc" );
  cout << (f2.is_open() ? "no\n" : "yes\n");
  f2.close();
  remove( "ABC" );
  remove( "abc" );
}

typedef bool (*test_func)( int c );

bool file_single_char( int c )
{
  string s;
  s += static_cast<char>( c );
  return file_ok( s.c_str() );
}

bool file_first_char( int c )
{
  string s;
  s += static_cast<char>( c );
  s += "DEF";
  return file_ok( s.c_str() );
}

bool file_last_char( int c )
{
  string s;
  s += "GHI";
  s += static_cast<char>( c );
  return file_ok( s.c_str() );
}

bool file_middle_char( int c )
{
  string s;
  s += "JKL";
  s += static_cast<char>( c );
  s += "LKJ";
  return file_ok( s.c_str() );
}

void test( test_func f )
{
  {  
    cout << " OK:";
    lister l;
    for ( int c = 1; c <= 255; ++c ) if ( f( c ) ) l.include( c );
  }
  {  
    cout << "\n Not OK:";
    lister l;
    for ( int c = 1; c <= 255; ++c ) if ( !f( c ) ) l.include( c );
  }
  cout << "\n";
}

#ifndef NO_DIRECT_H
bool dir_ok( const char * name )
{
  rmdir( name );
  bool result = mkdir( name /*, S_IRWXU|S_IRWXG|S_IRWXO*/ ) == 0;
  rmdir( name );
  return result;
}

void specific_dir_test( const char * name )
{
  cout << "\nIs directory name \"" << name << "\" valid: "
    << (dir_ok( name ) ? "yes" : "no");
}

bool dir_single_char( int c )
{
  string s;
  s += static_cast<char>( c );
  return dir_ok( s.c_str() );
}

bool dir_first_char( int c )
{
  string s;
  s += static_cast<char>( c );
  s += "LMN";
  return dir_ok( s.c_str() );
}

bool dir_last_char( int c )
{
  string s;
  s += "OPQ";
  s += static_cast<char>( c );
  return dir_ok( s.c_str() );
}

bool dir_middle_char( int c )
{
  string s;
  s += "RST";
  s += static_cast<char>( c );
  s += "TSR";
  return dir_ok( s.c_str() );
}

#endif

//  main  --------------------------------------------------------------------//

int main()
{
  cout << "\nFILENAME_MAX: " << FILENAME_MAX << "\n";
  cout << "\nMaximum number of characters in a file name (by actual probe): ";
  string s;
  int n;
  for ( n = 1; n <= 256; ++n )
  {
    s += "A";
    if ( !file_ok( s.c_str() ) ) break;
  }
  if ( n == 256 ) cout << "more than 255\n";
  else cout << (n-1) << "\n";
  cout << "Note the above will be unreliable on systems which\n"
          "limit total path length rather than file name length.\n";

  file_case_sense();
 
  cout << "\nTests to illuminate 8.3 or similar restrictions:\n";
  specific_file_test( "ABC" );
  specific_file_test( "ABC." );
  specific_file_test( "ABC.D" );
  specific_file_test( "ABC.DE" );
  specific_file_test( "ABC.DEF" );
  specific_file_test( "ABC.DEFG" );
  specific_file_test( "ABC.DEFGH" );
  specific_file_test( "ABC.DEF.GHI" );

  cout << "\n\nSingle character file name:\n\n";
  test( file_single_char );

  cout << "\nFirst character of multi-character file name:\n\n";
  test( file_first_char );

  cout << "\nLast character of multi-character file name:\n\n";
  test( file_last_char );

  cout << "\nMiddle character in multi-character file name:\n\n";
  test( file_middle_char );

# ifndef NO_DIRECT_H
  specific_dir_test( "UVW" );
  specific_dir_test( "UVW." );
  specific_dir_test( "UVW.X" );
  specific_dir_test( "UVW.XY" );
  specific_dir_test( "UVW.XYZ" );
  specific_dir_test( "UVW.XYZZ" );
  specific_dir_test( "UVW.XYZZZ" );
  specific_dir_test( "UV.WXY.Z12" );

  cout << "\n\nSingle character directory name:\n\n";
   test( dir_single_char );

  cout << "\nFirst character of multi-character directory name:\n\n";
  test( dir_first_char );

  cout << "\nLast character of multi-character directory name:\n\n";
  test( dir_last_char );

  cout << "\nMiddle character in multi-character directory name:\n\n";
  test( dir_middle_char );

# endif

  return 0;
}
