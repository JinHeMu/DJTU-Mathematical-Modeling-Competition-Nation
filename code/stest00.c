/*============================================================================
*
*    名称：stest00.c
*
*    目的：测试 'solpos.c' 中的太阳位置算法。
*
*        S_solpos
*            输入：     年份，一年中的天数，小时，分钟，秒，纬度，
*                        经度，时区
*
*            可选参数：气压  默认值 1013.0（标准气压）
*                        温度  默认值   10.0（标准温度）
*                        倾斜角  默认值    0.0（水平面）
*                        方位角  默认值  180.0（朝南面板）
*                        月份   （如果关闭了 S_DOY 函数）
*                        日     （ "             "             "     ）
*
*            输出：    大气质量，大气压力修正，方位角，入射角余弦，太阳高度余弦，日，一年中的天数，
*                        太阳高度，太阳辐照度，太阳辐照度（法线面），倾斜面太阳辐照度，月份，地球轨道参数，
*                        日出角修正，日出时间，日落时间，日出时间修正，太阳高度修正
*
*       S_init        （可选的输入参数结构初始化）
*           输入：     结构体 posdata*
*           输出：    结构体 posdata*
*
*                     （注意：初始化 S_solpos 的必需输入参数（上述）为超出范围的条件，强制用户提供参数；
*                      初始化 S_solpos 的可选输入参数（上述）为标称值。）
*
*      S_decode       （解码 S_solpos 返回的结果码的实用工具，可选）
*           输入：     long int S_solpos 返回的值，结构体 posdata*
*           输出：    输出到 stderr 的文本
*
*
*        所有变量都定义为 'solpos00.h' 中的 posdata 结构的成员。
*
*    用法：
*         在调用程序中，除了其他 'include' 语句之外，插入：
*
*              #include "solpos00.h"
*
*    Martin Rymes
*    National Renewable Energy Laboratory
*    1998 年 3 月 25 日
*
*    2001 年 3 月 28 日修订：SMW 更改了基准数字，以反映对 solpos00.c 进行的 2001 年 2 月更改
*
*----------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "solpos00.h"     /* <-- 这是我提到的 'include' */

int main()
{
    struct posdata pd, *pdat; /* 声明一个 posdata 结构和一个指向它的指针
                               （如果需要，也可以使用 malloc 动态分配结构） */
    long retval;              /* 用于捕获 S_solpos 返回的代码 */


    /**************  开始演示程序 **************/

    pdat = &pd; /* 为方便起见指向结构 */

    /* 初始化结构为默认值（只有当在调用代码中初始化了所有输入参数时才是可选的，
       在此示例中没有初始化）。 */

    S_init(pdat);

    /* 我们以亚特兰大，乔治亚州为例 */

    pdat->longitude = -84.43;  /* 请注意，纬度和经度以小数度表示，而不是度/分/秒 */
    pdat->latitude = 33.65;    /* 东部时区，尽管经度表明是中部时区。我们使用他们使用的时区。
                                 不要调整夏令时。 */

    pdat->year = 1999;         /* 年份是 1999 年。 */
    pdat->daynum = 203;        /* 7 月 22 日，一年中的第 203 天（算法会补偿闰年，所以只需计算天数） */

    /* 当天时间（标准时间）为 9:45:37 */

    pdat->hour = 9;
    pdat->minute = 45;
    pdat->second = 37;

    /* 假设温度为 27 摄氏度，气压为 1006 毫巴。
       温度用于大气折射修正，气压用于折射修正和压力修正的大气质量。 */

    pdat->temp = 27.0;
    pdat->press = 1006.0;

    /* 最后，我们将假设您有一个朝东南方向的平坦表面，倾斜角与纬度相同。 */

    pdat->tilt = pdat->latitude;  /* 与纬度相同的倾斜 */
    pdat->aspect = 135.0;       /* 135 度 = 东南 */

    printf("\n");
    printf("***** 测试 S_solpos: *****\n");
    printf("\n");

    retval = S_solpos(pdat);  /* 调用 S_solpos 函数 */
    S_decode(retval, pdat);    /* 一定要查看返回代码！ */

    /* 现在查看结果并与 NREL 基准进行比较 */

    printf("请注意，您的最后一位小数值可能会有所不同\n");
    printf("根据您的计算机的浮点存储和编译器的数学算法。如果您同意\n");
    printf("NREL 的至少 5 位有效数字的值，请假定它有效。\n\n");

    printf("请注意，S_solpos 已返回输入天数和月份。\n");
    printf("当配置为这样做时，S_solpos 将反转此输入/输出关系，接受\n");
    printf("月份和日期作为输入，并在 daynum 变量中返回一年中的天数。\n");
    printf("\n");
    printf("NREL    -> 1999.07.22，第 203 天，返回值 0，大气质量 amass 1.335752，折射压力 ampress 1.326522\n");
    printf("SOLTEST -> %d.%0.2d.%0.2d，第 %d 天，返回值 %ld，大气质量 amass %f，折射压力 ampress %f\n",
           pdat->year, pdat->month, pdat->day, pdat->daynum,
           retval, pdat->amass, pdat->ampress);
    printf("NREL    -> 方位角 azim 97.032875，余弦入射角 cosinc 0.912569，太阳高度角 elevref 48.409931\n");
    printf("SOLTEST -> 方位角 azim %f，余弦入射角 cosinc %f，太阳高度角 elevref %f\n",
           pdat->azim, pdat->cosinc, pdat->elevref);
    printf("NREL    -> 太阳辐射 etr 989.668518，太阳辐射 etrn 1323.239868，倾斜面太阳辐射 etrtilt 1207.547363\n");
    printf("SOLTEST -> 太阳辐射 etr %f，太阳辐射 etrn %f，倾斜面太阳辐射 etrtilt %f\n",
           pdat->etr, pdat->etrn, pdat->etrtilt);
    printf("NREL    -> 倾斜面效益 prime 1.037040，阴影带校正因子 sbcf 1.201910，日出时间 sunrise 347.173431\n");
    printf("SOLTEST -> 倾斜面效益 prime %f，阴影带校正因子 sbcf %f，日出时间 sunrise %f\n",
           pdat->prime, pdat->sbcf, pdat->sretr);
    printf("NREL    -> 日落时间 sunset 1181.111206，非校正效益 unprime 0.964283，折射的天顶角 zenref 41.590069\n");
    printf("SOLTEST -> 日落时间 sunset %f，非校正效益 unprime %f，折射的天顶角 zenref %f\n",
           pdat->ssetr, pdat->unprime, pdat->zenref);




/**********************************************************************/
/* S_solpos 配置示例，使用函数参数。

   选择最少的功能以满足您的需求可能会导致更快的执行。 S_GEOM（最小配置）和 S_ALL（计算所有变量）之间存在
   两倍的执行速度差异。 [S_DOY 实际上是最简单和最快速的配置，但它只进行日期转换并绕过了所有
   太阳几何计算。] 如果速度不是一个考虑因素，可以使用默认的 S_ALL 配置，通过调用 S_init 实现。

   位掩码在 S_solpos.h 中定义。 */

    /* 1）计算经过折射校正的太阳位置变量 */

    pdat->function = S_REFRAC;


    /* 2）计算阴影带校正因子 */

    pdat->function = S_SBCF;


    /* 3）同时选择上述两个功能（请注意，将两个位掩码“或”在一起以产生所需的结果）： */

    pdat->function = (S_REFRAC | S_SBCF);


    /* 4）修改上述配置以接受月份和日期，而不是一年中的天数。请注意，S_DOY
          （控制一年中的天数解释）必须反转，然后与其他功能代码“和”在一起，以将一年中的天数关闭。
          在关闭一年中的天数位后，S_solpos 期望以月和日期的形式接收日期输入。 */

    pdat->function = ((S_REFRAC | S_SBCF) & ~S_DOY);
    pdat->month = 7;
    pdat->day = 22;

    /*    还要注意，S_DOY 是唯一可以尝试以这种方式清除的功能位掩码：
          其他功能位掩码是多个掩码的复合，代表功能之间的相互依赖关系。
          关闭意外位将产生意外结果。如果在程序执行过程中需要更少的参数，
          您应该从零重新构建功能掩码，只使用所需的功能位掩码。 */


    /* 5）通过“或”位掩码将上述配置切换回一年中的天数： */

    pdat->function |= S_DOY;
    pdat->month = -99;  /* 现在忽略荒谬的月份和日期 */
    pdat->day = -99;    /* 并用正确的值覆盖它们 */

    /*    ... 然后再次切换回月份-日期等。：*/

    pdat->function &= ~S_DOY;


    /* 为了查看不同配置的效果，请将上述代码移到调用 S_solpos 之前，并检查结果。
       请注意，如果配置没有指定计算它们，则某些返回的参数未定义。
       因此，如果偏离了 S_ALL 默认配置，您必须跟踪正在计算的内容。 */


/**********************************************************************/
    /* 查看 S_solpos 返回代码

       在返回代码中，每个位表示范围内个别输入参数的错误。
       请参阅 S_solpos.h 中的位定义，以了解每个错误标志的位置。

       为了确保输入变量都在范围内，调用程序应始终查看 S_solpos 返回代码。
       在本示例中，函数 S_decode 通过检查代码并将解释写入标准错误输出来完成此任务。

       为了查看参数越界的效果，请将以下行移到调用 S_solpos 之前： */

    pdat->year = 99;  /* S_solpos 是否接受两位数年份？ */

    /* 这会导致 S_decode 输出有关输入年份值的描述性行。
       [此算法仅在 1950 年至 2050 年之间有效；因此，需要明确的四位数年份。
       如果日期以两位数格式表示，S_solpos 要求您将其转换为明确的四位数年份。] */

    /* S_decode（位于 solpos.c 文件中）可以用作构建自己的解码器的模板，
       以根据调用应用程序的要求处理错误。 */


/***********************************************************************/
    /* 访问单个功能 */

    /* S_solpos 被设计用于使用文档化的输入变量计算输出变量。
       但是，出于高级编程方便性的考虑，可以通过使用原始 L_ 掩码（与上面使用的复合 S_ 掩码不同）
       来访问 S_solpos 中的各个功能。但是要小心使用它们，因为调用程序必须提供功能所需的所有参数。
       由于许多这些变量在 S_solpos 中是内部精心创建的，所以单个功能可能没有边界检查；
       因此，您的调用程序必须在功能输入参数上执行所有验证。同样的原因，返回错误代码（retval）
       可能没有考虑所有相关的输入值，使功能容易受到计算错误或异常结束条件的影响。

       与上面的 S_ 掩码一样，将功能变量设置为 L_ 掩码。如果需要，可以对 L_ 掩码进行 OR 操作。

       solpos.h 文件包含了所有输出和过渡变量的列表，需要的 L_ 掩码以及单个功能内部计算所需的所有变量。

       例如，以下代码寻找仅 amass 值。它只调用大气质量函数，该函数除了折射的天顶角和压力外不需要任何参数。
       通常情况下，折射的天顶角是一个依赖于 S_solpos 内的许多其他功能计算得出的。
       但是通过使用 L_ 掩码，我们可以在外部简单地设置折射的天顶角并调用大气质量函数。 */

    pdat->function = L_AMASS;  /* 仅调用大气质量函数 */
    pdat->press = 1013.0;     /* 设置自己的压力 */

    /* 为本示例设置输出 */
    printf("原始大气质量循环：\n");
    printf("NREL    -> 37.92  5.59  2.90  1.99  1.55  1.30  1.15  1.06  1.02  1.00\n");
    printf("SOLTEST -> ");

    /* 循环遍历一系列在外部设置的折射的天顶角 */
    for (pdat->zenref = 90.0; pdat->zenref >= 0.0; pdat->zenref -= 10.0)
    {
        retval = S_solpos(pdat);        /* 调用 solpos */
        S_decode(retval, pdat);         /* retval 可能无效 */
        printf("%5.2f ", pdat->amass);   /* 打印大气质量 */
    }
    printf("\n");

    return 0;
}