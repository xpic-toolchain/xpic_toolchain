; RUN: llc < %s -march=xpic | FileCheck %s
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-f80:128:128-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32"
target triple = "i686-pc-mingw32"

; CHECK: add1
define i32 @add1(i32* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i32* %pa                           ; <i32> [#uses=1]
  %add = add nsw i32 %tmp1, %b                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: add2
define i32 @add2(i32 %a, i32* nocapture %pb) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp2 = load i32* %pb                           ; <i32> [#uses=1]
  %add = add nsw i32 %tmp2, %a                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: add3
define i32 @add3(i16* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = zext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %add = add nsw i32 %conv, %b                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: add4
define i32 @add4(i8* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = zext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %add = add nsw i32 %conv, %b                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: add5
define i32 @add5(i16* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = sext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %add = add nsw i32 %conv, %b                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: add6
define i32 @add6(i8* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = sext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %add = add nsw i32 %conv, %b                    ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %add
}

; CHECK: shift1a
define i32 @shift1a(i32* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i32* %pa                           ; <i32> [#uses=1]
  %shl = shl i32 %tmp1, %b                        ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift1b
define i32 @shift1b(i32* nocapture %pa) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i32* %pa                           ; <i32> [#uses=1]
  %shl = shl i32 %tmp1, 5                         ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift2a
define i32 @shift2a(i16* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = zext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %shl = shl i32 %conv, %b                        ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift2b
define i32 @shift2b(i16* nocapture %pa) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = zext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %shl = shl i32 %conv, 5                         ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift3a
define i32 @shift3a(i8* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = zext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %shl = shl i32 %conv, %b                        ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift3b
define i32 @shift3b(i8* nocapture %pa) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = zext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %shl = shl i32 %conv, 5                         ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift4a
define i32 @shift4a(i16* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = sext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %shl = shl i32 %conv, %b                        ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift4b
define i32 @shift4b(i16* nocapture %pa) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i16* %pa                           ; <i16> [#uses=1]
  %conv = sext i16 %tmp1 to i32                   ; <i32> [#uses=1]
  %shl = shl i32 %conv, 5                         ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift5a
define i32 @shift5a(i8* nocapture %pa, i32 %b) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = sext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %shl = shl i32 %conv, %b                        ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}

; CHECK: shift5b
define i32 @shift5b(i8* nocapture %pa) nounwind readonly {
entry:
; CHECK-NOT: load
  %tmp1 = load i8* %pa                            ; <i8> [#uses=1]
  %conv = sext i8 %tmp1 to i32                    ; <i32> [#uses=1]
  %shl = shl i32 %conv, 5                         ; <i32> [#uses=1]
; CHECK: inc pc
  ret i32 %shl
}
