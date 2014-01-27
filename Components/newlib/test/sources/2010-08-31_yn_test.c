#include <math.h>

long yn_vec[][10] = {
    {32,0,123,__LINE__, 0xbfffab42, 0x0311f796, 0x00000000, 0x00000000, 0x3fa99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc0299467, 0xe5a18bc5, 0x3ff00000, 0x00000000, 0x3fa99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc07fd9d6, 0x9d06dcbe, 0x40000000, 0x00000000, 0x3fa99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc0e3e68c, 0xdba5efde, 0x40080000, 0x00000000, 0x3fa99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc152a7a4, 0xa6911cc5, 0x40100000, 0x00000000, 0x3fa99999, 0x9999999a},
    {32,0,123,__LINE__, 0xbff88c3d, 0xd3fcf18c, 0x00000000, 0x00000000, 0x3fb99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc019d5f7, 0x4698d59d, 0x3ff00000, 0x00000000, 0x3fb99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc05fe944, 0x20ef173e, 0x40000000, 0x00000000, 0x3fb99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc0b3eb55, 0x16c3c852, 0x40080000, 0x00000000, 0x3fb99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc112aaa1, 0x31157cdc, 0x40100000, 0x00000000, 0x3fb99999, 0x9999999a},
    {32,0,123,__LINE__, 0xbff45519, 0x9a9aa247, 0x00000000, 0x00000000, 0x3fc33333, 0x33333334},
    {32,0,123,__LINE__, 0xc0117469, 0x70206678, 0x3ff00000, 0x00000000, 0x3fc33333, 0x33333334},
    {32,0,123,__LINE__, 0xc04c74b1, 0x98b68060, 0x40000000, 0x00000000, 0x3fc33333, 0x33333334},
    {32,0,123,__LINE__, 0xc097a51f, 0x95d29fe9, 0x40080000, 0x00000000, 0x3fc33333, 0x33333334},
    {32,0,123,__LINE__, 0xc0ed874a, 0x4ee11a42, 0x40100000, 0x00000000, 0x3fc33333, 0x33333334},
    {32,0,123,__LINE__, 0xbff14c35, 0x1831ea96, 0x00000000, 0x00000000, 0x3fc99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc00a9731, 0x8e2c8b35, 0x3ff00000, 0x00000000, 0x3fc99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc040141d, 0x501a47ac, 0x40000000, 0x00000000, 0x3fc99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc083fe8d, 0x7292ad0c, 0x40080000, 0x00000000, 0x3fc99999, 0x9999999a},
    {32,0,123,__LINE__, 0xc0d2b69a, 0x8cc17517, 0x40100000, 0x00000000, 0x3fc99999, 0x9999999a},
    {32,0,123,__LINE__, 0xbfedcf72, 0x3b7d21f4, 0x00000000, 0x00000000, 0x3fd00000, 0x00000000},
    {32,0,123,__LINE__, 0xc005a201, 0xec26d052, 0x3ff00000, 0x00000000, 0x3fd00000, 0x00000000},
    {32,0,123,__LINE__, 0xc034b386, 0x5a4ae742, 0x40000000, 0x00000000, 0x3fd00000, 0x00000000},
    {32,0,123,__LINE__, 0xc0748842, 0x567299a1, 0x40080000, 0x00000000, 0x3fd00000, 0x00000000},
    {32,0,123,__LINE__, 0xc0beb7af, 0xfb519b8b, 0x40100000, 0x00000000, 0x3fd00000, 0x00000000},
    {32,0,123,__LINE__, 0xbfe9d52f, 0x65f30ce4, 0x00000000, 0x00000000, 0x3fd33333, 0x33333333},
    {32,0,123,__LINE__, 0xc0025847, 0x81bd58a7, 0x3ff00000, 0x00000000, 0x3fd33333, 0x33333333},
    {32,0,123,__LINE__, 0xc02cf5ce, 0xe1dc62f3, 0x40000000, 0x00000000, 0x3fd33333, 0x33333333},
    {32,0,123,__LINE__, 0xc067d8cb, 0x48db5d12, 0x40080000, 0x00000000, 0x3fd33333, 0x33333333},
    {32,0,123,__LINE__, 0xc0adb208, 0x4c3057f3, 0x40100000, 0x00000000, 0x3fd33333, 0x33333333},
    {32,0,123,__LINE__, 0xbfe668f8, 0x269cb98f, 0x00000000, 0x00000000, 0x3fd66666, 0x66666666},
    {32,0,123,__LINE__, 0xc00000cf, 0xb133ffa1, 0x3ff00000, 0x00000000, 0x3fd66666, 0x66666666},
    {32,0,123,__LINE__, 0xc0257606, 0xe872c629, 0x40000000, 0x00000000, 0x3fd66666, 0x66666666},
    {32,0,123,__LINE__, 0xc05e2895, 0xa9f5c43e, 0x40080000, 0x00000000, 0x3fd66666, 0x66666666},
    {32,0,123,__LINE__, 0xc0a01291, 0x01db365b, 0x40100000, 0x00000000, 0x3fd66666, 0x66666666},
    {32,0,123,__LINE__, 0xbfe3648d, 0xa2beedbd, 0x00000000, 0x00000000, 0x3fd99999, 0x99999999},
    {32,0,123,__LINE__, 0xbffc7e73, 0xaf480232, 0x3ff00000, 0x00000000, 0x3fd99999, 0x99999999},
    {32,0,123,__LINE__, 0xc02098bf, 0x73611283, 0x40000000, 0x00000000, 0x3fd99999, 0x99999999},
    {32,0,123,__LINE__, 0xc0544cf5, 0x817c371b, 0x40080000, 0x00000000, 0x3fd99999, 0x99999999},
    {32,0,123,__LINE__, 0xc092e6f4, 0xaa7db185, 0x40100000, 0x00000000, 0x3fd99999, 0x99999999},
    {32,0,123,__LINE__, 0xbfe0af8a, 0xa64cf8b1, 0x00000000, 0x00000000, 0x3fdccccc, 0xcccccccc},
    {32,0,123,__LINE__, 0xbff9c0b2, 0x3c24181f, 0x3ff00000, 0x00000000, 0x3fdccccc, 0xcccccccc},
    {32,0,123,__LINE__, 0xc01a8746, 0x7c4209f0, 0x40000000, 0x00000000, 0x3fdccccc, 0xcccccccc},
    {32,0,123,__LINE__, 0xc04cabd6, 0xf82f5c11, 0x40080000, 0x00000000, 0x3fdccccc, 0xcccccccc},
    {32,0,123,__LINE__, 0xc087af79, 0xec8448a5, 0x40100000, 0x00000000, 0x3fdccccc, 0xcccccccc},
    {32,0,123,__LINE__, 0xbfdc72fe, 0xb3b7b8a4, 0x00000000, 0x00000000, 0x3fdfffff, 0xffffffff},
    {32,0,123,__LINE__, 0xbff78b26, 0xa2b7c4e0, 0x3ff00000, 0x00000000, 0x3fdfffff, 0xffffffff},
    {32,0,123,__LINE__, 0xc015c3f6, 0xb77c4957, 0x40000000, 0x00000000, 0x3fdfffff, 0xffffffff},
    {32,0,123,__LINE__, 0xc045079d, 0x82668b31, 0x40080000, 0x00000000, 0x3fdfffff, 0xffffffff},
    {32,0,123,__LINE__, 0xc07f345c, 0x68bbdfa6, 0x40100000, 0x00000000, 0x3fdfffff, 0xffffffff},
    {32,0,123,__LINE__, 0xbfd7edc1, 0xa5c74a03, 0x00000000, 0x00000000, 0x3fe19999, 0x99999999},
    {32,0,123,__LINE__, 0xbff5b705, 0x5ba816d7, 0x3ff00000, 0x00000000, 0x3fe19999, 0x99999999},
    {32,0,123,__LINE__, 0xc0123ecb, 0xad53a024, 0x40000000, 0x00000000, 0x3fe19999, 0x99999999},
    {32,0,123,__LINE__, 0xc03fd0ea, 0xb6dd8a79, 0x40080000, 0x00000000, 0x3fe19999, 0x99999999},
    {32,0,123,__LINE__, 0xc075685f, 0x08278fe9, 0x40100000, 0x00000000, 0x3fe19999, 0x99999999},
    {32,0,123,__LINE__, 0xbfd3bea0, 0x2ea8f055, 0x00000000, 0x00000000, 0x3fe33333, 0x33333333},
    {32,0,123,__LINE__, 0xbff42a90, 0x1e049cb9, 0x3ff00000, 0x00000000, 0x3fe33333, 0x33333333},
    {32,0,123,__LINE__, 0xc00f2471, 0x8187e729, 0x40000000, 0x00000000, 0x3fe33333, 0x33333333},
    {32,0,123,__LINE__, 0xc038b10a, 0xea10f6d6, 0x40080000, 0x00000000, 0x3fe33333, 0x33333333},
    {32,0,123,__LINE__, 0xc06e60bb, 0xde8f14ef, 0x40100000, 0x00000000, 0x3fe33333, 0x33333333},
    {32,0,123,__LINE__, 0xbfcfb24a, 0xb3c28954, 0x00000000, 0x00000000, 0x3fe4cccc, 0xcccccccd},
    {32,0,123,__LINE__, 0xbff2d40d, 0x6feded13, 0x3ff00000, 0x00000000, 0x3fe4cccc, 0xcccccccd},
    {32,0,123,__LINE__, 0xc00afc52, 0x771e3074, 0x40000000, 0x00000000, 0x3fe4cccc, 0xcccccccd},
    {32,0,123,__LINE__, 0xc03394d7, 0x35dd4674, 0x40080000, 0x00000000, 0x3fe4cccc, 0xcccccccd},
    {32,0,123,__LINE__, 0xc0662c1a, 0xb93689c3, 0x40100000, 0x00000000, 0x3fe4cccc, 0xcccccccd},
    {32,0,123,__LINE__, 0xbfc867b5, 0x59ffc712, 0x00000000, 0x00000000, 0x3fe66666, 0x66666667},
    {32,0,123,__LINE__, 0xbff1a6e9, 0x56728d34, 0x3ff00000, 0x00000000, 0x3fe66666, 0x66666667},
    {32,0,123,__LINE__, 0xc007b11b, 0x25df166b, 0x40000000, 0x00000000, 0x3fe66666, 0x66666667},
    {32,0,123,__LINE__, 0xc02fa392, 0xc2273c16, 0x40080000, 0x00000000, 0x3fe66666, 0x66666667},
    {32,0,123,__LINE__, 0xc060944a, 0x3246acfb, 0x40100000, 0x00000000, 0x3fe66666, 0x66666667},
    {32,0,123,__LINE__, 0xbfc18ee0, 0x9734f23a, 0x00000000, 0x00000000, 0x3fe80000, 0x00000001},
    {32,0,123,__LINE__, 0xbff099fc, 0xbe60fd83, 0x3ff00000, 0x00000000, 0x3fe80000, 0x00000001},
    {32,0,123,__LINE__, 0xc00509b8, 0x49b8ad89, 0x40000000, 0x00000000, 0x3fe80000, 0x00000001},
    {32,0,123,__LINE__, 0xc029f9b6, 0x1fd4c7b0, 0x40080000, 0x00000000, 0x3fe80000, 0x00000001},
    {32,0,123,__LINE__, 0xc0595168, 0x5d870243, 0x40100000, 0x00000000, 0x3fe80000, 0x00000001},
    {32,0,123,__LINE__, 0xbfb638ac, 0x9857e734, 0x00000000, 0x00000000, 0x3fe99999, 0x9999999b},
    {32,0,123,__LINE__, 0xbfef4cf5, 0x04341787, 0x3ff00000, 0x00000000, 0x3fe99999, 0x9999999b},
    {32,0,123,__LINE__, 0xc002de53, 0xbdddcf79, 0x40000000, 0x00000000, 0x3fe99999, 0x9999999b},
    {32,0,123,__LINE__, 0xc025a119, 0x5d1201de, 0x40080000, 0x00000000, 0x3fe99999, 0x9999999b},
    {32,0,123,__LINE__, 0xc053b015, 0x2951f343, 0x40100000, 0x00000000, 0x3fe99999, 0x9999999b},
    {32,0,123,__LINE__, 0xbfa41bfc, 0xc78b842a, 0x00000000, 0x00000000, 0x3feb3333, 0x33333335},
    {32,0,123,__LINE__, 0xbfed8e7b, 0x7f0fdfef, 0x3ff00000, 0x00000000, 0x3feb3333, 0x33333335},
    {32,0,123,__LINE__, 0xc001126f, 0x4890ce20, 0x40000000, 0x00000000, 0x3feb3333, 0x33333335},
    {32,0,123,__LINE__, 0xc0223cc8, 0x6131d662, 0x40080000, 0x00000000, 0x3feb3333, 0x33333335},
    {32,0,123,__LINE__, 0xc04f1de0, 0x4d92a9aa, 0x40100000, 0x00000000, 0x3feb3333, 0x33333335},
    {32,0,123,__LINE__, 0xbfebf0a7, 0x289d61b8, 0x3ff00000, 0x00000000, 0x3feccccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xbfff2272, 0x1b13153c, 0x40000000, 0x00000000, 0x3feccccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xc01f19f8, 0x1c8fc0b3, 0x40080000, 0x00000000, 0x3feccccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xc048f1e5, 0xdc49dd3f, 0x40100000, 0x00000000, 0x3feccccc, 0xcccccccf},
    {32,0,123,__LINE__, 0x3fa89f50, 0x75b2c5e8, 0x00000000, 0x00000000, 0x3fee6666, 0x66666669},
    {32,0,123,__LINE__, 0xbfea6d54, 0xa3bd64cf, 0x3ff00000, 0x00000000, 0x3fee6666, 0x66666669},
    {32,0,123,__LINE__, 0xbffc9661, 0x1516a1f9, 0x40000000, 0x00000000, 0x3fee6666, 0x66666669},
    {32,0,123,__LINE__, 0xc01ac9e4, 0x0877ba85, 0x40080000, 0x00000000, 0x3fee6666, 0x66666669},
    {32,0,123,__LINE__, 0xc044416c, 0xc8219ad1, 0x40100000, 0x00000000, 0x3fee6666, 0x66666669},
    {32,0,123,__LINE__, 0x3fb69802, 0x26f358eb, 0x00000000, 0x00000000, 0x3ff00000, 0x00000001},
    {32,0,123,__LINE__, 0xbfe8ffb2, 0x07d66b92, 0x3ff00000, 0x00000000, 0x3ff00000, 0x00000001},
    {32,0,123,__LINE__, 0xbffa6932, 0x2a45a11f, 0x40000000, 0x00000000, 0x3ff00000, 0x00000001},
    {32,0,123,__LINE__, 0xc017493b, 0xe94ad3ab, 0x40080000, 0x00000000, 0x3ff00000, 0x00000001},
    {32,0,123,__LINE__, 0xc040a3a3, 0x5da5f1b7, 0x40100000, 0x00000000, 0x3ff00000, 0x00000001},
    {32,0,123,__LINE__, 0x3fc028e5, 0x7ba671b1, 0x00000000, 0x00000000, 0x3ff0cccc, 0xccccccce},
    {32,0,123,__LINE__, 0xbfe7a3ee, 0x4fd1a036, 0x3ff00000, 0x00000000, 0x3ff0cccc, 0xccccccce},
    {32,0,123,__LINE__, 0xbff888db, 0x13daece2, 0x40000000, 0x00000000, 0x3ff0cccc, 0xccccccce},
    {32,0,123,__LINE__, 0xc0146946, 0xaa74c5f3, 0x40080000, 0x00000000, 0x3ff0cccc, 0xccccccce},
    {32,0,123,__LINE__, 0xc03ba020, 0x66d6d9af, 0x40100000, 0x00000000, 0x3ff0cccc, 0xccccccce},
    {32,0,123,__LINE__, 0x3fc4c1c3, 0x8a97c170, 0x00000000, 0x00000000, 0x3ff19999, 0x9999999b},
    {32,0,123,__LINE__, 0xbfe656fe, 0xd4e6087e, 0x3ff00000, 0x00000000, 0x3ff19999, 0x9999999b},
    {32,0,123,__LINE__, 0xbff6e74e, 0xa73b5cfc, 0x40000000, 0x00000000, 0x3ff19999, 0x9999999b},
    {32,0,123,__LINE__, 0xc0120767, 0xa624d948, 0x40080000, 0x00000000, 0x3ff19999, 0x9999999b},
    {32,0,123,__LINE__, 0xc0372746, 0xf81ba105, 0x40100000, 0x00000000, 0x3ff19999, 0x9999999b},
    {32,0,123,__LINE__, 0x3fc91951, 0xdc7f9b41, 0x00000000, 0x00000000, 0x3ff26666, 0x66666668},
    {32,0,123,__LINE__, 0xbfe51674, 0x19440e82, 0x3ff00000, 0x00000000, 0x3ff26666, 0x66666668},
    {32,0,123,__LINE__, 0xbff57978, 0xed5bd37f, 0x40000000, 0x00000000, 0x3ff26666, 0x66666668},
    {32,0,123,__LINE__, 0xc010099a, 0xa448bba8, 0x40080000, 0x00000000, 0x3ff26666, 0x66666668},
    {32,0,123,__LINE__, 0xc033938b, 0x31301627, 0x40100000, 0x00000000, 0x3ff26666, 0x66666668},
    {32,0,123,__LINE__, 0x3fcd31d7, 0x198f1810, 0x00000000, 0x00000000, 0x3ff33333, 0x33333335},
    {32,0,123,__LINE__, 0xbfe3e059, 0x66ac1181, 0x3ff00000, 0x00000000, 0x3ff33333, 0x33333335},
    {32,0,123,__LINE__, 0xbff43685, 0x636bf197, 0x40000000, 0x00000000, 0x3ff33333, 0x33333335},
    {32,0,123,__LINE__, 0xc00cb81d, 0x4c08e398, 0x40080000, 0x00000000, 0x3ff33333, 0x33333335},
    {32,0,123,__LINE__, 0xc030afa9, 0xf94ecf24, 0x40100000, 0x00000000, 0x3ff33333, 0x33333335},
    {32,0,123,__LINE__, 0x3fd0869f, 0xf937fa16, 0x00000000, 0x00000000, 0x3ff40000, 0x00000002},
    {32,0,123,__LINE__, 0xbfe2b31c, 0x35470a48, 0x3ff00000, 0x00000000, 0x3ff40000, 0x00000002},
    {32,0,123,__LINE__, 0xbff31758, 0x28ed39f1, 0x40000000, 0x00000000, 0x3ff40000, 0x00000002},
    {32,0,123,__LINE__, 0xc009def9, 0x3429cd53, 0x40080000, 0x00000000, 0x3ff40000, 0x00000002},
    {32,0,123,__LINE__, 0xc02ca8a6, 0x6cae1bef, 0x40100000, 0x00000000, 0x3ff40000, 0x00000002},
    {32,0,123,__LINE__, 0x3fd25698, 0x650b1f9a, 0x00000000, 0x00000000, 0x3ff4cccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xbfe18d79, 0x3faf4d46, 0x3ff00000, 0x00000000, 0x3ff4cccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xbff2162a, 0xc0672ab8, 0x40000000, 0x00000000, 0x3ff4cccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xc0076ff7, 0x26da4702, 0x40080000, 0x00000000, 0x3ff4cccc, 0xcccccccf},
    {32,0,123,__LINE__, 0xc028c844, 0x238c8047, 0x40100000, 0x00000000, 0x3ff4cccc, 0xcccccccf},
    {32,0,123,__LINE__, 0x3fd4098a, 0x03bf8bf4, 0x00000000, 0x00000000, 0x3ff59999, 0x9999999c},
    {32,0,123,__LINE__, 0xbfe06e6d, 0xc954f277, 0x3ff00000, 0x00000000, 0x3ff59999, 0x9999999c},
    {32,0,123,__LINE__, 0xbff12e42, 0x0c971b54, 0x40000000, 0x00000000, 0x3ff59999, 0x9999999c},
    {32,0,123,__LINE__, 0xc0055854, 0xa051bc74, 0x40080000, 0x00000000, 0x3ff59999, 0x9999999c},
    {32,0,123,__LINE__, 0xc02591b2, 0x37ab7c30, 0x40100000, 0x00000000, 0x3ff59999, 0x9999999c},
    {32,0,123,__LINE__, 0x3fd5a012, 0xe4dcec98, 0x00000000, 0x00000000, 0x3ff66666, 0x66666669},
    {32,0,123,__LINE__, 0xbfdeaa58, 0x1213a9a4, 0x3ff00000, 0x00000000, 0x3ff66666, 0x66666669},
    {32,0,123,__LINE__, 0xbff05bb6, 0x76876572, 0x40000000, 0x00000000, 0x3ff66666, 0x66666669},
    {32,0,123,__LINE__, 0xc0038927, 0x5decadfe, 0x40080000, 0x00000000, 0x3ff66666, 0x66666669},
    {32,0,123,__LINE__, 0xc022e2ea, 0x3a638496, 0x40100000, 0x00000000, 0x3ff66666, 0x66666669},
    {32,0,123,__LINE__, 0x3fd71abe, 0xc53f5eb2, 0x00000000, 0x00000000, 0x3ff73333, 0x33333336},
    {32,0,123,__LINE__, 0xbfdc8223, 0xe8af2ee9, 0x3ff00000, 0x00000000, 0x3ff73333, 0x33333336},
    {32,0,123,__LINE__, 0xbfef3692, 0xa1fe04a8, 0x40000000, 0x00000000, 0x3ff73333, 0x33333336},
    {32,0,123,__LINE__, 0xc001f678, 0xe9ce40a6, 0x40080000, 0x00000000, 0x3ff73333, 0x33333336},
    {32,0,123,__LINE__, 0xc020a1a1, 0x28d8cc82, 0x40100000, 0x00000000, 0x3ff73333, 0x33333336},
    {32,0,123,__LINE__, 0x3fd87a0b, 0x0d06836c, 0x00000000, 0x00000000, 0x3ff80000, 0x00000003},
    {32,0,123,__LINE__, 0xbfda6343, 0xb92d8b07, 0x3ff00000, 0x00000000, 0x3ff80000, 0x00000003},
    {32,0,123,__LINE__, 0xbfedd488, 0x01f6f3b9, 0x40000000, 0x00000000, 0x3ff80000, 0x00000003},
    {32,0,123,__LINE__, 0xc000969c, 0xdf7ef118, 0x40080000, 0x00000000, 0x3ff80000, 0x00000003},
    {32,0,123,__LINE__, 0xc01d72a8, 0xbebf03b5, 0x40100000, 0x00000000, 0x3ff80000, 0x00000003},
    {32,0,123,__LINE__, 0x3fd9be6a, 0x232d2a1a, 0x00000000, 0x00000000, 0x3ff8cccc, 0xccccccd0},
    {32,0,123,__LINE__, 0xbfd84cf8, 0xe7ade6ca, 0x3ff00000, 0x00000000, 0x3ff8cccc, 0xccccccd0},
    {32,0,123,__LINE__, 0xbfec8cbc, 0xe0dd7c84, 0x40000000, 0x00000000, 0x3ff8cccc, 0xccccccd0},
    {32,0,123,__LINE__, 0xbffec362, 0xe83a924e, 0x40080000, 0x00000000, 0x3ff8cccc, 0xccccccd0},
    {32,0,123,__LINE__, 0xc01a33bf, 0xd946495c, 0x40100000, 0x00000000, 0x3ff8cccc, 0xccccccd0},
    {32,0,123,__LINE__, 0x3fdae846, 0x369db908, 0x00000000, 0x00000000, 0x3ff99999, 0x9999999d},
    {32,0,123,__LINE__, 0xbfd63eb7, 0xd493af10, 0x3ff00000, 0x00000000, 0x3ff99999, 0x9999999d},
    {32,0,123,__LINE__, 0xbfeb5b56, 0x002b29ec, 0x40000000, 0x00000000, 0x3ff99999, 0x9999999d},
    {32,0,123,__LINE__, 0xbffca27d, 0x8b11089e, 0x40080000, 0x00000000, 0x3ff99999, 0x9999999d},
    {32,0,123,__LINE__, 0xc0176cea, 0xf25a92d4, 0x40100000, 0x00000000, 0x3ff99999, 0x9999999d},
    {32,0,123,__LINE__, 0x3fdbf803, 0x95addba6, 0x00000000, 0x00000000, 0x3ffa6666, 0x6666666a},
    {32,0,123,__LINE__, 0xbfd4381f, 0xd07821fc, 0x3ff00000, 0x00000000, 0x3ffa6666, 0x6666666a},
    {32,0,123,__LINE__, 0xbfea3d0d, 0x50f130f6, 0x40000000, 0x00000000, 0x3ffa6666, 0x6666666a},
    {32,0,123,__LINE__, 0xbffabfd9, 0xa44c2351, 0x40080000, 0x00000000, 0x3ffa6666, 0x6666666a},
    {32,0,123,__LINE__, 0xc01509af, 0xd3f8859b, 0x40100000, 0x00000000, 0x3ffa6666, 0x6666666a},
    {32,0,123,__LINE__, 0x3fdcee02, 0xa7ac1a57, 0x00000000, 0x00000000, 0x3ffb3333, 0x33333337},
    {32,0,123,__LINE__, 0xbfd238f4, 0x6db73c42, 0x3ff00000, 0x00000000, 0x3ffb3333, 0x33333337},
    {32,0,123,__LINE__, 0xbfe92f18, 0xa36f036f, 0x40000000, 0x00000000, 0x3ffb3333, 0x33333337},
    {32,0,123,__LINE__, 0xbff91294, 0x95c98040, 0x40080000, 0x00000000, 0x3ffb3333, 0x33333337},
    {32,0,123,__LINE__, 0xc012f990, 0xf743eb12, 0x40100000, 0x00000000, 0x3ffb3333, 0x33333337},
    {32,0,123,__LINE__, 0x3fddcaa1, 0x9824527c, 0x00000000, 0x00000000, 0x3ffc0000, 0x00000004},
    {32,0,123,__LINE__, 0xbfd04117, 0xe92d38c7, 0x3ff00000, 0x00000000, 0x3ffc0000, 0x00000004},
    {32,0,123,__LINE__, 0xbfe82f15, 0x515092d3, 0x40000000, 0x00000000, 0x3ffc0000, 0x00000004},
    {32,0,123,__LINE__, 0xbff79340, 0x197e7e29, 0x40080000, 0x00000000, 0x3ffc0000, 0x00000004},
    {32,0,123,__LINE__, 0xc0112f2f, 0xb4d4a2eb, 0x40100000, 0x00000000, 0x3ffc0000, 0x00000004},
    {32,0,123,__LINE__, 0x3fde8e3d, 0xc0648706, 0x00000000, 0x00000000, 0x3ffccccc, 0xccccccd1},
    {32,0,123,__LINE__, 0xbfcca10c, 0xe708dff2, 0x3ff00000, 0x00000000, 0x3ffccccc, 0xccccccd1},
    {32,0,123,__LINE__, 0xbfe73af7, 0xcb0a0ff0, 0x40000000, 0x00000000, 0x3ffccccc, 0xccccccd1},
    {32,0,123,__LINE__, 0xbff63b9c, 0x612a1225, 0x40080000, 0x00000000, 0x3ffccccc, 0xccccccd1},
    {32,0,123,__LINE__, 0xc00f3f46, 0xaf2e44e8, 0x40100000, 0x00000000, 0x3ffccccc, 0xccccccd1},
    {32,0,123,__LINE__, 0x3fdf3934, 0xd9602e2a, 0x00000000, 0x00000000, 0x3ffd9999, 0x9999999e},
    {32,0,123,__LINE__, 0xbfc8cea4, 0x60c507ac, 0x3ff00000, 0x00000000, 0x3ffd9999, 0x9999999e},
    {32,0,123,__LINE__, 0xbfe650fe, 0x33d095b1, 0x40000000, 0x00000000, 0x3ffd9999, 0x9999999e},
    {32,0,123,__LINE__, 0xbff50660, 0xdc5a2a5e, 0x40080000, 0x00000000, 0x3ffd9999, 0x9999999e},
    {32,0,123,__LINE__, 0xc00c83ee, 0xd1748df8, 0x40100000, 0x00000000, 0x3ffd9999, 0x9999999e},
    {32,0,123,__LINE__, 0x3fdfcbe5, 0xfe2a7986, 0x00000000, 0x00000000, 0x3ffe6666, 0x6666666b},
    {32,0,123,__LINE__, 0xbfc50b3f, 0x93b80a84, 0x3ff00000, 0x00000000, 0x3ffe6666, 0x6666666b},
    {32,0,123,__LINE__, 0xbfe56fa5, 0x69532494, 0x40000000, 0x00000000, 0x3ffe6666, 0x6666666b},
    {32,0,123,__LINE__, 0xbff3ef10, 0x468232a6, 0x40080000, 0x00000000, 0x3ffe6666, 0x6666666b},
    {32,0,123,__LINE__, 0xc00a1d8e, 0xa9356bdc, 0x40100000, 0x00000000, 0x3ffe6666, 0x6666666b},
    {32,0,123,__LINE__, 0x3fe02359, 0x42e057b3, 0x00000000, 0x00000000, 0x3fff3333, 0x33333338},
    {32,0,123,__LINE__, 0xbfc15746, 0xfb241dc4, 0x3ff00000, 0x00000000, 0x3fff3333, 0x33333338},
    {32,0,123,__LINE__, 0xbfe4959f, 0xf96ce29c, 0x40000000, 0x00000000, 0x3fff3333, 0x33333338},
    {32,0,123,__LINE__, 0xbff2f1d5, 0x75c2f518, 0x40080000, 0x00000000, 0x3fff3333, 0x33333338},
    {32,0,123,__LINE__, 0xc007ffe0, 0x680bca3f, 0x40100000, 0x00000000, 0x3fff3333, 0x33333338},
};

#define EPSILON 0.000000001

union testcase
{
  double d_value;
  struct { long low, high; } l_value;
} ret, param1, param2, result;

int main ( void )
{
  int n, d;
  const unsigned N = sizeof ( yn_vec ) / sizeof ( yn_vec[0] );
  unsigned i;

  for ( i = 0; i < N; ++i )
  {
    param1.l_value.high = yn_vec[i][6];
    param1.l_value.low = yn_vec[i][7];
    param2.l_value.high = yn_vec[i][8];
    param2.l_value.low = yn_vec[i][9];
    ret.d_value = yn((int)param1.d_value,param2.d_value);
    result.l_value.high = yn_vec[i][4];
    result.l_value.low = yn_vec[i][5];
    if (yn_vec[i][1] == 13)
    {
      if ( ( ret.d_value <= result.d_value+(yn_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(yn_vec[i][2]*EPSILON) ) )
      {
        return ( -1 );
      }
    }
    else if (yn_vec[i][1] == 0)
    {
      if ( !( ret.d_value <= result.d_value+(yn_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(yn_vec[i][2]*EPSILON) ) )
      {
        return ( -1 );
      }
    }
    else
    {
      ; // Default
    }
  }

  return ( 0 );
}
