/***************************************************************************//**

  @file         main.c

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Tests for the JSON parser.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "tests.h"

int main(int argc, char *argv[])
{
  (void)argc; //unused
  (void)argv; //unused

  test_parse_strings();
  test_parse_numbers();
  test_parse_atomics();
  test_parse_arrays();
  test_parse_objects();
  test_compare_strings();
  test_load_strings();

  return 0;
}

