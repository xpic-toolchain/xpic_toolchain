// RUN: %clang_cc1 -fsyntax-only -verify %s

namespace PR6382 {
  int foo()
  {
    goto error;
    {
      struct BitPacker {
        BitPacker() {}
      };
      BitPacker packer;
    }

  error:
    return -1;
  }
}

namespace PR6383 {
  void test (bool gross)
  {
    struct compare_and_set
    {
      void operator() (const bool inner, const bool gross = false)
      {
        // the code
      }
    } compare_and_set2;

    compare_and_set2 (false, gross);
  }
}
