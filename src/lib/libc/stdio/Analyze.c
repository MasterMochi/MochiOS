/******************************************************************************/
/*                                                                            */
/* src/lib/libc/stdio/Analyze.c                                               */
/*                                                                 2019/04/20 */
/* Copyright (C) 2019 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

/* モジュールヘッダ */
#include "Analyze.h"


/******************************************************************************/
/* グローバル関数定義                                                         */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief           フラグ解析
 * @details         書式文字列pFormatの読み込みインデックスpReadIdxの位置にある
 *                  フラグを解析し、フラグpFlagに設定する。
 *
 * @param[out]      *pFlag    フラグ
 * @param[in]       *pFormat  書式文字列
 * @param[in,out]   *pReadIdx 読込みインデックス
 */
/******************************************************************************/
void AnalyzeFlag( uint32_t   *pFlag,
                  const char *pFormat,
                  int        *pReadIdx )
{
    /* 1文字毎に繰り返す */
    while ( true ) {
        /* フラグ判定 */
        switch ( pFormat[ *pReadIdx ] ) {
            case '-':
                /* 左寄せ */
                *pFlag |=  ANALYZE_FLAG_LEFT;
                *pFlag &= ~ANALYZE_FLAG_ZERO;  /* 0フラグ無効 */
                break;

            case '+':
                /* 符号 */
                *pFlag |=  ANALYZE_FLAG_SIGN;
                *pFlag &= ~ANALYZE_FLAG_SPACE; /* 空白フラグ無効 */
                break;

            case ' ':
                /* 空白 */

                /* +フラグ競合チェック */
                if ( ( *pFlag & ANALYZE_FLAG_SIGN ) == 0 ) {
                    /* 非競合 */
                    *pFlag |= ANALYZE_FLAG_SPACE;
                }
                break;

            case '#':
                /* 代替 */
                *pFlag |= ANALYZE_FLAG_ALT;
                break;

            case '0':
                /* 0埋め */

                /* -フラグ競合チェック */
                if ( ( *pFlag & ANALYZE_FLAG_LEFT ) == 0 ) {
                    /* 非競合 */
                    *pFlag |= ANALYZE_FLAG_ZERO;
                }
                break;

            default:
                /* フラグ以外 */
                return;
        }

        /* 次のフラグへ */
        ( *pReadIdx )++;
    }

    return;
}


/******************************************************************************/
/**
 * @brief           長さ修飾子解析
 * @details         書式文字列pFormatの読み込みインデックスpReadIdxの位置にある
 *                  長さ修飾子を解析し、長さ修飾子pLengthに設定する。
 *
 * @param[out]      *length  長さ修飾子
 * @param[in]       *format  書式文字列
 * @param[in,out]   *readIdx 読込みインデックス
 */
/******************************************************************************/
void AnalyzeLength( uint32_t   *pLength,
                    const char *pFormat,
                    int        *pReadIdx )
{
    /* 文字判定 */
    switch( pFormat[ *pReadIdx ] ) {
        case 'h':
            /* 2文字目判定 */
            if ( pFormat[ *pReadIdx + 1 ] == 'h' ) {
                /* hh */
                *pLength   = ANALYZE_LENGTH_CHAR;
                *pReadIdx += 2;
            } else {
                /* h */
                *pLength   = ANALYZE_LENGTH_SHORT;
                *pReadIdx += 1;
            }
            break;

        case 'l':
            /* 2文字目判定 */
            if ( pFormat[ *pReadIdx + 1 ] == 'l' ) {
                /* ll */
                *pLength   = ANALYZE_LENGTH_LONGLONG;
                *pReadIdx += 2;
            } else {
                /* l */
                *pLength   = ANALYZE_LENGTH_LONG;
                *pReadIdx += 1;
            }
            break;

        case 't':
            /* t */
            *pLength   = ANALYZE_LENGTH_PTRDIFF;
            *pReadIdx += 1;
            break;

        case 'z':
            /* z */
            *pLength   = ANALYZE_LENGTH_SIZE;
            *pReadIdx += 1;
            break;

        case 'L':
            /* L */
            *pLength   = ANALYZE_LENGTH_LONGDOUBLE;
            *pReadIdx += 1;

        default:
            /* その他 */
            break;
    }

    return;
}


/******************************************************************************/
/**
 * @brief           数字解析
 * @details         書式文字列pFormatの読み込みインデックスpReadIdxの位置にある
 *                  10進数字を解析し、数値を設定する。
 *
 * @param[out]      *pValue   数値
 * @param[in]       *pFormat  書式文字列
 * @param[in,out]   *pReadIdx 読込みインデックス
 */
/******************************************************************************/
void AnalyzeNumber( uint32_t   *pValue,
                    const char *pFormat,
                    int        *pReadIdx )
{
    /* 初期化 */
    *pValue = 0;

    /* １文字毎に繰り返す */
    while ( true ) {
        /* 数字判定 */
        if ( '0' <= pFormat[ *pReadIdx ] && pFormat[ *pReadIdx ] <= '9' ) {
            /* 数字 */

            *pValue = *pValue * 10 + ( uint32_t ) ( pFormat[ *pReadIdx ] - '0' );

        } else {
            /* 数字以外 */

            break;
        }

        /* 次の文字へ */
        ( *pReadIdx )++;
    }

    return;
}


/******************************************************************************/
/**
 * @brief           精度解析
 * @details         書式文字列pFormatの読み込みインデックスpReadIdxの位置にある
 *                  精度を解析し、精度pPrecisionを設定する。
 *
 * @param[out]      *pPrecision 精度
 * @param[in]       *pFormat    書式文字列
 * @param[in,out]   *pReadIdx   読込みインデックス
 *
 * @note            「.」だけの場合は0として扱う。
 */
/******************************************************************************/
void AnalyzePrecision( uint32_t   *pPrecision,
                       const char *pFormat,
                       int        *pReadIdx    )
{
    /* 文字判定 */
    if ( pFormat[ *pReadIdx ] == '.' ) {
        /* . */

        /* 初期化 */
        *pPrecision = 0;

        /* 次の文字へ */
        ( *pReadIdx )++;

        /* 数字解析 */
        AnalyzeNumber( pPrecision, pFormat, pReadIdx );
    }

    return;
}


/******************************************************************************/
