/***************************************************************************//**

  @file         tests.h

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Test declarations.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef SMB_JSON_TEST_H
#define SMB_JSON_TEST_H

void test_parse_strings(void);
void test_parse_numbers(void);
void test_parse_atomics(void);
void test_parse_arrays(void);
void test_parse_objects(void);
void test_compare_strings(void);

#endif // SMB_JSON_TEST_H
