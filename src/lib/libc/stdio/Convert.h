/******************************************************************************/
/*                                                                            */
/* src/lib/libc/stdio/Convert.h                                               */
/*                                                                 2019/03/24 */
/* Copyright (C) 2019 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdint.h>


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/** 変換情報 */
typedef struct {
    uint32_t flag;      /**< フラグ           */
    uint32_t width;     /**< 最小フィールド幅 */
    uint32_t precision; /**< 精度             */
    uint32_t length;    /**< 長さ修飾子       */
    char     specifier; /**< 変換指定子       */
} ConvertInfo_t;


/******************************************************************************/
/* グローバル関数宣言                                                         */
/******************************************************************************/
/* 変換 */
extern void Convert( char       *pStr,
                     size_t     size,
                     const char *pFormat,
                     va_list    *pArg,
                     int        *pReadIdx,
                     int        *pWriteIdx );
/* 1文字変換 */
extern void ConvertCharacter( char          *pStr,
                              size_t        size,
                              va_list       *pArg,
                              int           *pWriteIdx,
                              ConvertInfo_t *pInfo      );
/* 符号付き整数変換 */
extern void ConvertSignedInteger( char          *pStr,
                                  size_t        size,
                                  va_list       *pArg,
                                  int           *pWriteIdx,
                                  ConvertInfo_t *pInfo      );
/* 文字列変換 */
extern void ConvertString( char          *pStr,
                           size_t        size,
                           va_list       *pArg,
                           int           *pWriteIdx,
                           ConvertInfo_t *pInfo      );
/* 符号無し整数変換 */
extern void ConvertUnsignedInteger( char          *pStr,
                                    size_t        size,
                                    va_list       *pArg,
                                    int           *pWriteIdx,
                                    ConvertInfo_t *pInfo      );


/******************************************************************************/
