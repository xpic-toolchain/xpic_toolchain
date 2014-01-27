; RUN: llc < %s 2> /dev/null -O0 -march=xpic
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32"
target triple = "i386-mingw32"

define i32 @bigStack(i32 %x) nounwind readnone optsize {
entry:
  %buf = alloca [16385 x i8], align 1             ; <[16385 x i8]*> [#uses=2]
  %tmp = add i32 %x, 1                            ; <i32> [#uses=1]
  br label %bb, !dbg !0

bb:                                               ; preds = %bb, %entry
  %indvar = phi i32 [ %indvar.next, %bb ], [ 0, %entry ] ; <i32> [#uses=2]
  %tmp4 = sub i32 %x, %indvar                     ; <i32> [#uses=1]
  %scevgep = getelementptr [16385 x i8]* %buf, i32 0, i32 %tmp4 ; <i8*> [#uses=1]
  store i8 0, i8* %scevgep, align 1, !dbg !7
  %indvar.next = add i32 %indvar, 1               ; <i32> [#uses=2]
  %exitcond = icmp eq i32 %indvar.next, %tmp      ; <i1> [#uses=1]
  br i1 %exitcond, label %bb1, label %bb, !dbg !9

bb1:                                              ; preds = %bb
  %0 = getelementptr inbounds [16385 x i8]* %buf, i32 0, i32 0, !dbg !10 ; <i8*> [#uses=1]
  %1 = load i8* %0, align 1, !dbg !10             ; <i8> [#uses=1]
  %2 = sext i8 %1 to i32, !dbg !10                ; <i32> [#uses=1]
  ret i32 %2, !dbg !10
}

define i32 @main() nounwind readnone optsize {
entry:
  %buf.i = alloca [16385 x i8], align 1           ; <[16385 x i8]*> [#uses=2]
  br label %bb.i, !dbg !11

bb.i:                                             ; preds = %bb.i, %entry
  %indvar.i = phi i32 [ %indvar.next.i, %bb.i ], [ 0, %entry ] ; <i32> [#uses=2]
  %tmp2 = sub i32 3, %indvar.i                    ; <i32> [#uses=1]
  %scevgep.i = getelementptr [16385 x i8]* %buf.i, i32 0, i32 %tmp2 ; <i8*> [#uses=1]
  store i8 0, i8* %scevgep.i, align 1, !dbg !17
  %indvar.next.i = add i32 %indvar.i, 1           ; <i32> [#uses=2]
  %exitcond = icmp eq i32 %indvar.next.i, 4       ; <i1> [#uses=1]
  br i1 %exitcond, label %bigStack.exit, label %bb.i, !dbg !18

bigStack.exit:                                    ; preds = %bb.i
  %0 = getelementptr inbounds [16385 x i8]* %buf.i, i32 0, i32 0, !dbg !19 ; <i8*> [#uses=1]
  %1 = load i8* %0, align 1, !dbg !19             ; <i8> [#uses=1]
  %2 = sext i8 %1 to i32, !dbg !19                ; <i32> [#uses=1]
  ret i32 %2, !dbg !12
}

!0 = metadata !{i32 6, i32 0, metadata !1, null}
!1 = metadata !{i32 458798, i32 0, metadata !2, metadata !"bigStack", metadata !"bigStack", metadata !"bigStack", metadata !2, i32 6, metadata !3, i1 false, i1 true, i32 0, i32 0, null, i1 false} ; [ DW_TAG_subprogram ]
!2 = metadata !{i32 458769, i32 0, i32 1, metadata !"2010-09-08_large_stack_frame.c", metadata !"C:\5Cworkspace\5Cgalileo\5Cxpic_tools_testsuite/sources/sandbox", metadata !"4.2.1 (Based on Apple Inc. build 5658) (LLVM build)", i1 true, i1 true, metadata !"", i32 0} ; [ DW_TAG_compile_unit ]
!3 = metadata !{i32 458773, metadata !2, metadata !"", metadata !2, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !4, i32 0, null} ; [ DW_TAG_subroutine_type ]
!4 = metadata !{metadata !5, metadata !6}
!5 = metadata !{i32 458788, metadata !2, metadata !"int", metadata !2, i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ]
!6 = metadata !{i32 458788, metadata !2, metadata !"unsigned int", metadata !2, i32 0, i64 32, i64 32, i64 0, i32 0, i32 7} ; [ DW_TAG_base_type ]
!7 = metadata !{i32 9, i32 0, metadata !8, null}
!8 = metadata !{i32 458763, metadata !1, i32 6, i32 0} ; [ DW_TAG_lexical_block ]
!9 = metadata !{i32 11, i32 0, metadata !8, null}
!10 = metadata !{i32 12, i32 0, metadata !8, null}
!11 = metadata !{i32 6, i32 0, metadata !1, metadata !12}
!12 = metadata !{i32 17, i32 0, metadata !13, null}
!13 = metadata !{i32 458763, metadata !14, i32 16, i32 0} ; [ DW_TAG_lexical_block ]
!14 = metadata !{i32 458798, i32 0, metadata !2, metadata !"main", metadata !"main", metadata !"main", metadata !2, i32 16, metadata !15, i1 false, i1 true, i32 0, i32 0, null, i1 false} ; [ DW_TAG_subprogram ]
!15 = metadata !{i32 458773, metadata !2, metadata !"", metadata !2, i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !16, i32 0, null} ; [ DW_TAG_subroutine_type ]
!16 = metadata !{metadata !5}
!17 = metadata !{i32 9, i32 0, metadata !8, metadata !12}
!18 = metadata !{i32 11, i32 0, metadata !8, metadata !12}
!19 = metadata !{i32 12, i32 0, metadata !8, metadata !12}
