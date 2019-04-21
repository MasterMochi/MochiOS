/******************************************************************************/
/*                                                                            */
/* src/lib/libc/stdio/Convert.c                                               */
/*                                                                 2019/04/08 */
/* Copyright (C) 2019 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* モジュールヘッダ */
#include "Analyze.h"
#include "Convert.h"


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/* 文字種別 */
#define UPPERCASE ( 0 ) /**< 大文字 */
#define LOWERCASE ( 1 ) /**< 小文字　*/

/** 整数変換バッファ最大長 */
#define BUFFER_LENGTH_MAX ( 10 )


/******************************************************************************/
/* ローカル関数宣言                                                           */
/******************************************************************************/
/* バッファ出力 */
static void PutBuffer( char          *pStr,
                       size_t        size,
                       int           *pWriteIdx,
                       char          *pBuffer,
                       int           bufferLength,
                       ConvertInfo_t *pInfo        );
/* バッファ出力(反転) */
static void PutBufferReverse( char          *pStr,
                              size_t        size,
                              int           *pWriteIdx,
                              char          *pBuffer,
                              int           bufferLength,
                              ConvertInfo_t *pInfo        );
/* 1文字出力 */
static inline void PutChar( char   *pStr,
                            size_t size,
                            int    idx,
                            char   c    );


/******************************************************************************/
/* グローバル変数定義                                                         */
/******************************************************************************/
/* 数変換テーブル */
const static char gNumConvTbl[ 2 ][ 17 ] = { "0123456789ABCDEF",
                                             "0123456789abcdef"  };


/******************************************************************************/
/* グローバル関数定義                                                         */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief           書式変換
 * @details         書式文字列pFormatの読込みインデックスpReadIdxの位置にある書
 *                  式を(必要があれば可変長引数リストpArgの引数を使って)変換し、
 *                  出力先文字列pStrの書込みインデックスpWriteIdxの位置に出力す
 *                  る。読込みインデックスpReadIdxと書込みpWriteIdxは、読込み書
 *                  込みを行った分が加算される。なお、出力先文字列サイズsizeを
 *                  超えた文字の出力は行わずに破棄する。
 *
 * @param[out]      *pStr      出力先文字列
 * @param[in]       size       出力先文字列サイズ
 * @param[in]       *pFormat   書式文字列
 * @param[in]       *pArg      可変長引数リスト
 * @param[in,out]   *pReadIdx  読込みインデックス
 * @param[in,out]   *pWriteIdx 書込みインデックス
 */
/******************************************************************************/
void Convert( char       *pStr,
              size_t     size,
              const char *pFormat,
              va_list    *pArg,
              int        *pReadIdx,
              int        *pWriteIdx )
{
    ConvertInfo_t info; /* 変換情報 */

    /* 初期化 */
    memset( &info, 0, sizeof ( ConvertInfo_t ) );
    info.precision = -1;

    /* フラグ解析 */
    AnalyzeFlag( &( info.flag ), pFormat, pReadIdx );

    /* 最小フィールド幅解析 */
    AnalyzeNumber( &( info.width ), pFormat, pReadIdx );

    /* 精度解析 */
    AnalyzePrecision( &( info.precision ), pFormat, pReadIdx );

    /* 長さ修飾子解析 */
    AnalyzeLength( &( info.length ), pFormat, pReadIdx );

    /* 変換指定子設定 */
    info.specifier = pFormat[ *pReadIdx ];

    /* 変換指定子判定 */
    switch ( info.specifier ) {
        case 'd': case 'i':
            /* 符号付き整数変換 */
            ConvertSignedInteger( pStr, size, pArg, pWriteIdx, &info );
            break;

        case 'o':
        case 'u':
        case 'x': case 'X':
            /* 符号無し整数変換 */
            ConvertUnsignedInteger( pStr, size, pArg, pWriteIdx, &info );
            break;

        case 'e': case 'E':
        case 'f': case 'F':
        case 'g': case 'G':
        case 'a': case 'A':
            /* 未対応 */
            break;

        case 'c':
            /* 1文字 */
            ConvertCharacter( pStr, size, pArg, pWriteIdx, &info );
            break;

        case 's':
            /* 文字列 */
            ConvertString( pStr, size, pArg, pWriteIdx, &info );
            break;

        case '%':
            /* % */
            PutChar( pStr, size, ( *pWriteIdx )++, '%' );
            break;

        default:
            /* エラー */
            return;
    }

    ( *pReadIdx )++;

    return;
}


/******************************************************************************/
/**
 * @brief           1文字変換
 * @details         1文字変換する。
 *
 * @param[out]      *pStr      出力先文字列
 * @param[in]       size       出力先文字列サイズ
 * @param[in]       *pArg      可変長引数リスト
 * @param[in,out]   *pWriteIdx 書込みインデックス
 * @param[in]       *pInfo     変換情報
 */
/******************************************************************************/
void ConvertCharacter( char          *pStr,
                       size_t        size,
                       va_list       *pArg,
                       int           *pWriteIdx,
                       ConvertInfo_t *pInfo      )
{
    char c;     /* 文字列 */

    /* 0フラグ無効 */
    pInfo->flag &= ~ANALYZE_FLAG_ZERO;

    /* 文字取得 */
    c = ( char ) va_arg( *pArg, int );

    /* バッファ出力 */
    PutBuffer( pStr, size, pWriteIdx, &c, 1, pInfo );

    return;
}


/******************************************************************************/
/**
 * @brief           符号付き整数変換
 * @details         変換情報pInfoをもとに可変長引数リストpArgから符号付き整数値
 *                  を取り出して変換した文字列を、出力先文字列pStrの書込みイン
 *                  デックスpWriteIdxの位置に書き込む。
 *
 * @param[out]      *pStr      出力先文字列
 * @param[in]       size       出力先文字列サイズ
 * @param[in]       arg        可変長引数リスト
 * @param[in,out]   *pWriteIdx 書込みインデックス
 * @param[in]       *pInfo     変換情報
 */
/******************************************************************************/
void ConvertSignedInteger( char          *pStr,
                           size_t        size,
                           va_list       *pArg,
                           int           *pWriteIdx,
                           ConvertInfo_t *pInfo      )
{
    int      idx;                           /* インデックス       */
    int      bufferLength;                  /* バッファ文字数     */
    char     buffer[ BUFFER_LENGTH_MAX ];   /* バッファ           */
    int32_t  value;                         /* 引数値             */
    int32_t  tmp;                           /* 一時変数           */
    uint32_t base;                          /* 基数               */
    uint32_t letterCase;                    /* 大文字小文字       */

    /* 初期化 */
    bufferLength = 0;
    memset( buffer, 0, BUFFER_LENGTH_MAX );

    /* 変換指定子判定 */
    switch ( pInfo->specifier ) {
        case 'd':
        case 'i':
            /* 10進整数 */
            base       = 10;
            letterCase = UPPERCASE;
            break;
        default:
            /* 不正 */
            return;
    }

    /* 長さ修飾子判定 */
    switch ( pInfo->length ) {
        case ANALYZE_LENGTH_CHAR:
            /* char型 */
            value = ( int32_t ) ( ( int8_t ) va_arg( *pArg, int ) );
            break;
        case ANALYZE_LENGTH_SHORT:
            /* short型 */
            value = ( int32_t ) ( ( int16_t ) va_arg( *pArg, int ) );
            break;
        case ANALYZE_LENGTH_LONG:
        case ANALYZE_LENGTH_LONGLONG:   /* 未対応 */
        case ANALYZE_LENGTH_PTRDIFF:
        case ANALYZE_LENGTH_SIZE:
        case ANALYZE_LENGTH_LONGDOUBLE: /* 未対応 */
        default:
            /* int型 */
            value = ( int32_t ) va_arg( *pArg, int );
            break;
    }
    tmp = value;

    /* 精度指定判定 */
    if ( pInfo->precision == -1 ) {
        /* 未指定 */

        /* デフォルト値設定 */
        pInfo->precision = 1;

    } else {
        /* 指定 */

        /* 0フラグ無効 */
        pInfo->flag &= ~ANALYZE_FLAG_ZERO;
    }

    /* 文字変換判定 */
    if ( !( ( pInfo->precision == 0 ) && ( tmp == 0 ) ) ) {
        /* する */

        /* 1桁毎に繰り返す */
        do {
            PutChar( buffer,
                     BUFFER_LENGTH_MAX,
                     bufferLength++,
                     gNumConvTbl[ letterCase ][ tmp % base ] );
            tmp /= base;
        } while ( tmp != 0 );

        /* 精度埋め */
        for ( idx = pInfo->precision - bufferLength; idx > 0; idx-- ) {
            PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '0' );
        }
    }

    /* 0埋めフラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_ZERO ) == 0 ) {
        /* 未指定 */

        /* 符号判定 */
        if ( value >= 0 ) {
            /* + */

            /* フラグ指定判定 */
            if ( ( pInfo->flag & ANALYZE_FLAG_SIGN ) != 0 ) {
                /* 符号フラグ指定 */

                PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '+' );

            } else if ( ( pInfo->flag & ANALYZE_FLAG_SPACE ) != 0 ) {
                /* 空白フラグ指定 */

                PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, ' ' );
            }

        } else {
            /* - */

            PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '-' );
        }

    } else {
        /* 指定 */

        /* 符号判定 */
        if ( value >= 0 ) {
            /* + */

            /* フラグ指定判定 */
            if ( ( pInfo->flag & ANALYZE_FLAG_SIGN ) != 0 ) {
                /* 符号フラグ指定 */

                PutChar( pStr, size, ( *pWriteIdx )++, '+' );
                pInfo->width--;

            } else if ( ( pInfo->flag & ANALYZE_FLAG_SPACE ) != 0 ) {
                /* 空白フラグ指定 */

                PutChar( pStr, size, ( *pWriteIdx )++, ' ' );
                pInfo->width--;
            }

        } else {
            /* - */

            PutChar( pStr, size, ( *pWriteIdx )++, '-' );
            pInfo->width--;
        }
    }

    /* バッファ出力 */
    PutBufferReverse( pStr, size, pWriteIdx, buffer, bufferLength, pInfo );

    return;
}


/******************************************************************************/
/**
 * @brief           文字列変換
 * @details         文字列変換する。
 *
 * @param[out]      *pStr      出力先文字列
 * @param[in]       size       出力先文字列サイズ
 * @param[in]       *pArg      可変長引数リスト
 * @param[in,out]   *pWriteIdx 書込みインデックス
 * @param[in]       *pInfo     変換情報
 */
/******************************************************************************/
void ConvertString( char          *pStr,
                    size_t        size,
                    va_list       *pArg,
                    int           *pWriteIdx,
                    ConvertInfo_t *pInfo      )
{
    char   *pArgStr;    /* 文字列   */
    size_t length;      /* 文字列長 */

    /* 0フラグ無効 */
    pInfo->flag &= ~ANALYZE_FLAG_ZERO;

    /* 文字列取得 */
    pArgStr = va_arg( *pArg, char * );

    /* 取得結果判定 */
    if ( pArgStr == NULL ) {
        /* 無効値 */

        pArgStr = "(null)";
    }

    /* 文字列長計算 */
    length = strlen( pArgStr );

    /* バッファ出力 */
    PutBuffer( pStr, size, pWriteIdx, pArgStr, length, pInfo );

    return;
}


/******************************************************************************/
/**
 * @brief           符号無し整数変換
 * @details         変換情報pInfoをもとに可変長引数リストpArgから符号無し整数値
 *                  を取り出して変換した文字列を、出力先文字列pStrの書込みイン
 *                  デックスpWriteIdxの位置に書き込む。
 *
 * @param[out]      *pStr      出力先文字列
 * @param[in]       size       出力先文字列サイズ
 * @param[in]       *pArg      可変長引数リスト
 * @param[in,out]   *pWriteIdx 書込みインデックス
 * @param[in]       *pInfo     変換情報
 */
/******************************************************************************/
void ConvertUnsignedInteger( char          *pStr,
                             size_t        size,
                             va_list       *pArg,
                             int           *pWriteIdx,
                             ConvertInfo_t *pInfo      )
{
    int      idx;                           /* インデックス       */
    int      bufferLength;                  /* バッファ文字数     */
    char     buffer[ BUFFER_LENGTH_MAX ];   /* バッファ           */
    uint32_t tmp;                           /* 一時変数           */
    uint32_t base;                          /* 基数               */
    uint32_t letterCase;                    /* 大文字小文字       */

    /* 初期化 */
    bufferLength = 0;
    memset( buffer, 0, BUFFER_LENGTH_MAX );

    /* 変換指定子判定 */
    switch ( pInfo->specifier ) {
        case 'o':
            /* 8進整数 */
            base       = 8;
            letterCase = UPPERCASE;
            break;
        case 'u':
            /* 10進整数 */
            base = 10;
            letterCase = UPPERCASE;
            break;
        case 'x':
            /* 16進整数 */
            base       = 16;
            letterCase = LOWERCASE;
            break;
        case 'X':
            /* 16進整数 */
            base       = 16;
            letterCase = UPPERCASE;
            break;
        default:
            /* 不正 */
            return;
    }

    /* 長さ修飾子判定 */
    switch ( pInfo->length ) {
        case ANALYZE_LENGTH_CHAR:
            /* char型 */
            tmp = ( uint32_t ) ( ( uint8_t ) va_arg( *pArg, unsigned int ) );
            break;
        case ANALYZE_LENGTH_SHORT:
            /* short型 */
            tmp = ( uint32_t ) ( ( uint16_t ) va_arg( *pArg, unsigned int ) );
            break;
        case ANALYZE_LENGTH_LONG:
        case ANALYZE_LENGTH_LONGLONG:   /* 未対応 */
        case ANALYZE_LENGTH_PTRDIFF:
        case ANALYZE_LENGTH_SIZE:
        case ANALYZE_LENGTH_LONGDOUBLE: /* 未対応 */
        default:
            /* int型 */
            tmp = ( uint32_t ) va_arg( *pArg, unsigned int );
            break;
    }

    /* 精度指定判定 */
    if ( pInfo->precision == -1 ) {
        /* 未指定 */

        /* デフォルト値設定 */
        pInfo->precision = 1;

    } else {
        /* 指定 */

        /* 0フラグ無効 */
        pInfo->flag &= ~ANALYZE_FLAG_ZERO;
    }

    /* 文字変換判定 */
    if ( !( ( pInfo->precision == 0 ) && ( tmp == 0 ) ) ) {
        /* する */

        /* 1桁毎に繰り返す */
        do {
            /* 文字変換 */
            PutChar( buffer,
                     BUFFER_LENGTH_MAX,
                     bufferLength++,
                     gNumConvTbl[ letterCase ][ tmp % base ] );
            tmp /= base;
        } while ( tmp != 0 );

        /* 精度埋め */
        for ( idx = pInfo->precision - bufferLength; idx > 0; idx-- ) {
            PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '0' );
        }
    }

    /* 0埋めフラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_ZERO ) == 0 ) {
        /* 未指定 */

        /* 代替形式フラグ指定判定 */
        if ( ( pInfo->flag & ANALYZE_FLAG_ALT ) != 0 ) {
            /* 指定 */

            /* 変換指定子判定 */
            switch ( pInfo->specifier ) {
                case 'o':
                    /* 8進整数 */
                    PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '0' );
                    break;
                case 'x':
                    /* 16進整数 */
                    PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, 'x' );
                    PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '0' );
                    break;
                case 'X':
                    /* 16進整数 */
                    PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, 'X' );
                    PutChar( buffer, BUFFER_LENGTH_MAX, bufferLength++, '0' );
                    break;
                default:
                    /* その他 */
                    break;
            }
        }

    } else {
        /* 指定 */

        /* 代替形式フラグ指定判定 */
        if ( ( pInfo->flag & ANALYZE_FLAG_ALT ) != 0 ) {
            /* 指定 */

            /* 変換指定子判定 */
            switch ( pInfo->specifier ) {
                case 'o':
                    /* 8進整数 */
                    PutChar( pStr, size, ( *pWriteIdx )++, '0' );
                    pInfo->width--;
                    break;
                case 'x':
                    /* 16進整数 */
                    PutChar( pStr, size, ( *pWriteIdx )++, 'x' );
                    pInfo->width--;
                    PutChar( pStr, size, ( *pWriteIdx )++, '0' );
                    pInfo->width--;
                    break;
                case 'X':
                    /* 16進整数 */
                    PutChar( pStr, size, ( *pWriteIdx )++, 'X' );
                    pInfo->width--;
                    PutChar( pStr, size, ( *pWriteIdx )++, '0' );
                    pInfo->width--;
                    break;
                default:
                    /* その他 */
                    break;
            }
        }
    }

    /* バッファ出力 */
    PutBufferReverse( pStr, size, pWriteIdx, buffer, bufferLength, pInfo );

    return;
}


/******************************************************************************/
/* ローカル関数定義                                                           */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief           バッファ出力
 * @details         変換情報pInfoをもとにバッファpBufferからバッファ長
 *                  bufferLength分を出力先文字列pStrに出力する。
 *
 * @param[out]      *pStr        出力先文字列
 * @param[in]       size         出力先文字列サイズ
 * @param[in,out]   *pWriteIdx   書込みインデックス
 * @param[in]       *pBuffer     バッファ
 * @param[in]       bufferLength バッファ長
 * @param[in]       *pInfo       変換情報
 */
/******************************************************************************/
static void PutBuffer( char          *pStr,
                       size_t        size,
                       int           *pWriteIdx,
                       char          *pBuffer,
                       int           bufferLength,
                       ConvertInfo_t *pInfo        )
{
    int  idx;   /* インデックス       */
    char pad;   /* フィールド埋め文字 */

    /* 初期化 */
    idx = 0;
    pad = ' ';

    /* 0フラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_ZERO ) != 0 ) {
        /* 指定 */

        /* フィールド幅埋め文字変更 */
        pad = '0';
    }

    /* 左寄せフラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_LEFT ) == 0 ) {
        /* 未指定 */

        /* フィールド幅埋め */
        for ( idx = 0; idx < ( ( int ) pInfo->width - bufferLength ); idx++ ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pad );
        }

        /* バッファ出力 */
        for ( idx = 0; idx < bufferLength; idx++ ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pBuffer[ idx ] );
        }

    } else {
        /* 指定 */

        /* バッファ出力 */
        for ( idx = 0; idx < bufferLength; idx++ ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pBuffer[ idx ] );
        }

        /* フィールド幅埋め */
        for ( idx = 0; idx < ( ( int ) pInfo->width - bufferLength ); idx++ ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pad );
        }
    }

    return;
}


/******************************************************************************/
/**
 * @brief           バッファ出力(反転)
 * @details         変換情報pInfoをもとにバッファpBufferからバッファ長
 *                  bufferLength分を反転して出力先文字列pStrに出力する。
 *
 * @param[out]      *pStr        出力先文字列
 * @param[in]       size         出力先文字列サイズ
 * @param[in,out]   *pWriteIdx   書込みインデックス
 * @param[in]       *pBuffer     バッファ
 * @param[in]       bufferLength バッファ長
 * @param[in]       *pInfo       変換情報
 */
/******************************************************************************/
static void PutBufferReverse( char          *pStr,
                              size_t        size,
                              int           *pWriteIdx,
                              char          *pBuffer,
                              int           bufferLength,
                              ConvertInfo_t *pInfo        )
{
    int  idx;   /* インデックス       */
    char pad;   /* フィールド埋め文字 */

    /* 初期化 */
    idx = 0;
    pad = ' ';

    /* 0フラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_ZERO ) != 0 ) {
        /* 指定 */

        /* フィールド幅埋め文字変更 */
        pad = '0';
    }

    /* 左寄せフラグ指定判定 */
    if ( ( pInfo->flag & ANALYZE_FLAG_LEFT ) == 0 ) {
        /* 未指定 */

        /* フィールド幅埋め */
        for ( idx = ( int ) pInfo->width - bufferLength; idx > 0; idx-- ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pad );
        }

        /* バッファ出力 */
        for ( idx = bufferLength - 1; idx >= 0; idx-- ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pBuffer[ idx ] );
        }

    } else {
        /* 指定 */

        /* バッファ出力 */
        for ( idx = bufferLength - 1; idx >= 0; idx-- ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pBuffer[ idx ] );
        }

        /* フィールド幅埋め */
        for ( idx = ( int ) pInfo->width - bufferLength; idx > 0; idx-- ) {
            PutChar( pStr, size, ( *pWriteIdx )++, pad );
        }
    }

    return;
}


/******************************************************************************/
/**
 * @brief           1文字出力
 * @details         書込みインデックスidxが出力先サイズ上限n以上でなければ、出
 *                  力先文字列pStrの書込みインデックスidxの位置に文字cを書き込
 *                  む。
 *
 * @param[out]      *pStr 出力先文字列
 * @param[in]       size  出力先文字列サイズ上限
 * @param[in]       idx   書込みインデックス
 * @param[in]       c     書込み文字
 */
/******************************************************************************/
static inline void PutChar( char   *pStr,
                            size_t size,
                            int    idx,
                            char   c    )
{
    /* サイズ判定 */
    if ( idx < size ) {
        /* 書込み可 */

        pStr[ idx ] = c;
    }

    return;
}


/******************************************************************************/
