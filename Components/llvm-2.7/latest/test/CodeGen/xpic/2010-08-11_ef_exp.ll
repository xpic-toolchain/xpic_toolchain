; RUN: llc < %s -O0 -march=xpic
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32"
target triple = "i386-mingw32"

define float @__ieee754_expf(float %x) nounwind readnone {
entry:
  %0 = fcmp ogt float %x, 0x41CF58B900000000      ; <i1> [#uses=1]
  br i1 %0, label %bb, label %bb3

bb:                                               ; preds = %entry
  %1 = fcmp ult float %x, 0x41CFC28AC0000000      ; <i1> [#uses=1]
  br i1 %1, label %bb2, label %bb1

bb1:                                              ; preds = %bb
  %2 = fptosi float %x to i32                     ; <i32> [#uses=2]
  %3 = sitofp i32 %2 to float                     ; <float> [#uses=1]
  br label %bb2

bb2:                                              ; preds = %bb1, %bb
  %t.0 = phi float [ %3, %bb1 ], [ undef, %bb ]   ; <float> [#uses=1]
  %k.0 = phi i32 [ %2, %bb1 ], [ undef, %bb ]     ; <i32> [#uses=1]
  %4 = fadd float %x, -1.000000e+000              ; <float> [#uses=1]
  br label %bb5

bb3:                                              ; preds = %entry
  %5 = fadd float %x, 0x46293E5940000000          ; <float> [#uses=1]
  %6 = fcmp ogt float %5, 1.000000e+000           ; <i1> [#uses=1]
  br i1 %6, label %bb4, label %bb5

bb4:                                              ; preds = %bb3
  %7 = fadd float %x, 1.000000e+000               ; <float> [#uses=1]
  ret float %7

bb5:                                              ; preds = %bb3, %bb2
  %t.1 = phi float [ %t.0, %bb2 ], [ undef, %bb3 ] ; <float> [#uses=3]
  %k.1 = phi i32 [ %k.0, %bb2 ], [ undef, %bb3 ]  ; <i32> [#uses=1]
  %x_addr.0 = phi float [ %4, %bb2 ], [ %x, %bb3 ] ; <float> [#uses=2]
  %8 = fmul float %t.1, 0x3E66376980000000        ; <float> [#uses=1]
  %9 = fadd float %8, 0xBEBBBD41C0000000          ; <float> [#uses=1]
  %10 = fmul float %9, %t.1                       ; <float> [#uses=1]
  %11 = fadd float %10, 0x3F11566AA0000000        ; <float> [#uses=1]
  %12 = fmul float %11, %t.1                      ; <float> [#uses=1]
  %13 = fadd float %12, 0xBF66C16C20000000        ; <float> [#uses=2]
  %14 = icmp eq i32 %k.1, 0                       ; <i1> [#uses=1]
  br i1 %14, label %bb6, label %bb8

bb6:                                              ; preds = %bb5
  %15 = fmul float %x_addr.0, %13                 ; <float> [#uses=1]
  %16 = fdiv float %15, %13                       ; <float> [#uses=1]
  ret float %16

bb8:                                              ; preds = %bb5
  ret float %x_addr.0
}

define i32 @main() nounwind readnone {
entry:
  ret i32 undef
}
