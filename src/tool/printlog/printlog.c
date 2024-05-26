/******************************************************************************/
/*                                                                            */
/* src/tool/printlog/printlog.c                                               */
/*                                                                 2024/05/03 */
/* Copyright (C) 2024 Mochi.                                                  */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* カーネルヘッダ */
#include <Cmn.h>


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/** アボートマクロ */
#define ABORT( ... )                    \
    {                                   \
        /* エラー出力 */                \
        fprintf( stderr, __VA_ARGS__ ); \
        fprintf( stderr, "\n" );        \
                                        \
        /* USAGE出力 */                 \
        printUsage( EXIT_FAILURE );     \
    }

/** ログ型 */
typedef struct {
    uint32_t logNo;     /**< ログ番号     */
    uint16_t moduleId;  /**< モジュールID */
    uint16_t lineNo;    /**< 行番号       */
    char     str[ 0 ];  /**< 文字列       */
} memLog_t;

/** モジュールID変換型 */
typedef struct {
    uint16_t moduleId;  /**< モジュールID */
    char     str[ 9 ];  /**< モジュール名 */
} vramConv_t;


/******************************************************************************/
/* ローカル関数宣言                                                           */
/******************************************************************************/
static const char *ConvModuleId( uint16_t moduleId );
static void printUsage( int status );


/******************************************************************************/
/* 静的グローバル変数定義                                                     */
/******************************************************************************/
/** モジュールID変換テーブル */
static const vramConv_t gConvTbl[] = {
    { CMN_MODULE_INIT_MAIN,      "INI-MAIN" },   /* 初期化制御(メイン)       */
    { CMN_MODULE_DEBUG_MAIN,     "DBG-MAIN" },   /* デバッグ制御(メイン)     */
    { CMN_MODULE_DEBUG_MEM,      "DBG-MEM " },   /* デバッグ制御(メモリ)     */
    { CMN_MODULE_DEBUG_VRAM,     "DBG-VRAM" },   /* デバッグ制御(VRAM)       */
    { CMN_MODULE_MEMMNG_MAIN,    "MEM-MAIN" },   /* メモリ管理(メイン)       */
    { CMN_MODULE_MEMMNG_SGMT,    "MEM-SGMT" },   /* メモリ管理(セグメント)   */
    { CMN_MODULE_MEMMNG_AREA,    "MEM-AREA" },   /* メモリ管理(領域)         */
    { CMN_MODULE_MEMMNG_PAGE,    "MEM-PAGE" },   /* メモリ管理(ページ)       */
    { CMN_MODULE_MEMMNG_CTRL,    "MEM-CTRL" },   /* メモリ管理(制御)         */
    { CMN_MODULE_MEMMNG_MAP,     "MEM-MAP " },   /* メモリ管理(マップ)       */
    { CMN_MODULE_MEMMNG_PHYS,    "MEM-PHYS" },   /* メモリ管理(物理)         */
    { CMN_MODULE_MEMMNG_IO,      "MEM-I/O " },   /* メモリ管理(I/O)          */
    { CMN_MODULE_MEMMNG_VIRT,    "MEM-VIRT" },   /* メモリ管理(仮想)         */
    { CMN_MODULE_MEMMNG_HEAP,    "MEM-HEAP" },   /* メモリ管理(ヒープ)       */
    { CMN_MODULE_TASKMNG_MAIN,   "TSK-MAIN" },   /* タスク管理(メイン)       */
    { CMN_MODULE_TASKMNG_TSS,    "TSK-TSS " },   /* タスク管理(TSS)          */
    { CMN_MODULE_TASKMNG_SCHED,  "TSK-SCHD" },   /* タスク管理(スケジューラ) */
    { CMN_MODULE_TASKMNG_TASK,   "TSK-TASK" },   /* タスク管理(タスク)       */
    { CMN_MODULE_TASKMNG_ELF,    "TSK-ELF " },   /* タスク管理(ELFローダ)    */
    { CMN_MODULE_TASKMNG_PROC,   "TSK-PROC" },   /* タスク管理(プロセス)     */
    { CMN_MODULE_TASKMNG_NAME,   "TSK-NAME" },   /* タスク管理(名前管理)     */
    { CMN_MODULE_TASKMNG_THREAD, "TSK-THRD" },   /* タスク管理(スレッド)     */
    { CMN_MODULE_INTMNG_MAIN,    "INT-MAIN" },   /* 割込管理(メイン)         */
    { CMN_MODULE_INTMNG_PIC,     "INT-PIC " },   /* 割込管理(PIC)            */
    { CMN_MODULE_INTMNG_IDT,     "INT-IDT " },   /* 割込管理(IDT)            */
    { CMN_MODULE_INTMNG_HDL,     "INT-HDL " },   /* 割込管理(ハンドラ)       */
    { CMN_MODULE_INTMNG_CTRL,    "INT-CTRL" },   /* 割込管理(ハードウェア)   */
    { CMN_MODULE_TIMERMNG_MAIN,  "TIM-MAIN" },   /* タイマ管理(メイン)       */
    { CMN_MODULE_TIMERMNG_CTRL,  "TIM-CTRL" },   /* タイマ管理(制御)         */
    { CMN_MODULE_TIMERMNG_PIT,   "TIM-PIT " },   /* タイマ管理(PIT)          */
    { CMN_MODULE_ITCCTRL_MAIN,   "ITC-MAIN" },   /* タスク間通信制御(メイン) */
    { CMN_MODULE_ITCCTRL_MSG,    "ITC-MSG " },   /* タスク間通信制御(ﾒｯｾｰｼﾞ) */
    { CMN_MODULE_IOCTRL_MAIN,    "IOC-MAIN" },   /* 入出力制御(メイン)       */
    { CMN_MODULE_IOCTRL_PORT,    "IOC-PORT" },   /* 入出力制御(I/Oポート)    */
    { CMN_MODULE_IOCTRL_MEM,     "IOC-MEM " },   /* 入出力制御(I/Oメモリ)    */
    { 0,                         "UNKNOWN " }  };/* 終端                     */


/******************************************************************************/
/* グローバル関数定義                                                         */
/******************************************************************************/
int main( int  argNum,
          char *pArg[] )
{
    int      fd;
    char     c;
    ssize_t  readSize;
    uint32_t bootFlag;
    memLog_t header;

    /* 引数チェック */
    if ( argNum != 2 ) {
        /* 不正 */
        printUsage( EXIT_FAILURE );
    }

    /* ログファイルオープン */
    fd = open( pArg[ 1 ], O_RDONLY );

    /* オープン結果判定 */
    if ( fd == -1 ) {
        /* 失敗 */

        /* アボート */
        ABORT(
            "ERROR(%04u): Can't open %s. errno=%d.\n",
            __LINE__,
            pArg[ 1 ],
            errno
        );
    }

    /* 起動フラグ読込み */
    readSize = read( fd, &bootFlag, sizeof ( uint32_t ) );

    /* 読込み結果判定 */
    if ( readSize != sizeof ( uint32_t ) ) {
        /* 失敗 */

        /* アボート */
        ABORT(
            "ERROR(%04u): Can't read bootFlag. errno=%d.\n",
            __LINE__,
            errno
        );
    }

    /* bootFlag出力 */
    printf( "bootFlag: 0x%X\n", bootFlag );

    /* ログ終端最後まで繰り返す */
    while ( true ) {
        /* ヘッダ読込み */
        readSize = read( fd, &header, sizeof ( memLog_t ) );

        /* 読込み結果判定 */
        if ( readSize != sizeof ( memLog_t ) ) {
            /* 失敗 */

            break;
        }

        /* ログ有無判定 */
        if ( header.lineNo == 0 ) {
            /* ログ無し */

            break;
        }

        /* ヘッダ出力 */
        printf(
            "\e[32m%04d:%s:%04u \e[0m",
            header.logNo,
            ConvModuleId( header.moduleId ),
            header.lineNo
        );

        /* \0まで繰り返す */
        while ( true ) {
            /* ログ読込み */
            readSize = read( fd, &c, 1 );

            /* 読込み結果判定 */
            if ( readSize != 1 ) {
                /* 失敗 */

                /* アボート */
                ABORT( "ERROR(%04u): Can't read. errno=%d.\n", __LINE__, errno );
            }

            /* 終端判定 */
            if ( c == '\0' ) {
                /* 改行出力 */
                printf( "\n" );
                break;
            }

            /* 出力 */
            putchar( c );
        }
    }

    /* ログファイルクローズ */
    close( fd );

    return 0;
}


/******************************************************************************/
/* ローカル関数定義                                                           */
/******************************************************************************/
/******************************************************************************/
/**
 * @brief       モジュールID文字列変換
 * @details     モジュールIDを文字列に変換する。
 *
 * @param[in]   moduleId モジュールID
 *
 * @return      文字列を返す。
 */
/******************************************************************************/
static const char *ConvModuleId( uint16_t moduleId )
{
    uint32_t idx;

    /* 初期化 */
    idx = 0;

    /* 変換テーブルエントリ毎に繰り返す */
    while ( gConvTbl[ idx ].moduleId != 0 ) {
        /* モジュールID一致確認 */
        if ( gConvTbl[ idx ].moduleId == moduleId ) {
            /* 一致 */
            break;
        }

        /* 次のエントリ */
        idx++;
    }

    return gConvTbl[ idx ].str;
}


/******************************************************************************/
/**
 * @brief       USAGE出力
 * @details     USAGEを出力しプログラムを終了する。
 *
 * @param[in]   status 終了ステータス
 */
/******************************************************************************/
static void printUsage( int status )
{
    /* USAGE出力 */
    fprintf( stderr, "USAGE: printlog [FILE]\n" );

    /* 終了 */
    exit( status );
}


/******************************************************************************/

