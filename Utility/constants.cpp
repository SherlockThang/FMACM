// ****************************************************************************
// NOTICE
//
// This work was produced for the U.S. Government under Contract 693KA8-22-C-00001 
// and is subject to Federal Aviation Administration Acquisition Management System 
// Clause 3.5-13, Rights In Data-General, Alt. III and Alt. IV (Oct. 1996).
//
// The contents of this document reflect the views of the author and The MITRE 
// Corporation and do not necessarily reflect the views of the Federal Aviation 
// Administration (FAA) or the Department of Transportation (DOT). Neither the FAA 
// nor the DOT makes any warranty or guarantee, expressed or implied, concerning 
// the content or accuracy of these views.
//
// For further information, please contact The MITRE Corporation, Contracts Management 
// Office, 7515 Colshire Drive, McLean, VA 22102-7539, (703) 983-6000.
//
// 2022 The MITRE Corporation. All Rights Reserved.
// ****************************************************************************

#include "utility/constants.h"

const double aaesim::constants::BARO_ALT_ERR_GRAD[aaesim::constants::BARO_ALT_ERR_STEPS] = {
      0,
      0.036295,
      0.088049,
      0.088403,
      0.088811,
      0.091698,
      0.089787,
      0.092764,
      0.093378,
      0.091659,
      0.094773,
      0.093182,
      0.091675,
      0.092585,
      0.082016,
      0.074051,
      0.061977,
      0.054609,
      0.051689,
      0.024394,
      0.014118,
      0.011854,
      0.009689,
      0.001995,
      -0.005435,
      -0.012609,
      -0.017776,
      -0.017529,
      -0.017215,
      -0.018567,
      -0.016385,
      -0.015868,
      -0.015283,
      -0.016331,
      -0.013906,
      -0.016481,
      -0.01393,
      -0.012989,
      -0.015292,
      -0.014196,
      -0.014663,
      -0.013415,
      -0.012093,
      -0.012311,
      -0.012438,
      -0.012473,
      -0.012417,
      -0.010695,
      -0.013585,
      -0.010139,
      -0.011266,
      -0.012276,
      -0.010129,
      -0.010933,
      -0.010119,
      -0.009208,
      -0.009687,
      -0.008573,
      -0.008829,
      -0.008967,
      -0.007541,
      -0.007453,
      -0.005816,
      -0.005499,
      -0.003646,
      -0.004502,
      -0.00382,
      -0.003016,
      -0.002087,
      -0.002401,
      -0.001211,
      -0.001244,
      0.000209,
      -0.000867,
      0.000862,
      0.001413,
      0.000815,
      0.001679,
      0.002696,
      0.003867,
      0.003933,
      0.004175,
      0.004596,
      0.005196,
      0.004756,
      0.005728,
      0.006883,
      0.007034,
      0.007392,
      0.009127,
      0.009894,
      0.009722,
      0.00865,
      0.007839,
      0.006182,
      0.004814,
      0.001584,
      -0.000241,
      -0.002793,
      -0.005003,
      -0.006873,
      -0.00741,
      -0.007637,
      -0.008523,
      -0.009072,
      -0.009283,
      -0.011019,
      -0.010527,
      -0.010601,
      -0.011201,
      -0.011407,
      -0.012087,
      -0.011487,
      -0.012179,
      -0.01075,
      -0.012206,
      -0.012345,
      -0.011201,
      -0.011952,
      -0.010613,
      -0.011066
};

const double aaesim::constants::BARO_ALT_ERR_ALTITUDES[aaesim::constants::BARO_ALT_ERR_STEPS] = {
      0,
      4762.888,
      4908.52,
      5054.808,
      5201.752,
      5349.024,
      5497.28,
      5645.864,
      5795.104,
      5945.328,
      6095.88,
      6247.416,
      6399.936,
      6553.112,
      6708.584,
      6866.024,
      7026.088,
      7188.12,
      7351.464,
      7520.056,
      7691.272,
      7863.8,
      8037.64,
      8213.776,
      8392.208,
      8572.936,
      8755.632,
      8939.312,
      9123.976,
      9309.952,
      9496.584,
      9684.2,
      9872.8,
      10062.712,
      10253.28,
      10445.488,
      10638.352,
      10832.2,
      11027.688,
      11224.16,
      11421.944,
      11620.712,
      11820.464,
      12021.528,
      12223.904,
      12427.592,
      12632.592,
      12838.576,
      13046.528,
      13255.136,
      13465.384,
      13677.272,
      13890.144,
      14104.656,
      14320.48,
      14537.616,
      14756.392,
      14976.48,
      15198.208,
      15421.576,
      15646.256,
      15872.576,
      16100.208,
      16329.48,
      16560.064,
      16792.616,
      17026.808,
      17262.64,
      17500.112,
      17739.552,
      17980.632,
      18223.68,
      18468.368,
      18715.352,
      18963.976,
      19214.568,
      19467.456,
      19722.312,
      19979.136,
      20237.928,
      20499.016,
      20762.4,
      21028.08,
      21296.056,
      21566.656,
      21839.552,
      22114.744,
      22392.56,
      22673,
      22955.736,
      23241.096,
      23529.408,
      23821,
      24115.872,
      24414.352,
      24716.44,
      25022.792,
      25333.08,
      25647.632,
      25966.448,
      26289.528,
      26616.544,
      26947.496,
      27282.712,
      27622.192,
      27965.936,
      28314.6,
      28667.528,
      29025.048,
      29387.488,
      29754.848,
      30127.456,
      30504.984,
      30888.088,
      31276.112,
      31670.368,
      32070.528,
      32476.264,
      32888.56,
      33306.76,
      33731.848
};

const double aaesim::constants::BARO_ALT_ERROR[aaesim::constants::BARO_ALT_ERR_STEPS] = {
      0,
      172.86788,
      185.69067,
      198.623017,
      211.673242,
      225.177802,
      238.489295,
      252.272478,
      266.208244,
      279.977651,
      294.245917,
      308.366412,
      322.348685,
      336.53044,
      349.281572,
      360.940138,
      370.860393,
      379.70877,
      388.1519,
      392.2646,
      394.681905,
      396.727044,
      398.411465,
      398.762828,
      397.793027,
      395.514178,
      392.266631,
      389.046983,
      385.868072,
      382.414997,
      379.357122,
      376.380069,
      373.497746,
      370.396335,
      367.746325,
      364.578491,
      361.891919,
      359.37402,
      356.384526,
      353.595503,
      350.695363,
      348.028884,
      345.613193,
      343.137807,
      340.620633,
      338.079965,
      335.534527,
      333.331456,
      330.506333,
      328.391189,
      326.022527,
      323.42132,
      321.265055,
      318.919714,
      316.735824,
      314.736451,
      312.61723,
      310.73038,
      308.772719,
      306.769701,
      305.075414,
      303.388628,
      302.064797,
      300.804093,
      299.963429,
      298.916493,
      298.021768,
      297.310559,
      296.815036,
      296.240255,
      295.948198,
      295.645798,
      295.696989,
      295.482746,
      295.697107,
      296.051235,
      296.257456,
      296.685305,
      297.377579,
      298.378389,
      299.405209,
      300.504947,
      301.72599,
      303.118281,
      304.405386,
      305.968561,
      307.862834,
      309.817077,
      311.8901,
      314.470738,
      317.293949,
      320.096912,
      322.619134,
      324.930569,
      326.77574,
      328.229859,
      328.714974,
      328.640115,
      327.761446,
      326.166437,
      323.946038,
      321.522869,
      318.995429,
      316.13831,
      313.058432,
      309.867301,
      306.025266,
      302.30982,
      298.519911,
      294.460278,
      290.269816,
      285.765962,
      281.429143,
      276.763209,
      272.591951,
      267.779643,
      262.839632,
      258.294975,
      253.367165,
      248.928873,
      244.224815
};

