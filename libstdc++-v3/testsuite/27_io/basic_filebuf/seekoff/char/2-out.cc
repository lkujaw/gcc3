// 2001-05-21 Benjamin Kosnik  <bkoz@redhat.com>

// Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// 27.8.1.4 Overridden virtual functions

#include <fstream>
#include <testsuite_hooks.h>
#include <testsuite_io.h>

// @require@ %-*.tst %-*.txt
// @diff@ %-*.tst %*.txt

const char name_01[] = "seekoff-2out.tst";

void test05() 
{
  using namespace std;
  using namespace __gnu_cxx_test;

  typedef filebuf::int_type 	int_type;
  typedef filebuf::pos_type 	pos_type;
  typedef filebuf::off_type 	off_type;
  typedef filebuf::traits_type 	traits_type;

  bool 				test = true;
  streamsize 			strmsz_1, strmsz_2;
  streamoff  			strmof_1, strmof_2;

  int_type c1;
  int_type c2;
  int_type c3;

  pos_type pt_1(off_type(-1));
  pos_type pt_2(off_type(0));
  off_type off_1 = 0;
  off_type off_2 = 0;

  // seekoff
  // pubseekoff(off_type off, ios_base::seekdir way, ios_base::openmode which)
  // alters the stream position to off

  // out
  {
    constraint_filebuf fb;
    fb.pubsetbuf(0, 0);
    fb.open(name_01, ios_base::out);
    VERIFY( fb.unbuffered() );

    //beg
    strmsz_1 = fb.in_avail(); 
    pt_1 = fb.pubseekoff(2, ios_base::beg);
    strmsz_2 = fb.in_avail(); 
    off_1 = pt_1;
    VERIFY( off_1 > 0 );
    c1 = fb.snextc(); //current in pointer +1
    VERIFY( c1 == traits_type::eof() );
    fb.pubseekoff(3, ios_base::beg);
    c2 = fb.sputc('\n');  //current in pointer +1
    fb.pubseekoff(4, ios_base::beg);
    c3 = fb.sgetc();
    VERIFY( c2 != c3 ); 
    VERIFY( c3 == traits_type::eof() );
    fb.pubsync(); 
    c1 = fb.sgetc();
    VERIFY( c1 == c3 );

    //cur
    pt_2 = fb.pubseekoff(2, ios_base::cur);
    off_2 = pt_2;
    VERIFY( (off_2 == (off_1 + 2 + 1 + 1)) );
    c1 = fb.snextc(); //current in pointer +1
    VERIFY( c1 == traits_type::eof() );
    fb.pubseekoff(0, ios_base::cur);
    c2 = fb.sputc('x');  //test current out pointer
    c3 = fb.sputc('\n');
    fb.pubseekoff(0, ios_base::cur);
    c1 = fb.sgetc();
    fb.pubsync(); 
    c3 = fb.sgetc();
    VERIFY( c1 == c3 );

    //end
    pt_2 = fb.pubseekoff(0, ios_base::end);
    off_1 = pt_2;
    VERIFY( off_1 > off_2 ); //weak, but don't know exactly where it ends
    c3 = fb.sputc('\n');
    strmsz_1 = fb.sputn("because because because. . .", 28);  
    VERIFY( strmsz_1 == 28 );
    fb.pubseekoff(-1, ios_base::end);
    fb.sgetc();
    c1 = fb.sungetc();
    // Defect?  retval of sungetc is not necessarily the character ungotten.
    // So re-get it.
    c1 = fb.sgetc();
    fb.pubsync(); 
    c3 = fb.sgetc();
    VERIFY( c1 == c3 );
    VERIFY( fb.unbuffered() );
  }
}

main() 
{
  test05();
  return 0;
}