// Test that ASan uses the default blacklist from resource directory.
// RUN: %clangxx_asan -### %s 2>&1 | FileCheck %s
// CHECK: fsanitize-blacklist={{.*}}asan_blacklist.txt
