/******************************************************************************/
/*                                                                            */
/* src/lib/libc/stdlib/free.c                                                 */
/*                                                                 2021/11/21 */
/* Copyright (C) 2018-2021 Mochi.                                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stddef.h>
#include <stdint.h>

/* ライブラリヘッダ */
#include <libmk.h>
#include <MLib/MLibList.h>

/* モジュール内ヘッダ */
#include "malloc.h"


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/


/******************************************************************************/
/* ローカル関数宣言                                                           */
/******************************************************************************/
/* 未使用メモリ領域リスト挿入 */
static void InsertFreeList( mallocArea_t *pArea );


/******************************************************************************/
/* モジュール外向けグローバル関数定義                                         */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief       free
 * @details     標準Cライブラリのfree関数。malloc関数によって割り当てたメモリ領
 *              域を解放する。
 *
 * @param[in]   *ptr メモリ解放アドレス
 */
/******************************************************************************/
void free( void *ptr )
{
    mallocArea_t *pArea;    /* メモリ領域 */

    /* アドレスチェック */
    if ( ptr == NULL ) {
        /* 不正 */

        return;
    }

    /* 初期化 */
    pArea = ( mallocArea_t * ) ( ptr - offsetof( mallocArea_t, area ) );

    /* ロック */
    MLibSpinLock( &gLock, NULL );

    /* 使用中メモリ領域リストから削除 */
    MLibListRemove( &gUsedList, &( pArea->node ) );

    /* ブレイクポイント変更判定 */
    if ( pgBreakPoint == ( ptr + pArea->size ) ) {
        /* ブレイクポイント変更有り */

        /* ブレイクポイント減算 */
        LibMkProcSetBreakPoint(
            - ( sizeof ( mallocArea_t ) + pArea->size ),
            &pgBreakPoint,
            NULL                                         );

    } else {
        /* ブレイクポイント変更無し */

        /* 未使用メモリ領域リストに挿入 */
        InsertFreeList( pArea );
    }

    /* アンロック */
    MLibSpinUnlock( &gLock, NULL );

    return;
}


/******************************************************************************/
/* モジュール内向けグローバル関数定義                                         */
/******************************************************************************/


/******************************************************************************/
/* ローカル関数定義                                                           */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief       未使用メモリ領域リスト挿入
 * @details     未使用メモリ領域を未使用メモリ領域リストの適切な位置に挿入する。
 *              必要があれば前後の未使用メモリ領域と結合する。
 *
 * @param[in]   pArea 未使用メモリ領域
 */
/******************************************************************************/
static void InsertFreeList( mallocArea_t *pArea )
{
    mallocArea_t *pPrevArea;    /* 前メモリ領域 */
    mallocArea_t *pNextArea;    /* 次メモリ領域 */

    /* 初期化 */
    pPrevArea = NULL;
    pNextArea = NULL;

    do {
        /* シフト */
        pPrevArea = pNextArea;

        /* 次未使用メモリ領域情報取得 */
        pNextArea = ( mallocArea_t * )
                    MLibListGetNextNode( &gFreeList,
                                         ( MLibListNode_t * ) pPrevArea );

        /* 取得結果判定 */
        if ( pNextArea == NULL ) {
            /* メモリ領域無し */

            break;
        }

    /* 挿入位置判定 */
    } while ( !( ( pPrevArea < pArea             ) &&
                 (             pArea < pNextArea )    ) );

    /* 次ノード有無判定 */
    if ( pNextArea != NULL ) {
        /* 有り */

        /* 次ノード隣接判定 */
        if ( ( ( uint32_t ) pArea->area + pArea->size ) ==
             ( ( uint32_t ) pNextArea                 )    ) {
            /* 隣接 */

            /* 次ノード結合 */
            pArea->size += sizeof ( mallocArea_t ) + pNextArea->size;

            /* 次ノード削除 */
            MLibListRemove( &gFreeList, &( pNextArea->node ) );
        }
    }

    /* 前ノード有無判定 */
    if ( pPrevArea != NULL ) {
        /* 有り */

        /* 前ノード隣接判定 */
        if ( ( ( uint32_t ) pPrevArea->area + pPrevArea->size ) ==
             ( ( uint32_t ) pArea                             )    ) {
            /* 隣接 */

            /* 前ノード結合 */
            pPrevArea->size += sizeof ( mallocArea_t ) + pArea->size;

            return;
        }

        /* 挿入 */
        MLibListInsertNext( &gFreeList,
                            &( pPrevArea->node ),
                            &( pArea->node )      );

    } else {
        /* 無し */

        /* 挿入 */
        MLibListInsertHead( &gFreeList, &( pArea->node ) );
    }

    return;
}


/******************************************************************************/

