/******************************************************************************/
/* src/include/MLib/MLibSplit.h                                               */
/*                                                                 2019/01/13 */
/* Copyright (C) 2019 Mochi                                                   */
/* https://github.com/MasterMochi/MLib.git                                    */
/******************************************************************************/
#ifndef _MLIB_SPLIT_H_
#define _MLIB_SPLIT_H_
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stddef.h>

/* ライブラリヘッダ */
#include "MLib.h"


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/* エラー番号 */
#define MLIB_SPLIT_ERR_NONE  ( 0x00000000 ) /**< エラー無し             */
#define MLIB_SPLIT_ERR_PARAM ( 0x00000001 ) /**< パラメータエラー       */
#define MLIB_SPLIT_ERR_NOMEM ( 0x00000002 ) /**< メモリ不足エラー       */
#define MLIB_SPLIT_ERR_NOIDX ( 0x00000003 ) /**< インデックス無効エラー */

/* 文字列分割ハンドル */
typedef struct {
    size_t   num;       /**< 分割文字列数                 */
    uint32_t idx;       /**< 操作中分割文字列インデックス */
    char     **ppStr;   /**< 分割文字列ポインタ配列参照   */
    char     *pStrs;    /**< 分割文字列格納先参照         */
    char     data[];    /**< データ領域                   */
} MLibSplitHandle_t;


/******************************************************************************/
/* グローバル関数プロトタイプ宣言                                             */
/******************************************************************************/
/* 分割文字列取得 */
extern MLibRet_t MLibSplitGet( MLibSplitHandle_t *pHandle,
                               uint32_t          idx,
                               char              **ppStr,
                               uint32_t          *pErrNo   );
/* 次分割文字列取得 */
extern MLibRet_t MLibSplitGetNext( MLibSplitHandle_t *pHandle,
                                   char              **ppStr,
                                   uint32_t          *pErrNo   );
/* 分割文字列数取得 */
extern MLibRet_t MLibSplitGetNum( MLibSplitHandle_t *pHandle,
                                  size_t            *pNum,
                                  uint32_t          *pErrNo   );
/* 文字列分割(区切り文字) */
extern MLibRet_t MLibSplitInitByDelimiter( MLibSplitHandle_t **ppHandle,
                                           const char        *pStr,
                                           size_t            length,
                                           char              delimiter,
                                           uint32_t          *pErrNo     );
/* 文字列分割終了処理 */
extern MLibRet_t MLibSplitTerm( MLibSplitHandle_t **ppHandle,
                                uint32_t          *pErrNo     );


/******************************************************************************/
#endif
