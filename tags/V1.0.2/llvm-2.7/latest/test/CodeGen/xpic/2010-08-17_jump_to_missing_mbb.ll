; RUN: llc < %s -O0 -march=xpic | not grep "BB1_-1"
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32"
target triple = "i386-mingw32"

define i32 @main() nounwind readnone optsize {
entry:
  %x1 = alloca i32, align 4                       ; <i32*> [#uses=3]
  volatile store i32 12, i32* %x1, align 4
  %0 = volatile load i32* %x1, align 4            ; <i32> [#uses=2]
  %1 = icmp eq i32 %0, 0                          ; <i1> [#uses=1]
  br i1 %1, label %test.exit, label %bb.i

bb.i:                                             ; preds = %entry
  %2 = and i32 %0, 255                            ; <i32> [#uses=1]
  %3 = icmp eq i32 %2, 1                          ; <i1> [#uses=1]
  %retval.i = select i1 %3, i32 0, i32 5          ; <i32> [#uses=1]
  br label %test.exit

test.exit:                                        ; preds = %entry, %bb.i
  %4 = phi i32 [ %retval.i, %bb.i ], [ 0, %entry ] ; <i32> [#uses=1]
  volatile store i32 0, i32* %x1, align 4
  ret i32 %4
}
