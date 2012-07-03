#include "AudioPhaseTable.h"

// common functions to both setting
unsigned Audio_computePhaseImpl(unsigned base_pitch, unsigned note_pitch, unsigned sampling_rate);


#ifdef AUDIO_USE_VARPHASE_TABLE

// Definition of tone phase
#define	D_AUDIO_VARPHASETABLE_START 107
#define D_AUDIO_VARPHASETABLE_END 192

static const unsigned gAudioVarPhaseTable[] = {
	15   ,    16,    17,    18,    19,    21,    22,    23,    25,    26,    28,    29,    31,    33,    35,    37,
	39   ,    42,    44,    47,    50,    53,    56,    59,    63,    66,    70,    75,    79,    84,    89,    94,
	100  ,   106,   112,   119,   126,   133,   141,   150,   159,   168,   178,   189,   200,   212,   225,   238,
	252  ,   267,   283,   300,   318,   337,   357,   378,   401,	 425,   450,   477,   505,   535,   567,   601,
	637  ,   674,   715,   757,   802,   850,   900,   954,  1011,  1071,  1135,  1202,  1274,  1349,  1430,  1515,
	1605 ,  1700,  1801,  1908,  2022,  2142,  2270,  2405,  2548,  2699,  2860,  3030,  3210,  3401,  3603,  3817,
	4044 ,  4285,  4540,  4810,  5096,  5399,  5720,  6060,  6420,  6802,  7206,
	0x1dd, // D_BASEPHASE ,
	505  ,   535,   567,   601,   637,   674,   715,   757,   802,   850,   900,   954,  1011,  1071,  1135,  1202,
	1274 ,  1349,  1430,  1515,  1605,  1700,  1801,  1908,  2022,  2142,  2270,  2405,  2548,  2699,  2860,  3030,
	3210 ,  3401,  3603,  3817,  4044,  4285,  4540,  4810,  5096,  5399,  5720,  6060,  6420,  6802,  7207,  7635,
	8089 ,  8570,  9080,  9620, 10192, 10798, 11440, 12121, 12841, 13605, 14414, 15271, 16179, 17141, 18161, 19240,
	20385, 21597, 22881, 24242, 25683, 27210, 28828, 30543, 32359, 34283, 36322, 38481, 40770, 43194, 45762, 48484,
	51367, 54421, 57657, 61086, 64718,
};

unsigned Audio_computePhase(unsigned base_pitch, unsigned note_pitch, unsigned sampling_rate) {
	int sign = ((int)note_pitch - (int)base_pitch) + D_AUDIO_VARPHASETABLE_START;
	unsigned phase = 0;
	
	if (sign < 0) {
		sign = 0;
	} else if (sign > D_AUDIO_VARPHASETABLE_END) {
		sign = D_AUDIO_VARPHASETABLE_END;
	}
	phase = (gAudioVarPhaseTable[sign] * 0x1d2 / 0x1dd);
	phase = (phase * sampling_rate ) >> 8;
	if (note_pitch < base_pitch) {
		phase >>= 4;
	}
	
	return phase;	
}

#else

#include "SpuDrv.h"

extern int fixExp(int a);

unsigned Audio_computePhase(unsigned base_pitch, unsigned note_pitch, unsigned sampling_rate) {
	const int POWER_ONE = 65536;
	const int LOG2_E = 94548; // 1.4426950408889634073599246810019 * POWER_ONE
	int sign = ((int)note_pitch - (int)base_pitch);
	long long int temp = 0;
	unsigned result = 0;
	unsigned phase = 0;
	
	phase = SAMPLERATE_2_PHASE(sampling_rate);

	if (sign != 0) {
		temp = POWER_ONE * sign / 12;
		temp *= POWER_ONE;
		temp /= LOG2_E;
		result = fixExp((int)temp);
	
		temp = result;
		temp *= phase;
		temp /= POWER_ONE;
		
		phase = (int)temp;
	}
	
	return phase;
}

#endif


#ifdef AUDIO_USE_PITCHBEND_TABLE

#define D_AUDIO_PITCHBEND_START 64
#define D_AUDIO_PITCHBEND_MAX_STEP 64

static const unsigned gAudioPitchBendTable_1[] = {
	15464, 15478, 15492, 15506, 15520, 15534, 15548, 15562, 15576, 15590, 15604, 15618, 15632, 15646, 15661, 15675,
	15689, 15703, 15717, 15731, 15746, 15760, 15774, 15788, 15803, 15817, 15831, 15845, 15860, 15874, 15888, 15903,
	15917, 15931, 15946, 15960, 15975, 15989, 16004, 16018, 16032, 16047, 16061, 16076, 16090, 16105, 16119, 16134,
	16149, 16163, 16178, 16192, 16207, 16222, 16236, 16251, 16266, 16280, 16295, 16310, 16324, 16339, 16354, 16369,
	16384,
	16398, 16413, 16428, 16443, 16458, 16472, 16487, 16502, 16517, 16532, 16547, 16562, 16577, 16592, 16607, 16622,
	16637, 16652, 16667, 16682, 16697, 16712, 16727, 16742, 16757, 16773, 16788, 16803, 16818, 16833, 16848, 16864,
	16879, 16894, 16909, 16925, 16940, 16955, 16970, 16986, 17001, 17016, 17032, 17047, 17063, 17078, 17093, 17109,
	17124, 17140, 17155, 17171, 17186, 17202, 17217, 17233, 17248, 17264, 17280, 17295, 17311, 17326, 17342, 17358
};

static const unsigned gAudioPitchBendTable_2[] = {
	14596, 14623, 14649, 14676, 14702, 14729, 14755, 14782, 14809, 14836, 14862, 14889, 14916, 14943, 14970, 14997,
	15024, 15051, 15079, 15106, 15133, 15160, 15188, 15215, 15243, 15270, 15298, 15325, 15353, 15381, 15409, 15437,
	15464, 15492, 15520, 15548, 15576, 15605, 15633, 15661, 15689, 15718, 15746, 15775, 15803, 15832, 15860, 15889,
	15918, 15946, 15975, 16004, 16033, 16062, 16091, 16120, 16149, 16178, 16208, 16237, 16266, 16296, 16325, 16354,
	16384,
	16414, 16443, 16473, 16503, 16533, 16562, 16592, 16622, 16652, 16682, 16713, 16743, 16773, 16803, 16834, 16864,
	16895, 16925, 16956, 16986, 17017, 17048, 17079, 17109, 17140, 17171, 17202, 17233, 17264, 17296, 17327, 17358,
	17390, 17421, 17452, 17484, 17516, 17547, 17579, 17611, 17643, 17674, 17706, 17738, 17770, 17802, 17835, 17867,
	17899, 17931, 17964, 17996, 18029, 18061, 18094, 18127, 18160, 18192, 18225, 18258, 18291, 18324, 18357, 18390
};

static const unsigned gAudioPitchBendTable_3[] = {
	13777, 13815, 13852, 13890, 13927, 13965, 14003, 14041, 14079, 14117, 14155, 14194, 14232, 14271, 14310, 14348,
	14387, 14426, 14465, 14505, 14544, 14583, 14623, 14663, 14702, 14742, 14782, 14822, 14862, 14903, 14943, 14984,
	15024, 15065, 15106, 15147, 15188, 15229, 15270, 15312, 15353, 15395, 15437, 15478, 15520, 15562, 15605, 15647,
	15689, 15732, 15775, 15817, 15860, 15903, 15946, 15990, 16033, 16076, 16120, 16164, 16208, 16251, 16296, 16340,
	16384,
	16428, 16473, 16518, 16562, 16607, 16652, 16697, 16743, 16788, 16834, 16879, 16925, 16971, 17017, 17063, 17109,
	17156, 17202, 17249, 17296, 17343, 17390, 17437, 17484, 17531, 17579, 17627, 17674, 17722, 17770, 17819, 17867,
	17915, 17964, 18013, 18061, 18110, 18160, 18209, 18258, 18308, 18357, 18407, 18457, 18507, 18557, 18607, 18658,
	18708, 18759, 18810, 18861, 18912, 18963, 19015, 19066, 19118, 19170, 19222, 19274, 19326, 19379, 19431, 19484
};

static const unsigned gAudioPitchBendTable_4[] = {
	13004, 13051, 13098, 13146, 13193, 13241, 13289, 13337, 13385, 13433, 13482, 13531, 13580, 13629, 13678, 13728,
	13777, 13827, 13877, 13927, 13978, 14028, 14079, 14130, 14181, 14232, 14284, 14335, 14387, 14439, 14491, 14544,
	14596, 14649, 14702, 14755, 14809, 14862, 14916, 14970, 15024, 15079, 15133, 15188, 15243, 15298, 15353, 15409,
	15464, 15520, 15576, 15633, 15689, 15746, 15803, 15860, 15918, 15975, 16033, 16091, 16149, 16208, 16266, 16325,
	16384,
	16443, 16503, 16562, 16622, 16682, 16743, 16803, 16864, 16925, 16986, 17048, 17109, 17171, 17233, 17296, 17358,
	17421, 17484, 17547, 17611, 17674, 17738, 17802, 17867, 17931, 17996, 18061, 18127, 18192, 18258, 18324, 18390,
	18457, 18524, 18591, 18658, 18725, 18793, 18861, 18929, 18998, 19066, 19135, 19205, 19274, 19344, 19414, 19484,
	19554, 19625, 19696, 19767, 19839, 19911, 19983, 20055, 20127, 20200, 20273, 20347, 20420, 20494, 20568, 20643
};

static const unsigned gAudioPitchBendTable_5[] = {
	12274, 12330, 12385, 12441, 12498, 12554, 12611, 12668, 12725, 12783, 12841, 12899, 12957, 13016, 13075, 13134,
	13193, 13253, 13313, 13373, 13433, 13494, 13555, 13617, 13678, 13740, 13802, 13865, 13927, 13990, 14054, 14117,
	14181, 14245, 14310, 14374, 14439, 14505, 14570, 14636, 14702, 14769, 14836, 14903, 14970, 15038, 15106, 15174,
	15243, 15312, 15381, 15450, 15520, 15591, 15661, 15732, 15803, 15875, 15946, 16018, 16091, 16164, 16237, 16310,
	16384,
	16458, 16533, 16607, 16682, 16758, 16834, 16910, 16986, 17063, 17140, 17218, 17296, 17374, 17452, 17531, 17611,
	17690, 17770, 17851, 17931, 18013, 18094, 18176, 18258, 18341, 18424, 18507, 18591, 18675, 18759, 18844, 18929,
	19015, 19101, 19187, 19274, 19361, 19449, 19537, 19625, 19714, 19803, 19893, 19983, 20073, 20164, 20255, 20347,
	20439, 20531, 20624, 20717, 20811, 20905, 21000, 21095, 21190, 21286, 21382, 21479, 21576, 21674, 21772, 21870
};

static const unsigned gAudioPitchBendTable_6[] = {
	11585, 11648, 11711, 11775, 11839, 11903, 11968, 12033, 12098, 12164, 12230, 12296, 12363, 12430, 12498, 12566,
	12634, 12702, 12771, 12841, 12910, 12981, 13051, 13122, 13193, 13265, 13337, 13409, 13482, 13555, 13629, 13703,
	13777, 13852, 13927, 14003, 14079, 14155, 14232, 14310, 14387, 14465, 14544, 14623, 14702, 14782, 14862, 14943,
	15024, 15106, 15188, 15270, 15353, 15437, 15520, 15605, 15689, 15775, 15860, 15946, 16033, 16120, 16208, 16296,
	16384,
	16473, 16562, 16652, 16743, 16834, 16925, 17017, 17109, 17202, 17296, 17390, 17484, 17579, 17674, 17770, 17867,
	17964, 18061, 18160, 18258, 18357, 18457, 18557, 18658, 18759, 18861, 18963, 19066, 19170, 19274, 19379, 19484,
	19590, 19696, 19803, 19911, 20019, 20127, 20237, 20347, 20457, 20568, 20680, 20792, 20905, 21019, 21133, 21247,
	21363, 21479, 21595, 21713, 21831, 21949, 22068, 22188, 22309, 22430, 22552, 22674, 22797, 22921, 23045, 23170
};

static const unsigned gAudioPitchBendTable_7[] = {
	10935, 11004, 11074, 11144, 11215, 11286, 11357, 11429, 11502, 11575, 11648, 11722, 11796, 11871, 11946, 12022,
	12098, 12175, 12252, 12330, 12408, 12486, 12566, 12645, 12725, 12806, 12887, 12969, 13051, 13134, 13217, 13301,
	13385, 13470, 13555, 13641, 13728, 13815, 13902, 13990, 14079, 14168, 14258, 14348, 14439, 14531, 14623, 14716,
	14809, 14903, 14997, 15092, 15188, 15284, 15381, 15478, 15576, 15675, 15775, 15875, 15975, 16076, 16178, 16281,
	16384,
	16488, 16592, 16697, 16803, 16910, 17017, 17125, 17233, 17343, 17452, 17563, 17674, 17786, 17899, 18013, 18127,
	18242, 18357, 18474, 18591, 18708, 18827, 18946, 19066, 19187, 19309, 19431, 19554, 19678, 19803, 19929, 20055,
	20182, 20310, 20439, 20568, 20699, 20830, 20962, 21095, 21228, 21363, 21498, 21634, 21772, 21910, 22048, 22188,
	22329, 22470, 22613, 22756, 22900, 23045, 23191, 23338, 23486, 23635, 23785, 23936, 24087, 24240, 24394, 24548
};

static const unsigned gAudioPitchBendTable_8[] = {
	10321, 10396, 10471, 10547, 10624, 10701, 10778, 10856, 10935, 11014, 11094, 11174, 11255, 11337, 11419, 11502,
	11585, 11669, 11754, 11839, 11925, 12011, 12098, 12186, 12274, 12363, 12453, 12543, 12634, 12725, 12818, 12910,
	13004, 13098, 13193, 13289, 13385, 13482, 13580, 13678, 13777, 13877, 13978, 14079, 14181, 14284, 14387, 14491,
	14596, 14702, 14809, 14916, 15024, 15133, 15243, 15353, 15464, 15576, 15689, 15803, 15918, 16033, 16149, 16266,
	16384,
	16503, 16622, 16743, 16864, 16986, 17109, 17233, 17358, 17484, 17611, 17738, 17867, 17996, 18127, 18258, 18390,
	18524, 18658, 18793, 18929, 19066, 19205, 19344, 19484, 19625, 19767, 19911, 20055, 20200, 20347, 20494, 20643,
	20792, 20943, 21095, 21247, 21401, 21556, 21713, 21870, 22028, 22188, 22349, 22511, 22674, 22838, 23004, 23170,
	23338, 23507, 23678, 23849, 24022, 24196, 24372, 24548, 24726, 24905, 25086, 25268, 25451, 25635, 25821, 26008
};

static const unsigned gAudioPitchBendTable_9[] = {
	9742 ,  9821,  9902,  9982, 10064, 10146, 10229, 10312, 10396, 10481, 10566, 10653, 10739, 10827, 10915, 11004,
	11094, 11185, 11276, 11368, 11460, 11554, 11648, 11743, 11839, 11935, 12033, 12131, 12230, 12330, 12430, 12532,
	12634, 12737, 12841, 12945, 13051, 13157, 13265, 13373, 13482, 13592, 13703, 13815, 13927, 14041, 14155, 14271,
	14387, 14505, 14623, 14742, 14862, 14984, 15106, 15229, 15353, 15478, 15605, 15732, 15860, 15990, 16120, 16251,
	16384,
	16518, 16652, 16788, 16925, 17063, 17202, 17343, 17484, 17627, 17770, 17915, 18061, 18209, 18357, 18507, 18658,
	18810, 18963, 19118, 19274, 19431, 19590, 19750, 19911, 20073, 20237, 20402, 20568, 20736, 20905, 21076, 21247,
	21421, 21595, 21772, 21949, 22128, 22309, 22491, 22674, 22859, 23045, 23233, 23423, 23614, 23806, 24001, 24196,
	24394, 24593, 24793, 24995, 25199, 25405, 25612, 25821, 26031, 26244, 26458, 26674, 26891, 27110, 27332, 27554
};

static const unsigned gAudioPitchBendTable_10[] = {
	9195 ,  9279,  9363,  9448,  9533,  9620,  9707,  9795,  9884,  9973, 10064, 10155, 10247, 10340, 10434, 10528,
	10624, 10720, 10817, 10915, 11014, 11114, 11215, 11317, 11419, 11523, 11627, 11733, 11839, 11946, 12055, 12164,
	12274, 12385, 12498, 12611, 12725, 12841, 12957, 13075, 13193, 13313, 13433, 13555, 13678, 13802, 13927, 14054,
	14181, 14310, 14439, 14570, 14702, 14836, 14970, 15106, 15243, 15381, 15520, 15661, 15803, 15946, 16091, 16237,
	16384,
	16533, 16682, 16834, 16986, 17140, 17296, 17452, 17611, 17770, 17931, 18094, 18258, 18424, 18591, 18759, 18929,
	19101, 19274, 19449, 19625, 19803, 19983, 20164, 20347, 20531, 20717, 20905, 21095, 21286, 21479, 21674, 21870,
	22068, 22268, 22470, 22674, 22880, 23087, 23296, 23507, 23721, 23936, 24153, 24372, 24593, 24816, 25041, 25268,
	25497, 25728, 25961, 26196, 26434, 26674, 26915, 27159, 27406, 27654, 27905, 28158, 28413, 28671, 28931, 29193
};

static const unsigned gAudioPitchBendTable_11[] = {
	8679 ,  8766,  8853,  8942,  9031,  9121,  9212,  9304,  9397,  9490,  9585,  9681,  9777,  9875,  9973, 10073,
	10173, 10275, 10377, 10481, 10585, 10691, 10798, 10905, 11014, 11124, 11235, 11347, 11460, 11575, 11690, 11807,
	11925, 12044, 12164, 12285, 12408, 12532, 12657, 12783, 12910, 13039, 13169, 13301, 13433, 13567, 13703, 13840,
	13978, 14117, 14258, 14400, 14544, 14689, 14836, 14984, 15133, 15284, 15437, 15591, 15746, 15903, 16062, 16222,
	16384,
	16547, 16713, 16879, 17048, 17218, 17390, 17563, 17738, 17915, 18094, 18275, 18457, 18641, 18827, 19015, 19205,
	19396, 19590, 19785, 19983, 20182, 20383, 20587, 20792, 21000, 21209, 21421, 21634, 21850, 22068, 22288, 22511,
	22735, 22962, 23191, 23423, 23656, 23893, 24131, 24372, 24615, 24860, 25108, 25359, 25612, 25868, 26126, 26386,
	26650, 26915, 27184, 27455, 27729, 28006, 28285, 28567, 28852, 29140, 29431, 29725, 30021, 30321, 30623, 30929
};

static const unsigned gAudioPitchBendTable_12[] = {
	8192 ,  8281,  8371,  8463,  8555,  8648,  8742,  8837,  8933,  9031,  9129,  9228,  9329,  9431,  9533,  9637,
	9742 ,  9848,  9955, 10064, 10173, 10284, 10396, 10509, 10624, 10739, 10856, 10975, 11094, 11215, 11337, 11460,
	11585, 11711, 11839, 11968, 12098, 12230, 12363, 12498, 12634, 12771, 12910, 13051, 13193, 13337, 13482, 13629,
	13777, 13927, 14079, 14232, 14387, 14544, 14702, 14862, 15024, 15188, 15353, 15520, 15689, 15860, 16033, 16208,
	16384,
	16562, 16743, 16925, 17109, 17296, 17484, 17674, 17867, 18061, 18258, 18457, 18658, 18861, 19066, 19274, 19484,
	19696, 19911, 20127, 20347, 20568, 20792, 21019, 21247, 21479, 21713, 21949, 22188, 22430, 22674, 22921, 23170,
	23423, 23678, 23936, 24196, 24460, 24726, 24995, 25268, 25543, 25821, 26102, 26386, 26674, 26964, 27258, 27554,
	27855, 28158, 28464, 28774, 29088, 29405, 29725, 30048, 30376, 30706, 31041, 31379, 31720, 32066, 32415, 32768,
};

static const unsigned gAudioPitchBendTable_13[] = {
	7732 ,  7823,  7916,  8009,  8104,  8199,  8296,  8394,  8493,  8593,  8695,  8797,  8901,  9006,  9113,  9220,
	9329 ,  9439,  9550,  9663,  9777,  9893, 10009, 10127, 10247, 10368, 10490, 10614, 10739, 10866, 10994, 11124,
	11255, 11388, 11523, 11659, 11796, 11935, 12076, 12219, 12363, 12509, 12657, 12806, 12957, 13110, 13265, 13421,
	13580, 13740, 13902, 14066, 14232, 14400, 14570, 14742, 14916, 15092, 15270, 15450, 15633, 15817, 16004, 16193,
	16384,
	16577, 16773, 16971, 17171, 17374, 17579, 17786, 17996, 18209, 18424, 18641, 18861, 19084, 19309, 19537, 19767,
	20001, 20237, 20476, 20717, 20962, 21209, 21459, 21713, 21969, 22228, 22491, 22756, 23025, 23296, 23571, 23849,
	24131, 24416, 24704, 24995, 25290, 25589, 25891, 26196, 26506, 26818, 27135, 27455, 27779, 28107, 28439, 28774,
	29114, 29458, 29805, 30157, 30513, 30873, 31237, 31606, 31979, 32357, 32738, 33125, 33516, 33911, 34312, 34716
};

static const unsigned gAudioPitchBendTable_14[] = {
	7298 ,  7391,  7485,  7580,  7677,  7774,  7873,  7973,  8075,  8177,  8281,  8387,  8493,  8601,  8711,  8821,
	8933 ,  9047,  9162,  9279,  9397,  9516,  9637,  9760,  9884, 10009, 10137, 10266, 10396, 10528, 10662, 10798,
	10935, 11074, 11215, 11357, 11502, 11648, 11796, 11946, 12098, 12252, 12408, 12566, 12725, 12887, 13051, 13217,
	13385, 13555, 13728, 13902, 14079, 14258, 14439, 14623, 14809, 14997, 15188, 15381, 15576, 15775, 15975, 16178,
	16384,
	16592, 16803, 17017, 17233, 17452, 17674, 17899, 18127, 18357, 18591, 18827, 19066, 19309, 19554, 19803, 20055,
	20310, 20568, 20830, 21095, 21363, 21634, 21910, 22188, 22470, 22756, 23045, 23338, 23635, 23936, 24240, 24548,
	24860, 25177, 25497, 25821, 26149, 26482, 26818, 27159, 27505, 27855, 28209, 28567, 28931, 29299, 29671, 30048,
	30430, 30817, 31209, 31606, 32008, 32415, 32827, 33245, 33667, 34095, 34529, 34968, 35413, 35863, 36319, 36781
};

static const unsigned gAudioPitchBendTable_15[] = {
	6889 ,  6983,  7078,  7174,  7272,  7371,  7472,  7573,  7677,  7781,  7887,  7995,  8104,  8214,  8326,  8440,
	8555 ,  8671,  8789,  8909,  9031,  9154,  9279,  9405,  9533,  9663,  9795,  9928, 10064, 10201, 10340, 10481,
	10624, 10769, 10915, 11064, 11215, 11368, 11523, 11680, 11839, 12000, 12164, 12330, 12498, 12668, 12841, 13016,
	13193, 13373, 13555, 13740, 13927, 14117, 14310, 14505, 14702, 14903, 15106, 15312, 15520, 15732, 15946, 16164,
	16384,
	16607, 16834, 17063, 17296, 17531, 17770, 18013, 18258, 18507, 18759, 19015, 19274, 19537, 19803, 20073, 20347,
	20624, 20905, 21190, 21479, 21772, 22068, 22369, 22674, 22983, 23296, 23614, 23936, 24262, 24593, 24928, 25268,
	25612, 25961, 26315, 26674, 27037, 27406, 27779, 28158, 28542, 28931, 29325, 29725, 30130, 30541, 30957, 31379,
	31806, 32240, 32679, 33125, 33576, 34034, 34498, 34968, 35445, 35928, 36417, 36914, 37417, 37927, 38444, 38968
};

static const unsigned gAudioPitchBendTable_16[] = {
	6502 ,  6597,  6693,  6790,  6889,  6989,  7090,  7194,  7298,  7404,  7512,  7621,  7732,  7845,  7959,  8075,
	8192 ,  8311,  8432,  8555,  8679,  8805,  8933,  9063,  9195,  9329,  9465,  9602,  9742,  9884, 10027, 10173,
	10321, 10471, 10624, 10778, 10935, 11094, 11255, 11419, 11585, 11754, 11925, 12098, 12274, 12453, 12634, 12818,
	13004, 13193, 13385, 13580, 13777, 13978, 14181, 14387, 14596, 14809, 15024, 15243, 15464, 15689, 15918, 16149,
	16384,
	16622, 16864, 17109, 17358, 17611, 17867, 18127, 18390, 18658, 18929, 19205, 19484, 19767, 20055, 20347, 20643,
	20943, 21247, 21556, 21870, 22188, 22511, 22838, 23170, 23507, 23849, 24196, 24548, 24905, 25268, 25635, 26008,
	26386, 26770, 27159, 27554, 27955, 28362, 28774, 29193, 29618, 30048, 30485, 30929, 31379, 31835, 32298, 32768,
	33245, 33728, 34219, 34716, 35221, 35734, 36254, 36781, 37316, 37859, 38409, 38968, 39535, 40110, 40693, 41285
};

static const unsigned gAudioPitchBendTable_17[] = {
	6137 ,  6232,  6328,  6426,  6526,  6626,  6729,  6833,  6939,  7046,  7155,  7265,  7378,  7492,  7608,  7725,
	7845 ,  7966,  8089,  8214,  8341,  8470,  8601,  8734,  8869,  9006,  9146,  9287,  9431,  9576,  9724,  9875,
	10027, 10182, 10340, 10500, 10662, 10827, 10994, 11164, 11337, 11512, 11690, 11871, 12055, 12241, 12430, 12622,
	12818, 13016, 13217, 13421, 13629, 13840, 14054, 14271, 14491, 14716, 14943, 15174, 15409, 15647, 15889, 16135,
	16384,
	16637, 16895, 17156, 17421, 17690, 17964, 18242, 18524, 18810, 19101, 19396, 19696, 20001, 20310, 20624, 20943,
	21267, 21595, 21929, 22268, 22613, 22962, 23317, 23678, 24044, 24416, 24793, 25177, 25566, 25961, 26362, 26770,
	27184, 27604, 28031, 28464, 28905, 29351, 29805, 30266, 30734, 31209, 31692, 32182, 32679, 33185, 33698, 34219,
	34748, 35285, 35831, 36385, 36947, 37518, 38099, 38688, 39286, 39893, 40510, 41136, 41772, 42418, 43074, 43740
};

static const unsigned gAudioPitchBendTable_18[] = {
	5793 ,  5887,  5984,  6082,  6182,  6283,  6386,  6490,  6597,  6705,  6814,  6926,  7039,  7155,  7272,  7391,
	7512 ,  7635,  7760,  7887,  8016,  8148,  8281,  8417,  8555,  8695,  8837,  8982,  9129,  9279,  9431,  9585,
	9742 ,  9902, 10064, 10229, 10396, 10566, 10739, 10915, 11094, 11276, 11460, 11648, 11839, 12033, 12230, 12430,
	12634, 12841, 13051, 13265, 13482, 13703, 13927, 14155, 14387, 14623, 14862, 15106, 15353, 15605, 15860, 16120,
	16384,
	16652, 16925, 17202, 17484, 17770, 18061, 18357, 18658, 18963, 19274, 19590, 19911, 20237, 20568, 20905, 21247,
	21595, 21949, 22309, 22674, 23045, 23423, 23806, 24196, 24593, 24995, 25405, 25821, 26244, 26674, 27110, 27554,
	28006, 28464, 28931, 29405, 29886, 30376, 30873, 31379, 31893, 32415, 32946, 33486, 34034, 34591, 35158, 35734,
	36319, 36914, 37518, 38133, 38757, 39392, 40037, 40693, 41360, 42037, 42726, 43425, 44137, 44859, 45594, 46341
};

static const unsigned gAudioPitchBendTable_19[] = {
	5468 ,  5562,  5658,  5756,  5856,  5957,  6060,  6165,  6271,  6380,  6490,  6603,  6717,  6833,  6951,  7071,
	7194 ,  7318,  7445,  7573,  7704,  7838,  7973,  8111,  8251,  8394,  8539,  8687,  8837,  8990,  9146,  9304,
	9465 ,  9628,  9795,  9964, 10137, 10312, 10490, 10672, 10856, 11044, 11235, 11429, 11627, 11828, 12033, 12241,
	12453, 12668, 12887, 13110, 13337, 13567, 13802, 14041, 14284, 14531, 14782, 15038, 15298, 15562, 15832, 16105,
	16384,
	16667, 16956, 17249, 17547, 17851, 18160, 18474, 18793, 19118, 19449, 19785, 20127, 20476, 20830, 21190, 21556,
	21929, 22309, 22694, 23087, 23486, 23893, 24306, 24726, 25154, 25589, 26031, 26482, 26940, 27406, 27880, 28362,
	28852, 29351, 29859, 30376, 30901, 31435, 31979, 32532, 33095, 33667, 34250, 34842, 35445, 36058, 36681, 37316,
	37961, 38618, 39286, 39965, 40656, 41360, 42075, 42803, 43543, 44296, 45062, 45842, 46635, 47441, 48262, 49097
};

static const unsigned gAudioPitchBendTable_20[] = {
	5161 ,  5255,  5350,  5448,  5547,  5648,  5751,  5856,  5962,  6071,  6182,  6294,  6409,  6526,  6644,  6765,
	6889 ,  7014,  7142,  7272,  7404,  7539,  7677,  7816,  7959,  8104,  8251,  8402,  8555,  8711,  8869,  9031,
	9195 ,  9363,  9533,  9707,  9884, 10064, 10247, 10434, 10624, 10817, 11014, 11215, 11419, 11627, 11839, 12055,
	12274, 12498, 12725, 12957, 13193, 13433, 13678, 13927, 14181, 14439, 14702, 14970, 15243, 15520, 15803, 16091,
	16384,
	16682, 16986, 17296, 17611, 17931, 18258, 18591, 18929, 19274, 19625, 19983, 20347, 20717, 21095, 21479, 21870,
	22268, 22674, 23087, 23507, 23936, 24372, 24816, 25268, 25728, 26196, 26674, 27159, 27654, 28158, 28671, 29193,
	29725, 30266, 30817, 31379, 31950, 32532, 33125, 33728, 34343, 34968, 35605, 36254, 36914, 37586, 38271, 38968,
	39678, 40400, 41136, 41886, 42649, 43425, 44216, 45022, 45842, 46677, 47527, 48393, 49274, 50172, 51085, 52016
};

static const unsigned gAudioPitchBendTable_21[] = {
	4871 ,  4964,  5059,  5156,  5255,  5355,  5458,  5562,  5668,  5777,  5887,  6000,  6115,  6232,  6351,  6473,
	6597 ,  6723,  6851,  6983,  7116,  7252,  7391,  7532,  7677,  7823,  7973,  8126,  8281,  8440,  8601,  8766,
	8933 ,  9104,  9279,  9456,  9637,  9821, 10009, 10201, 10396, 10595, 10798, 11004, 11215, 11429, 11648, 11871,
	12098, 12330, 12566, 12806, 13051, 13301, 13555, 13815, 14079, 14348, 14623, 14903, 15188, 15478, 15775, 16076,
	16384,
	16697, 17017, 17343, 17674, 18013, 18357, 18708, 19066, 19431, 19803, 20182, 20568, 20962, 21363, 21772, 22188,
	22613, 23045, 23486, 23936, 24394, 24860, 25336, 25821, 26315, 26818, 27332, 27855, 28388, 28931, 29484, 30048,
	30623, 31209, 31806, 32415, 33035, 33667, 34312, 34968, 35637, 36319, 37014, 37722, 38444, 39180, 39929, 40693,
	41472, 42265, 43074, 43898, 44738, 45594, 46467, 47356, 48262, 49185, 50126, 51085, 52063, 53059, 54074, 55109
};

static const unsigned gAudioPitchBendTable_22[] = {
	4598 ,  4690,  4784,  4880,  4978,  5077,  5179,  5283,  5389,  5497,  5607,  5720,  5835,  5952,  6071,  6193,
	6317 ,  6444,  6573,  6705,  6839,  6976,  7116,  7259,  7404,  7553,  7704,  7859,  8016,  8177,  8341,  8508,
	8679 ,  8853,  9031,  9212,  9397,  9585,  9777,  9973, 10173, 10377, 10585, 10798, 11014, 11235, 11460, 11690,
	11925, 12164, 12408, 12657, 12910, 13169, 13433, 13703, 13978, 14258, 14544, 14836, 15133, 15437, 15746, 16062,
	16384,
	16713, 17048, 17390, 17738, 18094, 18457, 18827, 19205, 19590, 19983, 20383, 20792, 21209, 21634, 22068, 22511,
	22962, 23423, 23893, 24372, 24860, 25359, 25868, 26386, 26915, 27455, 28006, 28567, 29140, 29725, 30321, 30929,
	31549, 32182, 32827, 33486, 34157, 34842, 35541, 36254, 36981, 37722, 38479, 39250, 40037, 40840, 41659, 42495,
	43347, 44216, 45103, 46008, 46930, 47871, 48831, 49811, 50810, 51829, 52868, 53928, 55010, 56113, 57238, 58386
};

static const unsigned gAudioPitchBendTable_23[] = {
	4340 ,  4431,  4524,  4618,  4715,  4814,  4915,  5018,  5124,  5231,  5341,  5453,  5567,  5684,  5803,  5925,
	6049 ,  6176,  6306,  6438,  6573,  6711,  6851,  6995,  7142,  7292,  7445,  7601,  7760,  7923,  8089,  8259,
	8432 ,  8609,  8789,  8974,  9162,  9354,  9550,  9751,  9955, 10164, 10377, 10595, 10817, 11044, 11276, 11512,
	11754, 12000, 12252, 12509, 12771, 13039, 13313, 13592, 13877, 14168, 14465, 14769, 15079, 15395, 15718, 16047,
	16384,
	16728, 17079, 17437, 17802, 18176, 18557, 18946, 19344, 19750, 20164, 20587, 21019, 21459, 21910, 22369, 22838,
	23317, 23806, 24306, 24816, 25336, 25868, 26410, 26964, 27530, 28107, 28697, 29299, 29913, 30541, 31181, 31835,
	32503, 33185, 33881, 34591, 35317, 36058, 36814, 37586, 38375, 39180, 40001, 40840, 41697, 42572, 43465, 44376,
	45307, 46257, 47228, 48218, 49230, 50262, 51317, 52393, 53492, 54614, 55759, 56929, 58123, 59342, 60587, 61858
};

static const unsigned gAudioPitchBendTable_24[] = {
	4096 ,  4186,  4277,  4371,  4467,  4565,  4664,  4767,  4871,  4978,  5087,  5198,  5312,  5428,  5547,  5668,
	5793 ,  5919,  6049,  6182,  6317,  6455,  6597,  6741,  6889,  7039,  7194,  7351,  7512,  7677,  7845,  8016,
	8192 ,  8371,  8555,  8742,  8933,  9129,  9329,  9533,  9742,  9955, 10173, 10396, 10624, 10856, 11094, 11337,
	11585, 11839, 12098, 12363, 12634, 12910, 13193, 13482, 13777, 14079, 14387, 14702, 15024, 15353, 15689, 16033,
	16384,
	16743, 17109, 17484, 17867, 18258, 18658, 19066, 19484, 19911, 20347, 20792, 21247, 21713, 22188, 22674, 23170,
	23678, 24196, 24726, 25268, 25821, 26386, 26964, 27554, 28158, 28774, 29405, 30048, 30706, 31379, 32066, 32768,
	33486, 34219, 34968, 35734, 36516, 37316, 38133, 38968, 39821, 40693, 41584, 42495, 43425, 44376, 45348, 46341,
	47356, 48393, 49452, 50535, 51642, 52773, 53928, 55109, 56316, 57549, 58809, 60097, 61413, 62757, 64132, 65535
};

static const unsigned* gAudioPitchBendTable[] = {
	gAudioPitchBendTable_2,
	gAudioPitchBendTable_1,
	gAudioPitchBendTable_2,	// defaults
	gAudioPitchBendTable_3,
	gAudioPitchBendTable_4,
	gAudioPitchBendTable_5,
	gAudioPitchBendTable_6,
	gAudioPitchBendTable_7,
	gAudioPitchBendTable_8,
	gAudioPitchBendTable_9,
	gAudioPitchBendTable_10,
	gAudioPitchBendTable_11,
	gAudioPitchBendTable_12,
	gAudioPitchBendTable_13,
	gAudioPitchBendTable_14,
	gAudioPitchBendTable_15,
	gAudioPitchBendTable_16,
	gAudioPitchBendTable_17,
	gAudioPitchBendTable_18,
	gAudioPitchBendTable_19,
	gAudioPitchBendTable_20,
	gAudioPitchBendTable_21,
	gAudioPitchBendTable_22,
	gAudioPitchBendTable_23,
	gAudioPitchBendTable_24
};

unsigned Audio_computePitchBendFactor(int sensitivity, int pitch_bend) {
	const unsigned* pb_table = 0;
	
	if (sensitivity > 24) {
		sensitivity = 24;
	} else if (sensitivity < 0) {
		sensitivity = 0;
	}
	
	pb_table = gAudioPitchBendTable[sensitivity];
	
	if (pitch_bend > D_AUDIO_PITCHBEND_MAX_STEP) {
		pitch_bend = D_AUDIO_PITCHBEND_MAX_STEP;
	} else if (pitch_bend < -D_AUDIO_PITCHBEND_MAX_STEP) {
		pitch_bend = -D_AUDIO_PITCHBEND_MAX_STEP;
	}
	
	pitch_bend += D_AUDIO_PITCHBEND_START;
	
	return pb_table[pitch_bend];
}

#else

#define D_AUDIO_PITCHBEND_START 64
#define D_AUDIO_PITCHBEND_MAX_STEP 64

#ifdef AUDIO_USE_LIMITED_PITCHBEND_TABLE
static const unsigned gAudioPitchBendTable_2[] = {
	14596, 14623, 14649, 14676, 14702, 14729, 14755, 14782, 14809, 14836, 14862, 14889, 14916, 14943, 14970, 14997,
	15024, 15051, 15079, 15106, 15133, 15160, 15188, 15215, 15243, 15270, 15298, 15325, 15353, 15381, 15409, 15437,
	15464, 15492, 15520, 15548, 15576, 15605, 15633, 15661, 15689, 15718, 15746, 15775, 15803, 15832, 15860, 15889,
	15918, 15946, 15975, 16004, 16033, 16062, 16091, 16120, 16149, 16178, 16208, 16237, 16266, 16296, 16325, 16354,
	16384,
	16414, 16443, 16473, 16503, 16533, 16562, 16592, 16622, 16652, 16682, 16713, 16743, 16773, 16803, 16834, 16864,
	16895, 16925, 16956, 16986, 17017, 17048, 17079, 17109, 17140, 17171, 17202, 17233, 17264, 17296, 17327, 17358,
	17390, 17421, 17452, 17484, 17516, 17547, 17579, 17611, 17643, 17674, 17706, 17738, 17770, 17802, 17835, 17867,
	17899, 17931, 17964, 17996, 18029, 18061, 18094, 18127, 18160, 18192, 18225, 18258, 18291, 18324, 18357, 18390
};
#endif

extern int fixExp(int a);

unsigned Audio_computePitchBendFactor(int sensitivity, int pitch_bend) {
	const int POWER_ONE = 65536;
	const int LOG2_E = 94548; // 1.4426950408889634073599246810019 * POWER_ONE
	long long int temp = 0;
	unsigned result = 0;
	
	if (sensitivity > 24) {
		sensitivity = 24;
	} else if (sensitivity < 0) {
		sensitivity = 0;
	}
	
	if (pitch_bend > D_AUDIO_PITCHBEND_MAX_STEP) {
		pitch_bend = D_AUDIO_PITCHBEND_MAX_STEP;
	} else if (pitch_bend < -D_AUDIO_PITCHBEND_MAX_STEP) {
		pitch_bend = -D_AUDIO_PITCHBEND_MAX_STEP;
	}

#ifdef AUDIO_USE_LIMITED_PITCHBEND_TABLE
	if ((sensitivity == 0) || (sensitivity == 2)) {
		pitch_bend += D_AUDIO_PITCHBEND_START;
		return gAudioPitchBendTable_2[pitch_bend];
	}
#endif
	
	result = POWER_ONE;

#ifdef AUDIO_ENABLE_FIX_POINT_LIB
	if (pitch_bend != 0) {
		temp = POWER_ONE * sensitivity * pitch_bend / 12 / 64;
		temp *= POWER_ONE;
		temp /= LOG2_E;
		result = fixExp((int)temp);
	}
#endif
	
	return result >> 2; // 16bit fraction part result to 14bit fraction part result
}

#endif
