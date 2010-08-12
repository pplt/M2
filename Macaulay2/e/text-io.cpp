// (c) 1994 Michael E. Stillman

#include <stdio.h>
#include <string.h>
#include "text-io.hpp"

int MAX_LINE_LENGTH = 75;
int emit_line_len = 0;

int i_text_io()
{
  return 1;
}

void bignum_text_out(buffer &o, mpz_t a)
{
  char s[1000];
  char *str;

  int size = static_cast<int>(mpz_sizeinbase(a, 10) + 2);
  char *allocstr = (size > 1000 ? newarray_atomic(char,size) : s);

  str = mpz_get_str(allocstr, 10, a);
  o << str;

  if (size > 1000) deletearray(allocstr);
}

void clear_emit_size()
{
  emit_line_len = 0;
}

void emit_wrapped(const char *s)
{
  // We wish verbose display to wrap at some reasonable length.
  emit_line_len -= static_cast<int>(strlen(s));
  if (emit_line_len <= 0) {
    emit_line_len = MAX_LINE_LENGTH;
    fprintf(stdout, "\n");
    fprintf(stdout, wrapping_prefix);
  }
  fputs(s, stdout);
  fflush(stdout);
}

void emit(const char *s)
{
  fputs(s, stdout);
  fflush(stdout);
}

void emit_line(const char *s)
{
  fprintf(stdout, "%s%s%s", s, "\n", wrapping_prefix);
  fflush(stdout);
}


// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
// End: