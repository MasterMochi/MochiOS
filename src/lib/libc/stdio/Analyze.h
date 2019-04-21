/******************************************************************************/
/*                                                                            */
/* src/lib/libc/stdio/Analyze.h                                               */
/*                                                                 2019/02/20 */
/* Copyright (C) 2019 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
#ifndef ANALYZE_H
#define ANALYZE_H
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdint.h>


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/* フラグ */
#define ANALYZE_FLAG_LEFT  ( 0x00000001 )   /**< 左寄せフラグ */
#define ANALYZE_FLAG_SIGN  ( 0x00000002 )   /**< 符号フラグ   */
#define ANALYZE_FLAG_SPACE ( 0x00000004 )   /**< 空白フラグ   */
#define ANALYZE_FLAG_ALT   ( 0x00000008 )   /**< 代替フラグ   */
#define ANALYZE_FLAG_ZERO  ( 0x00000010 )   /**< 0埋めフラグ  */

/* 長さ修飾子 */
#define ANALYZE_LENGTH_CHAR       ( 0x00000001 )  /**< 長さ修飾子hh(char型)       */
#define ANALYZE_LENGTH_SHORT      ( 0x00000002 )  /**< 長さ修飾子h(short型)       */
#define ANALYZE_LENGTH_LONG       ( 0x00000003 )  /**< 長さ修飾子l(long型)        */
#define ANALYZE_LENGTH_LONGLONG   ( 0x00000004 )  /**< 長さ修飾子ll(long long型)  */
#define ANALYZE_LENGTH_PTRDIFF    ( 0x00000005 )  /**< 長さ修飾子t(ptrdiff_t型)   */
#define ANALYZE_LENGTH_SIZE       ( 0x00000006 )  /**< 長さ修飾子z(size_t型)      */
#define ANALYZE_LENGTH_LONGDOUBLE ( 0x00000007 )  /**< 長さ修飾子L(long double型) */


/******************************************************************************/
/* グローバル関数宣言                                                         */
/******************************************************************************/
/* フラグ解析 */
extern void AnalyzeFlag( uint32_t   *pFlag,
                         const char *pFormat,
                         int        *pReadIdx );
/* 長さ修飾子解析 */
extern void AnalyzeLength( uint32_t   *pLength,
                           const char *pFormat,
                           int        *pReadIdx );
/* 数字解析 */
extern void AnalyzeNumber( uint32_t   *pValue,
                           const char *pFormat,
                           int        *pReadIdx );
/* 精度解析 */
extern void AnalyzePrecision( uint32_t   *pPrecision,
                              const char *pFormat,
                              int        *pReadIdx    );


/******************************************************************************/
#endif
