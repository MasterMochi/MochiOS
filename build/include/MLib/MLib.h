/******************************************************************************/
/* src/include/MLib/MLib.h                                                    */
/*                                                                 2019/01/13 */
/* Copyright (C) 2017-2019 Mochi                                              */
/* https://github.com/MasterMochi/MLib.git                                    */
/******************************************************************************/
#ifndef _MLIB_H_
#define _MLIB_H_
/******************************************************************************/
/* インクルード                                                               */
/******************************************************************************/
/* 標準ヘッダ */
#include <stdint.h>


/******************************************************************************/
/* 定義                                                                       */
/******************************************************************************/
/* 関数戻り値定義 */
#define MLIB_SUCCESS (  0 )     /**< 正常終了 */
#define MLIB_FAILURE ( -1 )     /**< 異常終了 */

/** アライメント計算マクロ */
#define MLIB_ALIGN( _VALUE, _ALIGNMENT )                                  \
    ( ( ( _VALUE ) + ( ( _ALIGNMENT ) - 1 ) ) & ~( ( _ALIGNMENT ) - 1 ) )

/** フラグ判定マクロ */
#define MLIB_HAVE_FLAG( _VALUE, _FLAG )         \
    ( ( ( _VALUE ) & ( _FLAG ) ) == ( _FLAG ) )

/** 最大値取得マクロ */
#define MLIB_MAX( _VALUE1, _VALUE2 )                          \
    ( ( _VALUE1 ) > ( _VALUE2 ) ? ( _VALUE1 ) : ( _VALUE2 ) )
/** 最小値取得マクロ */
#define MLIB_MIN( _VALUE1, _VALUE2 )                          \
    ( ( _VALUE1 ) < ( _VALUE2 ) ? ( _VALUE1 ) : ( _VALUE2 ) )

/** 配列エントリ数マクロ */
#define MLIB_NUMOF( _ARRAY )                       \
    ( sizeof ( _ARRAY ) / sizeof ( _ARRAY[ 0 ] ) )

/** 代入マクロ(非NULL条件) */
#define MLIB_SET_IFNOT_NULL( _PTR, _VALUE ) \
    {                                       \
        if ( ( _PTR ) != NULL ) {           \
            *( _PTR ) = ( _VALUE );         \
        }                                   \
    }

/** 関数戻り値型 */
typedef int32_t MLibRet_t;


/******************************************************************************/
#endif
