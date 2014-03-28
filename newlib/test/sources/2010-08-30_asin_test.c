#include <math.h>

long asin_vec[][8] = {
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff33333, 0x33333333},     /* 64.0000=f(-1.20000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff30a3d, 0x70a3d70a},     /* 64.0000=f(-1.19000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff2e147, 0xae147ae1},     /* 64.0000=f(-1.18000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff2b851, 0xeb851eb8},     /* 64.0000=f(-1.17000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff28f5c, 0x28f5c28f},     /* 64.0000=f(-1.16000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff26666, 0x66666666},     /* 64.0000=f(-1.15000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff23d70, 0xa3d70a3d},     /* 64.0000=f(-1.14000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff2147a, 0xe147ae14},     /* 64.0000=f(-1.13000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff1eb85, 0x1eb851eb},     /* 64.0000=f(-1.12000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff1c28f, 0x5c28f5c2},     /* 64.0000=f(-1.11000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff19999, 0x99999999},     /* 64.0000=f(-1.10000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff170a3, 0xd70a3d70},     /* 64.0000=f(-1.09000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff147ae, 0x147ae147},     /* 64.0000=f(-1.08000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff11eb8, 0x51eb851e},     /* 64.0000=f(-1.07000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff0f5c2, 0x8f5c28f5},     /* 64.0000=f(-1.06000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff0cccc, 0xcccccccc},     /* 64.0000=f(-1.05000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff0a3d7, 0x0a3d70a3},     /* 64.0000=f(-1.04000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff07ae1, 0x47ae147a},     /* 64.0000=f(-1.03000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff051eb, 0x851eb851},     /* 64.0000=f(-1.02000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff028f5, 0xc28f5c28},     /* 64.0000=f(-1.01000)*/
    {64, 0,123,__LINE__, 0xbff921fb, 0x4e9c057e, 0xbfefffff, 0xfffffffe},     /* -1.57079=f(-0.01000)*/
    {64, 0,123,__LINE__, 0xbff6de3c, 0x6f33d516, 0xbfefae14, 0x7ae147ac},     /* -1.42925=f(-0.99000)*/
    {64, 0,123,__LINE__, 0xbff5ed69, 0x0583be02, 0xbfef5c28, 0xf5c28f5a},     /* -1.37046=f(-0.98000)*/
    {64, 0,123,__LINE__, 0xbff53425, 0x38981ec3, 0xbfef0a3d, 0x70a3d708},     /* -1.32523=f(-0.97000)*/
    {64, 0,123,__LINE__, 0xbff4978f, 0xa3269edd, 0xbfeeb851, 0xeb851eb6},     /* -1.28700=f(-0.96000)*/
    {64, 0,123,__LINE__, 0xbff40d41, 0x159f3406, 0xbfee6666, 0x66666664},     /* -1.25323=f(-0.95000)*/
    {64, 0,123,__LINE__, 0xbff38fe4, 0xcb950b0e, 0xbfee147a, 0xe147ae12},     /* -1.22263=f(-0.94000)*/
    {64, 0,123,__LINE__, 0xbff31c50, 0xa48f3a72, 0xbfedc28f, 0x5c28f5c0},     /* -1.19441=f(-0.93000)*/
    {64, 0,123,__LINE__, 0xbff2b075, 0x29b17486, 0xbfed70a3, 0xd70a3d6e},     /* -1.16808=f(-0.92000)*/
    {64, 0,123,__LINE__, 0xbff24ae4, 0x3a7af145, 0xbfed1eb8, 0x51eb851c},     /* -1.14328=f(-0.91000)*/
    {64, 0,123,__LINE__, 0xbff1ea93, 0x705fa16e, 0xbfeccccc, 0xccccccca},     /* -1.11977=f(-0.90000)*/
    {64, 0,123,__LINE__, 0xbff18eb9, 0xcef862d5, 0xbfec7ae1, 0x47ae1478},     /* -1.09734=f(-0.89000)*/
    {64, 0,123,__LINE__, 0xbff136bb, 0x485f3d8e, 0xbfec28f5, 0xc28f5c26},     /* -1.07586=f(-0.88000)*/
    {64, 0,123,__LINE__, 0xbff0e21b, 0xd416ba13, 0xbfebd70a, 0x3d70a3d4},     /* -1.05520=f(-0.87000)*/
    {64, 0,123,__LINE__, 0xbff09076, 0xee9d82b8, 0xbfeb851e, 0xb851eb82},     /* -1.03527=f(-0.86000)*/
    {64, 0,123,__LINE__, 0xbff04179, 0xcba26d0b, 0xbfeb3333, 0x33333330},     /* -1.01598=f(-0.85000)*/
    {64, 0,123,__LINE__, 0xbfefe9be, 0x811df6c4, 0xbfeae147, 0xae147ade},     /* -0.99728=f(-0.84000)*/
    {64, 0,123,__LINE__, 0xbfef54d9, 0xa373fac6, 0xbfea8f5c, 0x28f5c28c},     /* -0.97910=f(-0.83000)*/
    {64, 0,123,__LINE__, 0xbfeec3e1, 0x0a73632b, 0xbfea3d70, 0xa3d70a3a},     /* -0.96141=f(-0.82000)*/
    {64, 0,123,__LINE__, 0xbfee367e, 0x7f21298e, 0xbfe9eb85, 0x1eb851e8},     /* -0.94415=f(-0.81000)*/
    {64, 0,123,__LINE__, 0xbfedac67, 0x0561bb49, 0xbfe99999, 0x99999996},     /* -0.92729=f(-0.80000)*/
    {64, 0,123,__LINE__, 0xbfed2558, 0xe9188de5, 0xbfe947ae, 0x147ae144},     /* -0.91080=f(-0.79000)*/
    {64, 0,123,__LINE__, 0xbfeca11a, 0x353bd845, 0xbfe8f5c2, 0x8f5c28f2},     /* -0.89466=f(-0.78000)*/
    {64, 0,123,__LINE__, 0xbfec1f77, 0x7a9974eb, 0xbfe8a3d7, 0x0a3d70a0},     /* -0.87884=f(-0.77000)*/
    {64, 0,123,__LINE__, 0xbfeba042, 0xd36663d4, 0xbfe851eb, 0x851eb84e},     /* -0.86331=f(-0.76000)*/
    {64, 0,123,__LINE__, 0xbfeb2353, 0x15c680d6, 0xbfe7ffff, 0xfffffffc},     /* -0.84806=f(-0.75000)*/
    {64, 0,123,__LINE__, 0xbfeaa883, 0x2b0b051f, 0xbfe7ae14, 0x7ae147aa},     /* -0.83307=f(-0.74000)*/
    {64, 0,123,__LINE__, 0xbfea2fb1, 0x83f1f795, 0xbfe75c28, 0xf5c28f58},     /* -0.81832=f(-0.73000)*/
    {64, 0,123,__LINE__, 0xbfe9b8bf, 0xa40885b7, 0xbfe70a3d, 0x70a3d706},     /* -0.80380=f(-0.72000)*/
    {64, 0,123,__LINE__, 0xbfe94391, 0xbfac8e45, 0xbfe6b851, 0xeb851eb4},     /* -0.78949=f(-0.71000)*/
    {64, 0,123,__LINE__, 0xbfe8d00e, 0x692afd8f, 0xbfe66666, 0x66666662},     /* -0.77539=f(-0.70000)*/
    {64, 0,123,__LINE__, 0xbfe85e1e, 0x4a3a04ec, 0xbfe6147a, 0xe147ae10},     /* -0.76148=f(-0.69000)*/
    {64, 0,123,__LINE__, 0xbfe7edab, 0xe7a11ff6, 0xbfe5c28f, 0x5c28f5be},     /* -0.74776=f(-0.68000)*/
    {64, 0,123,__LINE__, 0xbfe77ea3, 0x6d51891f, 0xbfe570a3, 0xd70a3d6c},     /* -0.73420=f(-0.67000)*/
    {64, 0,123,__LINE__, 0xbfe710f2, 0x8188f806, 0xbfe51eb8, 0x51eb851a},     /* -0.72081=f(-0.66000)*/
    {64, 0,123,__LINE__, 0xbfe6a488, 0x1ddc9b7c, 0xbfe4cccc, 0xccccccc8},     /* -0.70758=f(-0.65000)*/
    {64, 0,123,__LINE__, 0xbfe63954, 0x6d3fd534, 0xbfe47ae1, 0x47ae1476},     /* -0.69449=f(-0.64000)*/
    {64, 0,123,__LINE__, 0xbfe5cf48, 0xae44b71e, 0xbfe428f5, 0xc28f5c24},     /* -0.68155=f(-0.63000)*/
    {63, 0,123,__LINE__, 0xbfe56657, 0x18f62b90, 0xbfe3d70a, 0x3d70a3d2},     /* -0.66874=f(-0.62000)*/
    {64, 0,123,__LINE__, 0xbfe4fe72, 0xc7c6f173, 0xbfe3851e, 0xb851eb80},     /* -0.65606=f(-0.61000)*/
    {64, 0,123,__LINE__, 0xbfe4978f, 0xa3269eda, 0xbfe33333, 0x3333332e},     /* -0.64350=f(-0.60000)*/
    {64, 0,123,__LINE__, 0xbfe431a2, 0x4f5fc8de, 0xbfe2e147, 0xae147adc},     /* -0.63105=f(-0.59000)*/
    {64, 0,123,__LINE__, 0xbfe3cca0, 0x1c71184b, 0xbfe28f5c, 0x28f5c28a},     /* -0.61872=f(-0.58000)*/
    {64, 0,123,__LINE__, 0xbfe3687e, 0xf79f207e, 0xbfe23d70, 0xa3d70a38},     /* -0.60650=f(-0.57000)*/
    {64, 0,123,__LINE__, 0xbfe30535, 0x5e86c36c, 0xbfe1eb85, 0x1eb851e6},     /* -0.59438=f(-0.56000)*/
    {64, 0,123,__LINE__, 0xbfe2a2ba, 0x538032e9, 0xbfe19999, 0x99999994},     /* -0.58236=f(-0.55000)*/
    {64, 0,123,__LINE__, 0xbfe24105, 0x53298498, 0xbfe147ae, 0x147ae142},     /* -0.57043=f(-0.54000)*/
    {64, 0,123,__LINE__, 0xbfe1e00e, 0x4af594fc, 0xbfe0f5c2, 0x8f5c28f0},     /* -0.55860=f(-0.53000)*/
    {64, 0,123,__LINE__, 0xbfe17fcd, 0x90a0d198, 0xbfe0a3d7, 0x0a3d709e},     /* -0.54685=f(-0.52000)*/
    {64, 0,123,__LINE__, 0xbfe1203b, 0xda719c32, 0xbfe051eb, 0x851eb84c},     /* -0.53518=f(-0.51000)*/
    {63, 0,123,__LINE__, 0xbfe0c152, 0x382d735f, 0xbfdfffff, 0xfffffff4},     /* -0.52359=f(-0.50000)*/
    {61, 0,123,__LINE__, 0xbfe0630a, 0x0caf0114, 0xbfdf5c28, 0xf5c28f50},     /* -0.51208=f(-0.49000)*/
    {64, 0,123,__LINE__, 0xbfe0055d, 0x080bb62d, 0xbfdeb851, 0xeb851eac},     /* -0.50065=f(-0.48000)*/
    {64, 0,123,__LINE__, 0xbfdf508a, 0x44738570, 0xbfde147a, 0xe147ae08},     /* -0.48929=f(-0.47000)*/
    {64, 0,123,__LINE__, 0xbfde9779, 0x2c522bd2, 0xbfdd70a3, 0xd70a3d64},     /* -0.47799=f(-0.46000)*/
    {64, 0,123,__LINE__, 0xbfdddf7b, 0xba8753be, 0xbfdccccc, 0xccccccc0},     /* -0.46676=f(-0.45000)*/
    {64, 0,123,__LINE__, 0xbfdd2887, 0x569581bb, 0xbfdc28f5, 0xc28f5c1c},     /* -0.45559=f(-0.44000)*/
    {64, 0,123,__LINE__, 0xbfdc7291, 0xd50fd810, 0xbfdb851e, 0xb851eb78},     /* -0.44449=f(-0.43000)*/
    {64, 0,123,__LINE__, 0xbfdbbd91, 0x70937b71, 0xbfdae147, 0xae147ad4},     /* -0.43344=f(-0.42000)*/
    {60, 0,123,__LINE__, 0xbfdb097c, 0xc349e2b8, 0xbfda3d70, 0xa3d70a30},     /* -0.42245=f(-0.41000)*/
    {64, 0,123,__LINE__, 0xbfda564a, 0xc0e73a24, 0xbfd99999, 0x9999998c},     /* -0.41151=f(-0.40000)*/
    {64, 0,123,__LINE__, 0xbfd9a3f2, 0xb11964dc, 0xbfd8f5c2, 0x8f5c28e8},     /* -0.40063=f(-0.39000)*/
    {64, 0,123,__LINE__, 0xbfd8f26c, 0x2a5d5e0e, 0xbfd851eb, 0x851eb844},     /* -0.38979=f(-0.38000)*/
    {64, 0,123,__LINE__, 0xbfd841af, 0x0d31cc44, 0xbfd7ae14, 0x7ae147a0},     /* -0.37900=f(-0.37000)*/
    {64, 0,123,__LINE__, 0xbfd791b3, 0x7f9e8a16, 0xbfd70a3d, 0x70a3d6fc},     /* -0.36826=f(-0.36000)*/
    {64, 0,123,__LINE__, 0xbfd6e271, 0xe909bbd5, 0xbfd66666, 0x66666658},     /* -0.35757=f(-0.35000)*/
    {64, 0,123,__LINE__, 0xbfd633e2, 0xee53c5a9, 0xbfd5c28f, 0x5c28f5b4},     /* -0.34691=f(-0.34000)*/
    {64, 0,123,__LINE__, 0xbfd585ff, 0x6e341c2e, 0xbfd51eb8, 0x51eb8510},     /* -0.33630=f(-0.33000)*/
    {64, 0,123,__LINE__, 0xbfd4d8c0, 0x7dd17d6d, 0xbfd47ae1, 0x47ae146c},     /* -0.32572=f(-0.32000)*/
    {64, 0,123,__LINE__, 0xbfd42c1f, 0x6590a36d, 0xbfd3d70a, 0x3d70a3c8},     /* -0.31519=f(-0.31000)*/
    {64, 0,123,__LINE__, 0xbfd38015, 0x9e14f6ef, 0xbfd33333, 0x33333324},     /* -0.30469=f(-0.30000)*/
    {64, 0,123,__LINE__, 0xbfd2d49c, 0xcd6f3123, 0xbfd28f5c, 0x28f5c280},     /* -0.29422=f(-0.29000)*/
    {64, 0,123,__LINE__, 0xbfd229ae, 0xc47638cc, 0xbfd1eb85, 0x1eb851dc},     /* -0.28379=f(-0.28000)*/
    {64, 0,123,__LINE__, 0xbfd17f45, 0x7c46da9a, 0xbfd147ae, 0x147ae138},     /* -0.27339=f(-0.27000)*/
    {64, 0,123,__LINE__, 0xbfd0d55b, 0x13e7479e, 0xbfd0a3d7, 0x0a3d7094},     /* -0.26302=f(-0.26000)*/
    {64, 0,123,__LINE__, 0xbfd02be9, 0xce0b87bd, 0xbfcfffff, 0xffffffe0},     /* -0.25268=f(-0.25000)*/
    {64, 0,123,__LINE__, 0xbfcf05d8, 0x1df0951e, 0xbfceb851, 0xeb851e98},     /* -0.24236=f(-0.24000)*/
    {64, 0,123,__LINE__, 0xbfcdb4b8, 0xb5036e78, 0xbfcd70a3, 0xd70a3d50},     /* -0.23207=f(-0.23000)*/
    {64, 0,123,__LINE__, 0xbfcc646a, 0xa44819b8, 0xbfcc28f5, 0xc28f5c08},     /* -0.22181=f(-0.22000)*/
    {64, 0,123,__LINE__, 0xbfcb14e3, 0x666821d7, 0xbfcae147, 0xae147ac0},     /* -0.21157=f(-0.21000)*/
    {63, 0,123,__LINE__, 0xbfc9c618, 0xaa4ae21b, 0xbfc99999, 0x99999978},     /* -0.20135=f(-0.20000)*/
    {64, 0,123,__LINE__, 0xbfc87800, 0x4fcac25f, 0xbfc851eb, 0x851eb830},     /* -0.19116=f(-0.19000)*/
    {64, 0,123,__LINE__, 0xbfc72a90, 0x648fbf03, 0xbfc70a3d, 0x70a3d6e8},     /* -0.18098=f(-0.18000)*/
    {64, 0,123,__LINE__, 0xbfc5ddbf, 0x210c49be, 0xbfc5c28f, 0x5c28f5a0},     /* -0.17082=f(-0.17000)*/
    {64, 0,123,__LINE__, 0xbfc49182, 0xe599c56f, 0xbfc47ae1, 0x47ae1458},     /* -0.16069=f(-0.16000)*/
    {64, 0,123,__LINE__, 0xbfc345d2, 0x37b20e8f, 0xbfc33333, 0x33333310},     /* -0.15056=f(-0.15000)*/
    {64, 0,123,__LINE__, 0xbfc1faa3, 0xbf43ad16, 0xbfc1eb85, 0x1eb851c8},     /* -0.14046=f(-0.14000)*/
    {64, 0,123,__LINE__, 0xbfc0afee, 0x441e734b, 0xbfc0a3d7, 0x0a3d7080},     /* -0.13036=f(-0.13000)*/
    {64, 0,123,__LINE__, 0xbfbecb51, 0x56ece03e, 0xbfbeb851, 0xeb851e71},     /* -0.12028=f(-0.12000)*/
    {64, 0,123,__LINE__, 0xbfbc3793, 0xeaf6762d, 0xbfbc28f5, 0xc28f5be2},     /* -0.11022=f(-0.11000)*/
    {64, 0,123,__LINE__, 0xbfb9a492, 0x7603783d, 0xbfb99999, 0x99999953},     /* -0.10016=f(-0.00100)*/
    {64, 0,123,__LINE__, 0xbfb7123b, 0x567d55f2, 0xbfb70a3d, 0x70a3d6c4},     /* -0.09012=f(-0.09000)*/
    {64, 0,123,__LINE__, 0xbfb4807d, 0x13203146, 0xbfb47ae1, 0x47ae1435},     /* -0.08008=f(-0.08000)*/
    {64, 0,123,__LINE__, 0xbfb1ef46, 0x56a8d246, 0xbfb1eb85, 0x1eb851a6},     /* -0.07005=f(-0.07000)*/
    {64, 0,123,__LINE__, 0xbfaebd0b, 0xd734d81c, 0xbfaeb851, 0xeb851e2d},     /* -0.06003=f(-0.06000)*/
    {64, 0,123,__LINE__, 0xbfa99c55, 0x703285cd, 0xbfa99999, 0x9999990e},     /* -0.05002=f(-0.05000)*/
    {64, 0,123,__LINE__, 0xbfa47c47, 0x73aebccd, 0xbfa47ae1, 0x47ae13ef},     /* -0.04001=f(-0.04000)*/
    {64, 0,123,__LINE__, 0xbf9eb980, 0x084226ee, 0xbf9eb851, 0xeb851da0},     /* -0.03000=f(-0.03000)*/
    {64, 0,123,__LINE__, 0xbf947b3a, 0xc64bdf81, 0xbf947ae1, 0x47ae1362},     /* -0.02000=f(-0.02000)*/
    {64, 0,123,__LINE__, 0xbf847af7, 0xa68f8cbd, 0xbf847ae1, 0x47ae1249},     /* -0.01000=f(-0.00010)*/
    {64, 0,123,__LINE__, 0x3cd19000, 0x00000000, 0x3cd19000, 0x00000000},     /* 9.74915e-16=f(9.74915e-16)*/
    {64, 0,123,__LINE__, 0x3f847af7, 0xa68f9121, 0x3f847ae1, 0x47ae16ad},     /* 0.01000=f(0.01000)*/
    {64, 0,123,__LINE__, 0x3f947b3a, 0xc64be1b3, 0x3f947ae1, 0x47ae1594},     /* 0.02000=f(0.02000)*/
    {64, 0,123,__LINE__, 0x3f9eb980, 0x08422920, 0x3f9eb851, 0xeb851fd2},     /* 0.03000=f(0.03000)*/
    {64, 0,123,__LINE__, 0x3fa47c47, 0x73aebde6, 0x3fa47ae1, 0x47ae1508},     /* 0.04001=f(0.04000)*/
    {64, 0,123,__LINE__, 0x3fa99c55, 0x703286e6, 0x3fa99999, 0x99999a27},     /* 0.05002=f(0.05000)*/
    {64, 0,123,__LINE__, 0x3faebd0b, 0xd734d935, 0x3faeb851, 0xeb851f46},     /* 0.06003=f(0.06000)*/
    {64, 0,123,__LINE__, 0x3fb1ef46, 0x56a8d2d3, 0x3fb1eb85, 0x1eb85232},     /* 0.07005=f(0.07000)*/
    {64, 0,123,__LINE__, 0x3fb4807d, 0x132031d3, 0x3fb47ae1, 0x47ae14c1},     /* 0.08008=f(0.08000)*/
    {64, 0,123,__LINE__, 0x3fb7123b, 0x567d567e, 0x3fb70a3d, 0x70a3d750},     /* 0.09012=f(0.09000)*/
    {64, 0,123,__LINE__, 0x3fb9a492, 0x760378c9, 0x3fb99999, 0x999999df},     /* 0.10016=f(0.10000)*/
    {64, 0,123,__LINE__, 0x3fbc3793, 0xeaf676ba, 0x3fbc28f5, 0xc28f5c6e},     /* 0.11022=f(0.11000)*/
    {64, 0,123,__LINE__, 0x3fbecb51, 0x56ece0cb, 0x3fbeb851, 0xeb851efd},     /* 0.12028=f(0.12000)*/
    {64, 0,123,__LINE__, 0x3fc0afee, 0x441e7392, 0x3fc0a3d7, 0x0a3d70c6},     /* 0.13036=f(0.13000)*/
    {64, 0,123,__LINE__, 0x3fc1faa3, 0xbf43ad5d, 0x3fc1eb85, 0x1eb8520e},     /* 0.14046=f(0.14000)*/
    {64, 0,123,__LINE__, 0x3fc345d2, 0x37b20ed6, 0x3fc33333, 0x33333356},     /* 0.15056=f(0.15000)*/
    {64, 0,123,__LINE__, 0x3fc49182, 0xe599c5b6, 0x3fc47ae1, 0x47ae149e},     /* 0.16069=f(0.16000)*/
    {64, 0,123,__LINE__, 0x3fc5ddbf, 0x210c4a05, 0x3fc5c28f, 0x5c28f5e6},     /* 0.17082=f(0.17000)*/
    {64, 0,123,__LINE__, 0x3fc72a90, 0x648fbf4a, 0x3fc70a3d, 0x70a3d72e},     /* 0.18098=f(0.18000)*/
    {64, 0,123,__LINE__, 0x3fc87800, 0x4fcac2a6, 0x3fc851eb, 0x851eb876},     /* 0.19116=f(0.19000)*/
    {64, 0,123,__LINE__, 0x3fc9c618, 0xaa4ae262, 0x3fc99999, 0x999999be},     /* 0.20135=f(0.20000)*/
    {64, 0,123,__LINE__, 0x3fcb14e3, 0x6668221e, 0x3fcae147, 0xae147b06},     /* 0.21157=f(0.21000)*/
    {64, 0,123,__LINE__, 0x3fcc646a, 0xa44819ff, 0x3fcc28f5, 0xc28f5c4e},     /* 0.22181=f(0.22000)*/
    {64, 0,123,__LINE__, 0x3fcdb4b8, 0xb5036ec0, 0x3fcd70a3, 0xd70a3d96},     /* 0.23207=f(0.23000)*/
    {64, 0,123,__LINE__, 0x3fcf05d8, 0x1df09566, 0x3fceb851, 0xeb851ede},     /* 0.24236=f(0.24000)*/
    {64, 0,123,__LINE__, 0x3fd02be9, 0xce0b87e1, 0x3fd00000, 0x00000013},     /* 0.25268=f(0.25000)*/
    {64, 0,123,__LINE__, 0x3fd0d55b, 0x13e747c2, 0x3fd0a3d7, 0x0a3d70b7},     /* 0.26302=f(0.26000)*/
    {64, 0,123,__LINE__, 0x3fd17f45, 0x7c46dabf, 0x3fd147ae, 0x147ae15b},     /* 0.27339=f(0.27000)*/
    {64, 0,123,__LINE__, 0x3fd229ae, 0xc47638f1, 0x3fd1eb85, 0x1eb851ff},     /* 0.28379=f(0.28000)*/
    {64, 0,123,__LINE__, 0x3fd2d49c, 0xcd6f3148, 0x3fd28f5c, 0x28f5c2a3},     /* 0.29422=f(0.29000)*/
    {64, 0,123,__LINE__, 0x3fd38015, 0x9e14f713, 0x3fd33333, 0x33333347},     /* 0.30469=f(0.30000)*/
    {64, 0,123,__LINE__, 0x3fd42c1f, 0x6590a392, 0x3fd3d70a, 0x3d70a3eb},     /* 0.31519=f(0.31000)*/
    {64, 0,123,__LINE__, 0x3fd4d8c0, 0x7dd17d92, 0x3fd47ae1, 0x47ae148f},     /* 0.32572=f(0.32000)*/
    {64, 0,123,__LINE__, 0x3fd585ff, 0x6e341c53, 0x3fd51eb8, 0x51eb8533},     /* 0.33630=f(0.33000)*/
    {64, 0,123,__LINE__, 0x3fd633e2, 0xee53c5ce, 0x3fd5c28f, 0x5c28f5d7},     /* 0.34691=f(0.34000)*/
    {63, 0,123,__LINE__, 0x3fd6e271, 0xe909bbfb, 0x3fd66666, 0x6666667b},     /* 0.35757=f(0.35000)*/
    {64, 0,123,__LINE__, 0x3fd791b3, 0x7f9e8a3c, 0x3fd70a3d, 0x70a3d71f},     /* 0.36826=f(0.36000)*/
    {64, 0,123,__LINE__, 0x3fd841af, 0x0d31cc69, 0x3fd7ae14, 0x7ae147c3},     /* 0.37900=f(0.37000)*/
    {64, 0,123,__LINE__, 0x3fd8f26c, 0x2a5d5e34, 0x3fd851eb, 0x851eb867},     /* 0.38979=f(0.38000)*/
    {63, 0,123,__LINE__, 0x3fd9a3f2, 0xb1196503, 0x3fd8f5c2, 0x8f5c290b},     /* 0.40063=f(0.39000)*/
    {64, 0,123,__LINE__, 0x3fda564a, 0xc0e73a4b, 0x3fd99999, 0x999999af},     /* 0.41151=f(0.40000)*/
    {64, 0,123,__LINE__, 0x3fdb097c, 0xc349e2de, 0x3fda3d70, 0xa3d70a53},     /* 0.42245=f(0.41000)*/
    {64, 0,123,__LINE__, 0x3fdbbd91, 0x70937b97, 0x3fdae147, 0xae147af7},     /* 0.43344=f(0.42000)*/
    {64, 0,123,__LINE__, 0x3fdc7291, 0xd50fd837, 0x3fdb851e, 0xb851eb9b},     /* 0.44449=f(0.43000)*/
    {64, 0,123,__LINE__, 0x3fdd2887, 0x569581e2, 0x3fdc28f5, 0xc28f5c3f},     /* 0.45559=f(0.44000)*/
    {64, 0,123,__LINE__, 0x3fdddf7b, 0xba8753e5, 0x3fdccccc, 0xcccccce3},     /* 0.46676=f(0.45000)*/
    {64, 0,123,__LINE__, 0x3fde9779, 0x2c522bfa, 0x3fdd70a3, 0xd70a3d87},     /* 0.47799=f(0.46000)*/
    {64, 0,123,__LINE__, 0x3fdf508a, 0x44738598, 0x3fde147a, 0xe147ae2b},     /* 0.48929=f(0.47000)*/
    {64, 0,123,__LINE__, 0x3fe0055d, 0x080bb641, 0x3fdeb851, 0xeb851ecf},     /* 0.50065=f(0.48000)*/
    {64, 0,123,__LINE__, 0x3fe0630a, 0x0caf0128, 0x3fdf5c28, 0xf5c28f73},     /* 0.51208=f(0.49000)*/
    {64, 0,123,__LINE__, 0x3fe0c152, 0x382d7372, 0x3fe00000, 0x0000000b},     /* 0.52359=f(0.50000)*/
    {64, 0,123,__LINE__, 0x3fe1203b, 0xda719c46, 0x3fe051eb, 0x851eb85d},     /* 0.53518=f(0.51000)*/
    {64, 0,123,__LINE__, 0x3fe17fcd, 0x90a0d1ac, 0x3fe0a3d7, 0x0a3d70af},     /* 0.54685=f(0.52000)*/
    {64, 0,123,__LINE__, 0x3fe1e00e, 0x4af59510, 0x3fe0f5c2, 0x8f5c2901},     /* 0.55860=f(0.53000)*/
    {64, 0,123,__LINE__, 0x3fe24105, 0x532984ac, 0x3fe147ae, 0x147ae153},     /* 0.57043=f(0.54000)*/
    {64, 0,123,__LINE__, 0x3fe2a2ba, 0x538032fd, 0x3fe19999, 0x999999a5},     /* 0.58236=f(0.55000)*/
    {63, 0,123,__LINE__, 0x3fe30535, 0x5e86c380, 0x3fe1eb85, 0x1eb851f7},     /* 0.59438=f(0.56000)*/
    {64, 0,123,__LINE__, 0x3fe3687e, 0xf79f2092, 0x3fe23d70, 0xa3d70a49},     /* 0.60650=f(0.57000)*/
    {64, 0,123,__LINE__, 0x3fe3cca0, 0x1c711860, 0x3fe28f5c, 0x28f5c29b},     /* 0.61872=f(0.58000)*/
    {64, 0,123,__LINE__, 0x3fe431a2, 0x4f5fc8f3, 0x3fe2e147, 0xae147aed},     /* 0.63105=f(0.59000)*/
    {64, 0,123,__LINE__, 0x3fe4978f, 0xa3269eef, 0x3fe33333, 0x3333333f},     /* 0.64350=f(0.60000)*/
    {64, 0,123,__LINE__, 0x3fe4fe72, 0xc7c6f189, 0x3fe3851e, 0xb851eb91},     /* 0.65606=f(0.61000)*/
    {64, 0,123,__LINE__, 0x3fe56657, 0x18f62ba7, 0x3fe3d70a, 0x3d70a3e3},     /* 0.66874=f(0.62000)*/
    {64, 0,123,__LINE__, 0x3fe5cf48, 0xae44b733, 0x3fe428f5, 0xc28f5c35},     /* 0.68155=f(0.63000)*/
    {64, 0,123,__LINE__, 0x3fe63954, 0x6d3fd54a, 0x3fe47ae1, 0x47ae1487},     /* 0.69449=f(0.64000)*/
    {64, 0,123,__LINE__, 0x3fe6a488, 0x1ddc9b92, 0x3fe4cccc, 0xccccccd9},     /* 0.70758=f(0.65000)*/
    {64, 0,123,__LINE__, 0x3fe710f2, 0x8188f81c, 0x3fe51eb8, 0x51eb852b},     /* 0.72081=f(0.66000)*/
    {64, 0,123,__LINE__, 0x3fe77ea3, 0x6d518936, 0x3fe570a3, 0xd70a3d7d},     /* 0.73420=f(0.67000)*/
    {64, 0,123,__LINE__, 0x3fe7edab, 0xe7a1200e, 0x3fe5c28f, 0x5c28f5cf},     /* 0.74776=f(0.68000)*/
    {63, 0,123,__LINE__, 0x3fe85e1e, 0x4a3a0502, 0x3fe6147a, 0xe147ae21},     /* 0.76148=f(0.69000)*/
    {63, 0,123,__LINE__, 0x3fe8d00e, 0x692afda6, 0x3fe66666, 0x66666673},     /* 0.77539=f(0.70000)*/
    {64, 0,123,__LINE__, 0x3fe94391, 0xbfac8e5d, 0x3fe6b851, 0xeb851ec5},     /* 0.78949=f(0.71000)*/
    {64, 0,123,__LINE__, 0x3fe9b8bf, 0xa40885cf, 0x3fe70a3d, 0x70a3d717},     /* 0.80380=f(0.72000)*/
    {64, 0,123,__LINE__, 0x3fea2fb1, 0x83f1f7af, 0x3fe75c28, 0xf5c28f69},     /* 0.81832=f(0.73000)*/
    {64, 0,123,__LINE__, 0x3feaa883, 0x2b0b0538, 0x3fe7ae14, 0x7ae147bb},     /* 0.83307=f(0.74000)*/
    {64, 0,123,__LINE__, 0x3feb2353, 0x15c680ef, 0x3fe80000, 0x0000000d},     /* 0.84806=f(0.75000)*/
    {64, 0,123,__LINE__, 0x3feba042, 0xd36663ef, 0x3fe851eb, 0x851eb85f},     /* 0.86331=f(0.76000)*/
    {64, 0,123,__LINE__, 0x3fec1f77, 0x7a997507, 0x3fe8a3d7, 0x0a3d70b1},     /* 0.87884=f(0.77000)*/
    {64, 0,123,__LINE__, 0x3feca11a, 0x353bd860, 0x3fe8f5c2, 0x8f5c2903},     /* 0.89466=f(0.78000)*/
    {64, 0,123,__LINE__, 0x3fed2558, 0xe9188e00, 0x3fe947ae, 0x147ae155},     /* 0.91080=f(0.79000)*/
    {64, 0,123,__LINE__, 0x3fedac67, 0x0561bb65, 0x3fe99999, 0x999999a7},     /* 0.92729=f(0.80000)*/
    {64, 0,123,__LINE__, 0x3fee367e, 0x7f2129ab, 0x3fe9eb85, 0x1eb851f9},     /* 0.94415=f(0.81000)*/
    {64, 0,123,__LINE__, 0x3feec3e1, 0x0a73634a, 0x3fea3d70, 0xa3d70a4b},     /* 0.96141=f(0.82000)*/
    {64, 0,123,__LINE__, 0x3fef54d9, 0xa373fae5, 0x3fea8f5c, 0x28f5c29d},     /* 0.97910=f(0.83000)*/
    {64, 0,123,__LINE__, 0x3fefe9be, 0x811df6e3, 0x3feae147, 0xae147aef},     /* 0.99728=f(0.84000)*/
    {64, 0,123,__LINE__, 0x3ff04179, 0xcba26d1c, 0x3feb3333, 0x33333341},     /* 1.01598=f(0.85000)*/
    {64, 0,123,__LINE__, 0x3ff09076, 0xee9d82c8, 0x3feb851e, 0xb851eb93},     /* 1.03527=f(0.86000)*/
    {64, 0,123,__LINE__, 0x3ff0e21b, 0xd416ba24, 0x3febd70a, 0x3d70a3e5},     /* 1.05520=f(0.87000)*/
    {64, 0,123,__LINE__, 0x3ff136bb, 0x485f3da0, 0x3fec28f5, 0xc28f5c37},     /* 1.07586=f(0.88000)*/
    {64, 0,123,__LINE__, 0x3ff18eb9, 0xcef862e8, 0x3fec7ae1, 0x47ae1489},     /* 1.09734=f(0.89000)*/
    {64, 0,123,__LINE__, 0x3ff1ea93, 0x705fa182, 0x3feccccc, 0xccccccdb},     /* 1.11977=f(0.90000)*/
    {64, 0,123,__LINE__, 0x3ff24ae4, 0x3a7af15a, 0x3fed1eb8, 0x51eb852d},     /* 1.14328=f(0.91000)*/
    {64, 0,123,__LINE__, 0x3ff2b075, 0x29b1749b, 0x3fed70a3, 0xd70a3d7f},     /* 1.16808=f(0.92000)*/
    {64, 0,123,__LINE__, 0x3ff31c50, 0xa48f3a89, 0x3fedc28f, 0x5c28f5d1},     /* 1.19441=f(0.93000)*/
    {64, 0,123,__LINE__, 0x3ff38fe4, 0xcb950b26, 0x3fee147a, 0xe147ae23},     /* 1.22263=f(0.94000)*/
    {64, 0,123,__LINE__, 0x3ff40d41, 0x159f3421, 0x3fee6666, 0x66666675},     /* 1.25323=f(0.95000)*/
    {64, 0,123,__LINE__, 0x3ff4978f, 0xa3269efb, 0x3feeb851, 0xeb851ec7},     /* 1.28700=f(0.96000)*/
    {64, 0,123,__LINE__, 0x3ff53425, 0x38981ee6, 0x3fef0a3d, 0x70a3d719},     /* 1.32523=f(0.97000)*/
    {64, 0,123,__LINE__, 0x3ff5ed69, 0x0583be2d, 0x3fef5c28, 0xf5c28f6b},     /* 1.37046=f(0.98000)*/
    {64, 0,123,__LINE__, 0x3ff6de3c, 0x6f33d552, 0x3fefae14, 0x7ae147bd},     /* 1.42925=f(0.99000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff00000, 0x00000007},     /* 64.0000=f(1.00000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff028f5, 0xc28f5c30},     /* 64.0000=f(1.01000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff051eb, 0x851eb859},     /* 64.0000=f(1.02000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff07ae1, 0x47ae1482},     /* 64.0000=f(1.03000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff0a3d7, 0x0a3d70ab},     /* 64.0000=f(1.04000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff0cccc, 0xccccccd4},     /* 64.0000=f(1.05000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff0f5c2, 0x8f5c28fd},     /* 64.0000=f(1.06000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff11eb8, 0x51eb8526},     /* 64.0000=f(1.07000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff147ae, 0x147ae14f},     /* 64.0000=f(1.08000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff170a3, 0xd70a3d78},     /* 64.0000=f(1.09000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff19999, 0x999999a1},     /* 64.0000=f(1.10000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff1c28f, 0x5c28f5ca},     /* 64.0000=f(1.11000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff1eb85, 0x1eb851f3},     /* 64.0000=f(1.12000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff2147a, 0xe147ae1c},     /* 64.0000=f(1.13000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff23d70, 0xa3d70a45},     /* 64.0000=f(1.14000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff26666, 0x6666666e},     /* 64.0000=f(1.15000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff28f5c, 0x28f5c297},     /* 64.0000=f(1.16000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff2b851, 0xeb851ec0},     /* 64.0000=f(1.17000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff2e147, 0xae147ae9},     /* 64.0000=f(1.18000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff30a3d, 0x70a3d712},     /* 64.0000=f(1.19000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc01921fb, 0x54442d18},     /* 64.0000=f(-6.28318)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc012d97c, 0x7f3321d2},     /* 64.0000=f(-4.71238)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc00921fb, 0x54442d18},     /* 64.0000=f(-3.14159)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff921fb, 0x54442d18},     /* 64.0000=f(-1.57079)*/
    {64, 0,123,__LINE__, 0x00000000, 0x00000000, 0x00000000, 0x00000000},     /* 0.00000=f(0.00000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x3ff921fb, 0x54442d18},     /* 64.0000=f(1.57079)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x400921fb, 0x54442d18},     /* 64.0000=f(3.14159)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x4012d97c, 0x7f3321d2},     /* 64.0000=f(4.71238)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc03e0000, 0x00000000},     /* 64.0000=f(-30.0000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc03c4ccc, 0xcccccccd},     /* 64.0000=f(-28.3000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc03a9999, 0x9999999a},     /* 64.0000=f(-26.6000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc038e666, 0x66666667},     /* 64.0000=f(-24.9000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0373333, 0x33333334},     /* 64.0000=f(-23.2000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0358000, 0x00000001},     /* 64.0000=f(-21.5000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc033cccc, 0xccccccce},     /* 64.0000=f(-19.8000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0321999, 0x9999999b},     /* 64.0000=f(-18.1000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0306666, 0x66666668},     /* 64.0000=f(-16.4000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc02d6666, 0x6666666a},     /* 64.0000=f(-14.7000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc02a0000, 0x00000004},     /* 64.0000=f(-13.0000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0269999, 0x9999999e},     /* 64.0000=f(-11.3000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0233333, 0x33333338},     /* 64.0000=f(-9.60000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc01f9999, 0x999999a3},     /* 64.0000=f(-7.90000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc018cccc, 0xccccccd6},     /* 64.0000=f(-6.20000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0120000, 0x00000009},     /* 64.0000=f(-4.50000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xc0066666, 0x66666678},     /* 64.0000=f(-2.80000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0xbff19999, 0x999999bd},     /* 64.0000=f(-1.10000)*/
    {60, 0,123,__LINE__, 0x3fe4978f, 0xa3269e87, 0x3fe33333, 0x333332ec},     /* 0.64350=f(0.60000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40026666, 0x66666654},     /* 64.0000=f(2.30000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x400fffff, 0xffffffee},     /* 64.0000=f(4.00000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x4016cccc, 0xccccccc4},     /* 64.0000=f(5.70000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x401d9999, 0x99999991},     /* 64.0000=f(7.40000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40223333, 0x3333332f},     /* 64.0000=f(9.10000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40259999, 0x99999995},     /* 64.0000=f(10.8000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x4028ffff, 0xfffffffb},     /* 64.0000=f(12.5000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x402c6666, 0x66666661},     /* 64.0000=f(14.2000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x402fcccc, 0xccccccc7},     /* 64.0000=f(15.9000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40319999, 0x99999997},     /* 64.0000=f(17.6000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40334ccc, 0xccccccca},     /* 64.0000=f(19.3000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x4034ffff, 0xfffffffd},     /* 64.0000=f(21.0000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x4036b333, 0x33333330},     /* 64.0000=f(22.7000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x40386666, 0x66666663},     /* 64.0000=f(24.4000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x403a1999, 0x99999996},     /* 64.0000=f(26.1000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x403bcccc, 0xccccccc9},     /* 64.0000=f(27.8000)*/
    {64,13, 37,__LINE__, 0x40500000, 0x00000000, 0x403d7fff, 0xfffffffc},     /* 64.0000=f(29.5000)*/
};

#define EPSILON 0.0001

union testcase
{
  double d_value;
  struct { long low, high; } l_value;
} ret, param, result;

int main ( void )
{
  int n, d;
  const unsigned N = sizeof ( asin_vec ) / sizeof ( asin_vec[0] );
  unsigned i;

  for ( i = 0; i < N; ++i )
  {
    param.l_value.high = asin_vec[i][6];
    param.l_value.low = asin_vec[i][7];
    ret.d_value = asin(param.d_value);
    result.l_value.high = asin_vec[i][4];
    result.l_value.low = asin_vec[i][5];
    if (asin_vec[i][1] == 13)
    {
      if ( ( ret.d_value <= result.d_value+(asin_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(asin_vec[i][2]*EPSILON) ) )
      {
        return ( -1 );
      }
    }
    else if (asin_vec[i][1] == 0)
    {
      if ( !( ret.d_value <= result.d_value+(asin_vec[i][2]*EPSILON) && ret.d_value >= result.d_value-(asin_vec[i][2]*EPSILON) ) )
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
