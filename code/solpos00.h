/*============================================================================
*
*    名称:  solpos00.h
*
*    包含:
*        S_solpos     (计算太阳位置和辐射强度
*                      从时间和地点)
*            输入:     (从 posdata)
*                          年、月、日、小时、分钟、秒、
*                          纬度、经度、时区、间隔
*            可选:     (从 posdata; 默认值从 S_init 函数)
*                            压力   默认 1013.0（标准压力）
*                            温度   默认   10.0（标准温度）
*                            倾斜度  默认    0.0（水平面）
*                            方位   默认  180.0（面向南的面板）
*                            sbwid  默认    7.6（阴影带宽度）
*                            sbrad  默认   31.7（阴影带半径）
*                            sbsky  默认   0.04（阴影带天空因子）
*
*            输出:     (posdata) daynum、amass、ampress、azim、cosinc、
*                        elevref、etr、etrn、etrtilt、prime、
*                        sbcf、sretr、ssetr、unprime、zenref
*
*            返回:   长整型状态码（在 solpos.h 中定义）
*
*    用法:
*         在调用程序中，除了其他 'includes'，插入:
*
*              #include "solpos.h"
*
*    Martin Rymes
*    美国国家可再生能源实验室
*    1998年3月25日
*----------------------------------------------------------------------------*/

/*============================================================================
*
*     定义函数代码
*
*----------------------------------------------------------------------------*/
#define L_DOY    0x0001
#define L_GEOM   0x0002
#define L_ZENETR 0x0004
#define L_SSHA   0x0008
#define L_SBCF   0x0010
#define L_TST    0x0020
#define L_SRSS   0x0040
#define L_SOLAZM 0x0080
#define L_REFRAC 0x0100
#define L_AMASS  0x0200
#define L_PRIME  0x0400
#define L_TILT   0x0800
#define L_ETR    0x1000
#define L_ALL    0xFFFF

/*============================================================================
*
*     定义每个函数的位掩码
*
*----------------------------------------------------------------------------*/
#define S_DOY    ( L_DOY                          )
#define S_GEOM   ( L_GEOM   | S_DOY               )
#define S_ZENETR ( L_ZENETR | S_GEOM              )
#define S_SSHA   ( L_SSHA   | S_GEOM              )
#define S_SBCF   ( L_SBCF   | S_SSHA              )
#define S_TST    ( L_TST    | S_GEOM              )
#define S_SRSS   ( L_SRSS   | S_SSHA   | S_TST    )
#define S_SOLAZM ( L_SOLAZM | S_ZENETR            )
#define S_REFRAC ( L_REFRAC | S_ZENETR            )
#define S_AMASS  ( L_AMASS  | S_REFRAC            )
#define S_PRIME  ( L_PRIME  | S_AMASS             )
#define S_TILT   ( L_TILT   | S_SOLAZM | S_REFRAC )
#define S_ETR    ( L_ETR    | S_REFRAC            )
#define S_ALL    ( L_ALL                          )


/*============================================================================
*
*     枚举错误代码
*     (位位置从最低有效位到最高有效位)
*
*----------------------------------------------------------------------------*/
/*          代码          位       参数            范围
      ===============     ===  ===================  =============   */
enum {S_YEAR_ERROR,    /*  0   年份                1950 -  2050   */
    S_MONTH_ERROR,   /*  1   月份                    1 -    12   */
    S_DAY_ERROR,     /*  2   月份中的天数             1 -    31   */
    S_DOY_ERROR,     /*  3   年份中的天数             1 -   366   */
    S_HOUR_ERROR,    /*  4   小时                     0 -    24   */
    S_MINUTE_ERROR,  /*  5   分钟                   0 -    59   */
    S_SECOND_ERROR,  /*  6   秒钟                   0 -    59   */
    S_TZONE_ERROR,   /*  7   时区                -12 -    12   */
    S_INTRVL_ERROR,  /*  8   间隔（秒）             0 - 28800   */
    S_LAT_ERROR,     /*  9   纬度               -90 -    90   */
    S_LON_ERROR,     /* 10   经度             -180 -   180   */
    S_TEMP_ERROR,    /* 11   温度（摄氏度）  -100 -   100   */
    S_PRESS_ERROR,   /* 12   压力（毫巴）         0 -  2000   */
    S_TILT_ERROR,    /* 13   倾斜度             -90 -    90   */
    S_ASPECT_ERROR,  /* 14   方位              -360 -   360   */
    S_SBWID_ERROR,   /* 15   阴影带宽度（厘米）    1 -   100   */
    S_SBRAD_ERROR,   /* 16   阴影带半径（厘米）    1 -   100   */
    S_SBSKY_ERROR};  /* 17   阴影带天空因子   -1 -     1   */

struct posdata
{
    /***** 常见变量的字母表列表 *****/
    /* 每个注释以 1 列字母代码开头：
       I:  输入变量
       O:  输出变量
       T:  算法中使用的过渡变量，仅对太阳辐射建模者
           有兴趣，因为您可能是其中之一，
           可用于您，因为您
           可能是其中之一。

       功能列指示 solpos 中必须使用的子函数
       通过使用“function”参数来打开开关
       计算所需的输出变量。所有功能代码都是
       在 solpos.h 文件中定义的。默认值
       S_ALL开关计算所有输出变量。
       可以通过OR功能码来组合多个函数。
       例如，
       （S_TST | S_SBCF）。仅指定功能
       所需的输出变量可能允许 solpos
       更快地执行。

       S_DOY掩码可在
       表示为日数（daynum）的输入日期之间切换
       或作为月份和日期。要设置开关（到
       使用daynum输入），或作为或'd；要
       清除开关（使用月份和日期输入），
       反转并按位与。

       例如：
           pdat->function |= S_DOY（设置daynum输入）
           pdat->function &= ~S_DOY（设置月份和日期输入）

       无论使用哪种日期形式，S_solpos都将
       计算并返回另一种形式的变量。请参阅soltest.c程序以
       其他示例。 */

    /* 变量        I/O  功能    描述 */
    /* -------------  ----  ----------  ---------------------------------------*/

    int   day;       /* I/O: S_DOY      月的天数（5月27日=27，等等）
                                        solpos将默认情况下计算这个，
                                        或者根据S_DOY的设置，可以选择要求它作为输入
                                        日期输入为S_DOY功能开关的设置取决于S_DOY。
                                        */
    int   daynum;    /* I/O: S_DOY      天数（一年中的日期；2月1日=32）
                                        solpos默认情况下需要这个，但
                                        将根据S_DOY功能开关的设置选择性地从
                                        月和日期计算它。
                                        */
    int   function;  /* I:              选择所需输出函数的开关。 */
    int   hour;      /* I:              一天中的小时，0 - 23，默认 = 12 */
    int   interval;  /* I:              测量周期的间隔，以秒为单位。
                                        强制solpos使用
                                        间隔中点的时间和日期。假定输入时间（小时，
                                        分钟和秒）是测量的结束
                                        间隔。 */
    int   minute;    /* I:              小时的分钟，0 - 59，默认 = 0 */
    int   month;     /* I/O: S_DOY      月份编号（1月=1，2月=2等）
                                        solpos将默认情况下计算这个，
                                        或者根据S_DOY的设置，可以选择要求它作为输入
                                        日期输入为S_DOY功能开关的设置取决于S_DOY。
                                        */
    int   second;    /* I:              分钟的秒数，0 - 59，默认 = 0 */
    int   year;      /* I:              4位年份（不允许2位年份） */

    /***** 浮点数 *****/

    float amass;      /* O:  S_AMASS    相对光学气团 */
    float ampress;    /* O:  S_AMASS    压力校正的气团 */
    float aspect;     /* I:             面板表面的方位角（它的方向
                                        面向N=0、E=90、S=180、W=270，
                                        默认 = 180 */
    float azim;       /* O:  S_SOLAZM   太阳方位角：N=0、E=90、S=180、
                                        W=270 */
    float cosinc;     /* O:  S_TILT     太阳入射角的余弦值
                                        在面板上 */
    float coszen;     /* O:  S_REFRAC   修正后的太阳天顶角的余弦值 */
    float dayang;     /* T:  S_GEOM     天角（daynum*360/year-length）
                                        度 */
    float declin;     /* T:  S_GEOM     赤纬-在赤道太阳正午的天顶角，度NORTH */
    float eclong;     /* T:  S_GEOM     黄道经度，度 */
    float ecobli;     /* T:  S_GEOM     黄道的倾斜度 */
    float ectime;     /* T:  S_GEOM     黄道计算的时间 */
    float elevetr;    /* O:  S_ZENETR   太阳高度，无大气
                                        修正（= ETR） */
    float elevref;    /* O:  S_REFRAC   太阳高度角，
                                        从地平线度，折射 */
    float eqntim;     /* T:  S_TST      时间方程（TST - LMT），分钟 */
    float erv;        /* T:  S_GEOM     地球半径矢量
                                        （乘以太阳常数） */
    float etr;        /* O:  S_ETR      太阳辐射外大气（大气顶部）
                                        W/平方米的全球水平太阳
                                        辐射 */
    float etrn;       /* O:  S_ETR      太阳辐射外大气（大气顶部）
                                        W/平方米的直接法线太阳
                                        辐射 */
    float etrtilt;    /* O:  S_TILT     太阳辐射外大气（大气顶部）
                                        W/平方米的倾斜的全球辐射
                                        表面 */
    float gmst;       /* T:  S_GEOM     格林威治平均恒星时，小时 */
    float hrang;      /* T:  S_GEOM     时间角-从太阳正午到太阳小时，
                                        西度 */
    float julday;     /* T:  S_GEOM     1月1日2000日的儒略日
                                        减去2,400,000天（以恢复
                                        单精度） */
    float latitude;   /* I:             纬度，度北（南为负） */
    float longitude;  /* I:             经度，东经度（西经度为负） */
    float lmst;       /* T:  S_GEOM     当地的平均恒星时，度 */
    float mnanom;     /* T:  S_GEOM     平均偏差，度 */
    float mnlong;     /* T:  S_GEOM     平均经度，度 */
    float rascen;     /* T:  S_GEOM     赤经，度 */
    float press;      /* I:             表面压力，毫巴，用于
                                        折射校正和ampress */
    float prime;      /* O:  S_PRIME    归一化Kt，Kn等的因子 */
    float sbcf;       /* O:  S_SBCF     阴影带校正因子 */
    float sbwid;      /* I:             阴影带宽度（厘米） */
    float sbrad;      /* I:             阴影带半径（厘米） */
    float sbsky;      /* I:             阴影带天空因子 */
    float solcon;     /* I:             太阳常数（NREL使用1367 W/平方米） */
    float ssha;       /* T:  S_SRHA     太阳日落（/rise）时间角，度 */
    float sretr;      /* O:  S_SRSS     日出时间，距午夜分钟，
                                        地方，无折射 */
    float ssetr;      /* O:  S_SRSS     日落时间，距午夜分钟，
                                        地方，无折射 */
    float temp;       /* I:             环境干球温度，摄氏度，
                                        用于折射校正 */
    float tilt;       /* I:             平面倾斜度，与水平面的度数 */
    float timezone;   /* I:             时区，东经（西经为负）。
                                      美国：山地 = -7，中部 = -6，等等。 */
    float tst;        /* T:  S_TST      真太阳时间，距午夜分钟 */
    float tstfix;     /* T:  S_TST      真太阳时间 - 当地标准时间 */
    float unprime;    /* O:  S_PRIME    去标准化的因子Kt'，Kn'等 */
    float utime;      /* T:  S_GEOM     通用（格林威治）标准时间 */
    float zenetr;     /* T:  S_ZENETR   太阳天顶角，无大气
                                        修正（= ETR） */
    float zenref;     /* O:  S_REFRAC   太阳天顶角，从顶点度，
                                        折射 */
};

/* 对于希望访问单独功能的用户，下表列出了所有输出和过渡变量，
函数的L_掩码，以及该函数所需的所有输入变量。
将功能变量设置为L_掩码，将强制S_solpos只调用所需的函数。可以OR功能码如所需。

VARIABLE      Mask       Required Variables
---------  ----------  ---------------------------------------
 amass      L_AMASS    zenref, press
 ampress    L_AMASS    zenref, press
 azim       L_SOLAZM   elevetr, declin, latitude, hrang
 cosinc     L_TILT     azim, aspect, tilt, zenref, coszen,etrn
 coszen     L_REFRAC   elevetr, press, temp
 dayang     L_GEOM     All date, time, and location inputs
 declin     L_GEOM     All date, time, and location inputs
 eclong     L_GEOM     All date, time, and location inputs
 ecobli     L_GEOM     All date, time, and location inputs
 ectime     L_GEOM     All date, time, and location inputs
 elevetr    L_ZENETR   declin, latitude, hrang
 elevref    L_REFRAC   elevetr, press, temp
 eqntim     L_TST      hrang, hour, minute, second, interval
 erv        L_GEOM     All date, time, and location inputs
 etr        L_ETR      coszen, solcon, erv
 etrn       L_ETR      coszen, solcon, erv
 etrtilt    L_TILT     azim, aspect, tilt, zenref, coszen, etrn
 gmst       L_GEOM     All date, time, and location inputs
 hrang      L_GEOM     All date, time, and location inputs
 julday     L_GEOM     All date, time, and location inputs
 lmst       L_GEOM     All date, time, and location inputs
 mnanom     L_GEOM     All date, time, and location inputs
 mnlong     L_GEOM     All date, time, and location inputs
 rascen     L_GEOM     All date, time, and location inputs
 prime      L_PRIME    amass
 sbcf       L_SBCF     latitude, declin, ssha, sbwid, sbrad, sbsky
 ssha       L_SRHA     latitude, declin
 sretr      L_SRSS     ssha, tstfix
 ssetr      L_SRSS     ssha, tstfix
 tst        L_TST      hrang, hour, minute, second, interval
 tstfix     L_TST      hrang, hour, minute, second, interval
 unprime    L_PRIME    amass
 utime      L_GEOM     All date, time, and location inputs
 zenetr     L_ZENETR   declination, latitude, hrang
 zenref     L_REFRAC   elevetr, press, temp
 */


/*============================================================================
*    Long int function S_solpos，改编自NREL VAX太阳库
*
*    此函数根据地球上的日期、时间和
*    太阳位置和光强（理论上的最大太阳能）。
*    （DEFAULT值来自可选的S_posinit函数。）
*
*    需要：
*        日期和时间：
*            year
*            month（作为月份或日号）
*            day（作为月份或日号）
*            hour
*            minute
*            second
*            interval（间隔（秒））
*            timezone（小时）
*            longitude（度）（东经为正，西经为负）
*            latitude（度）（北纬为正，南纬为负）
*            temp（环境干球温度，摄氏度）
*            press（表面压力，毫巴）
*            tilt（平面倾斜度，度，为平，=90为垂直）
*            aspect（平面的方位，度，从北（=0，=360））
*            sbwid（阴影带宽度，厘米）
*            sbrad（阴影带半径，厘米）
*            sbsky（阴影带天空因子）
*
*            功能（选项）：
*            amass     相对光学质量
*            ampress   气团相对质量
*            azim      方位角（从北=0，逆时针为正）
*            cosinc    太阳入射角的余弦值
*            elevref   太阳高度角，从地平线度，折射
*            etr       太阳辐射外大气（大气顶部） W/sq m
*            etrn      太阳辐射外大气（大气顶部） W/sq m
*            etrtilt   太阳辐射外大气（大气顶部） W/sq m
*            sbcf      阴影带校正因子
*            sretr     无折射的日出时间
*            ssetr     无折射的日落时间
*            unprime   去标准化Kt, Kn等的因子
*            zenref    太阳高度角，从顶点度，折射
*
*----------------------------------------------------------------------------*/
