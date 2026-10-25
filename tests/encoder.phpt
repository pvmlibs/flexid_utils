--TEST--
Custom alphabet encoder test
--FILE--
<?php
    $alphabet = 'R9jHJSAcLBfFroKqTzXvgxtNbsp83DEkVMIn-y0m5hdG_P47awQl26uYZUeW1iCO';

    for ($i = 2; $i <= \strlen($alphabet); $i++) {
        $encodedIds = [];
        $expected = 0;
        $encoder = new \FlexId\Encoder(\substr($alphabet, 0, $i));
        $incrementalHash = hash_init('sha256');
        for ($j = 1; $j < 1000; $j++) {
            $encoded = $encoder->encode($j);
            $encodedIds[$encoded] = 1;
            $expected++;
            hash_update($incrementalHash, $encoded);

            if ($j !== $encoder->decode($encoded)) {
                var_dump("failed to decode $j");
            }
            $negative = -$j;
            $encoded = $encoder->encode($negative);
            $encodedIds[$encoded] = 1;
            $expected++;
            hash_update($incrementalHash, $encoded);
            if ($negative !== $encoder->decode($encoded)) {
                var_dump("failed to decode $negative");
            }
        }
        $encoded = $encoder->encode(0);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if (0 !== $encoder->decode($encoded)) {
            var_dump("failed to decode 0");
        }
        $encoded = $encoder->encode(PHP_INT_MAX);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if (PHP_INT_MAX !== $encoder->decode($encoded)) {
            var_dump("failed to decode PHP_INT_MAX");
        }
        $encoded = $encoder->encode(PHP_INT_MIN);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if (PHP_INT_MIN !== $encoder->decode($encoded)) {
            var_dump("failed to decode PHP_INT_MIN");
        }
        var_dump(hash_final($incrementalHash));
        var_dump(\count($encodedIds) === $expected);
    }

    for ($i = 2; $i <= \strlen($alphabet); $i++) {
        $encodedIds = [];
        $expected = 0;
        $encoder = new \FlexId\Encoder(\substr($alphabet, 0, $i));
        $incrementalHash = hash_init('sha256');
        for ($j = 1; $j < 1000; $j++) {
            $k = 1000 - $j;
            $encoded = $encoder->doubleEncode($j, $k);
            $encodedIds[$encoded] = 1;
            $expected++;
            hash_update($incrementalHash, $encoded);

            if ([$j, $k] !== $encoder->doubleDecode($encoded)) {
                var_dump("failed to decode $j");
            }
            $negative = -$j;
            $encoded = $encoder->doubleEncode($negative, -$k);
            $encodedIds[$encoded] = 1;
            $expected++;
            hash_update($incrementalHash, $encoded);
            if ([$negative, -$k] !== $encoder->doubleDecode($encoded)) {
                var_dump("failed to decode $negative");
            }
        }
        $encoded = $encoder->doubleEncode(0, 0);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if ([0, 0] !== $encoder->doubleDecode($encoded)) {
            var_dump("failed to decode 0");
        }
        $encoded = $encoder->doubleEncode(PHP_INT_MAX, PHP_INT_MAX - 1);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if ([PHP_INT_MAX, PHP_INT_MAX - 1] !== $encoder->doubleDecode($encoded)) {
            var_dump("failed to decode PHP_INT_MAX");
        }
        $encoded = $encoder->doubleEncode(PHP_INT_MIN, PHP_INT_MIN + 1);
        $encodedIds[$encoded] = 1;
        $expected++;
        hash_update($incrementalHash, $encoded);
        if ([PHP_INT_MIN, PHP_INT_MIN + 1] !== $encoder->doubleDecode($encoded)) {
            var_dump("failed to decode PHP_INT_MIN");
        }
        var_dump(hash_final($incrementalHash));
        var_dump(\count($encodedIds) === $expected);
    }
?>
--EXPECT--
string(64) "baf33b0d44ca96854fedcb2fa6584cd7a9acf1bd84a665ca3b2ce1ff6d4e5676"
bool(true)
string(64) "2c743b494926122d7d8f957ea9ebf0a240d725f6a5bc7760d6b8b07eb5560683"
bool(true)
string(64) "39ab817489a27d9fdcba88b7a5b94c05296753645ede535462eb9e72bc9c1e00"
bool(true)
string(64) "4392bd12978a5740df4b98a55e8378d5f608773363daba74903e358906c496cb"
bool(true)
string(64) "784ce0f20367943bf4b30d6c75d8e5ef576bef85770848ec87f15f10933af6e2"
bool(true)
string(64) "a41c388bc87c5256f26f16a2229a0171ac42237022f2de5742e5a918d4232c3f"
bool(true)
string(64) "4f4a9b547ddf1185820c2203cafc725d6e02626afab03216bd4615bc2ae6e959"
bool(true)
string(64) "9a72d4560302d4fa392bb1461c37d5e5f094ba1fda61762750eccc3a9170f190"
bool(true)
string(64) "c39771f22f510a33af89c0ba01a8c367e55e35fb48c8b5ffbc5fa48dd06d0908"
bool(true)
string(64) "8695adfc3569bfd31c2df97a98c0cd892af97a9ef467da885f9006f16e3c66be"
bool(true)
string(64) "3446c43d86dd49b9dbde63f1d273fdd619400526ff083fcdbb99cfe86deb15a7"
bool(true)
string(64) "54abea863b072bf4bba18f4ba79553946567c95aa4547ca381e3c577aac4280d"
bool(true)
string(64) "e04ae97194264531046ae3aa0b70a27344e96259987d9aacd441a32d487cdd16"
bool(true)
string(64) "04f6ddd02d14aab09146401b21d204e7ea9434068c24ae7984eb5bb5d01d212f"
bool(true)
string(64) "7e188fad10942fb6ed949847273ae38548711556c7866213795a749358a95af8"
bool(true)
string(64) "bc8213ac539f12ab96857cf61e0d44d0e3dcc848ed74395dfd81443c7a629ce5"
bool(true)
string(64) "36cb4793dba282195623d5be19affc33983ed43ded1e36f184ef60eef64200f0"
bool(true)
string(64) "963f1d9da517194d760e7e1856c4620fa0f0d0001580dcaa90971fef1ffcb7e4"
bool(true)
string(64) "aa7bb377e70643dc53d7aef8b29cad0ccb80b95e4f647275d78d8aec7878e85d"
bool(true)
string(64) "e271d2a36abf807b889a3f9a95dab30dc29a45579aa9c391235a19cf496905d1"
bool(true)
string(64) "8e1ea08546b1ab2abfb01326bface6a98b6de7f3e4ee51207b789fa11bbe4fbc"
bool(true)
string(64) "608709f36133a9c3d0f13e16953f8d5fd49f84f03a3b4c5972cf199da288465f"
bool(true)
string(64) "0ecf719bb564f18d03263b2a818d47a75a309879fd412d1ff3a1c9053da4e81f"
bool(true)
string(64) "334a205c26d680ce3f5b4c0f9fd8f7322c217a02be76e80abcd65e167f689ec0"
bool(true)
string(64) "bf3c3e67763b9879c9e9c08e112ef0f01b9adabf2593daa98ee11b7e01834c8e"
bool(true)
string(64) "d02ed48d0f68ada7a3a7690b53d3689461155ab031fc9cb1504930f3a613d7f3"
bool(true)
string(64) "3108b7f1a43c8580c3035ade538bb3906aed08972352580413cf6154f0345451"
bool(true)
string(64) "f148a1028268dbb2528ef38e7f21c19874f4dc2e1014824fa160da167639b7f9"
bool(true)
string(64) "0468b1ba13c9908499dc6fc8eec9858e02b878290d998067d5a096c8513ff6c5"
bool(true)
string(64) "89eda3a0a7e83245c0ba83581a98749bc39f7fe88b77d80e785829971732e3e0"
bool(true)
string(64) "3f76e7191949294ab96e2bc30802195dba8342b26b2a49de6268388c003a6782"
bool(true)
string(64) "af8f5eb5637928a9209c9ec638bf263ca1e8059f37f2609a4be7a7af00501d59"
bool(true)
string(64) "b1dac2a255997c10d4a7f1ddac3a3c75ce1f7bb1e362f4ee8a6406d3389a2d5c"
bool(true)
string(64) "3c19abae18cb5d39d80b8355451f85c59e70ed3110437fbebdbf6c832c5348ee"
bool(true)
string(64) "41b87bc4e04061222030cc8645dc7333b16690e794319b0ec73c6021c6508c7c"
bool(true)
string(64) "75fca5b7ff6a6db41c6d7adfee21a66db2c7000f9cbf82d015efe149dea12856"
bool(true)
string(64) "fc20d6ecf5423ad715593d1ec739244d34f6734d866475fd45e78af873ae305e"
bool(true)
string(64) "55872635fea76b3371816be9b83c925ee31e5d6423834f58bbafb749b1c7140a"
bool(true)
string(64) "36d1ffaa31ea9c4f00e71be3ad2876ae04fdba53ecaead4d8e72e0e01030e35c"
bool(true)
string(64) "9f1b822fb12716f3a4172b6c1d4fcd339e4d4440415fb962a596115be68cbab0"
bool(true)
string(64) "cbb44e5a2d696b0db66827b42ff9114cd3f31110ca55217aee57a7651e2feaad"
bool(true)
string(64) "7ec846de0f2a4a968e6727ffb61bbf76ca22c1f7e093a3f563a3c71ded79be4b"
bool(true)
string(64) "bad49b9c1739779a07b5270e4e726c6c59c65111b12acbc77c64c7f67132b788"
bool(true)
string(64) "0239a98aee5b25baa651105e266c414fe56eaf850cf3fccd2c1bc20bd22f7421"
bool(true)
string(64) "414073d7761c6c8a6f16d7a4cd9d2a2bce6bfb678cb59ad14388cff0f83ba322"
bool(true)
string(64) "ec5d7f03167de2662d00405cb64f6ec5dfed38a1787541b523f75a0758e2f19e"
bool(true)
string(64) "bb529bc45bfabbd263a57a57a09eb24962d7d998909bd68b0354ec260ab7f293"
bool(true)
string(64) "531c5fa18c2c15542859ac334a967c6f99bdedc7b52103c3fae5d44726455866"
bool(true)
string(64) "2ed39e7dba79a76ada5f5d686c7272198b33ac8702967d0a3e6f2ec238d2efc9"
bool(true)
string(64) "dcd72d2b9e95ee130ad604edda3ca68f3940b460b5d9716afe81d86c6dfc89d7"
bool(true)
string(64) "b9949c1e16cfba79863fc381084fe1f0ea32b257d40806d96cd55d60d381221b"
bool(true)
string(64) "5bfc41e9b5ec708a9315772addc2fd7b3834603da4b11423648227b39a3895b4"
bool(true)
string(64) "6804fff9c96c2638c15c0dc2a79e7089c1c9e4a0f66ed78c7e0ce8a83cfa5226"
bool(true)
string(64) "ee1d25033817c253eac82e72c3ad8596ebdb7bbe8af5aa0c9b0b88b07075e622"
bool(true)
string(64) "93430ecbd765e6cea04564dd861096a5558ea657a0e24a236adc4d15537a8a86"
bool(true)
string(64) "23090417243e3432490972cbee5e0faf45b386195b642093157e14381a829176"
bool(true)
string(64) "5afa3d861e0544c46cb5d30278111558d07675eb5b6861f53bf586bb04e3943f"
bool(true)
string(64) "72cbc1ba2fccf37dff8e2228b19d33668783d516bd3277c592cdd329efd09b2e"
bool(true)
string(64) "1d565f50af7b5847a5d4fbaa663db98ae604c11df588f1704f12aee2fae36482"
bool(true)
string(64) "d513992cb98df28e8ffc2a242666c4be13840457f616b43f753ff8b8aab98c9a"
bool(true)
string(64) "e5e397ab3d95f453ce979a507224d4fe553b96f8b9f3e2a96fcacf5a22679fc1"
bool(true)
string(64) "d11bca059e6e47efaebd4c1569d5abe2ee55c23ee85b05274578fa009e853b4a"
bool(true)
string(64) "f2fce4d5949a1d9fe01c18d9207c161e5c945eec9093d934aeff103201117106"
bool(true)
string(64) "6f85148c552d40aa04f8661a888b8f42f0b79f1cbc9f5b3b7f527121f6b2adde"
bool(true)
string(64) "ff461a6f1e9a16e8e24fd928a3c6cf4fffe62870ddd83fece624ad40c3074655"
bool(true)
string(64) "de723655cfd5b0d8e278b795abdfb8c35d3eb4ae096144d16242f46b480de31a"
bool(true)
string(64) "277f21892d97765904fa98d005b708d90f100be54da6be5a96783eb2293393d3"
bool(true)
string(64) "27331c6895db78da108445dc4cba88a9d63cf4e06fec5280f856fec96d3126b1"
bool(true)
string(64) "efa4650b7e8ad1a756bfb0be24fdf44580ef7051172367295edce3ab16f5e891"
bool(true)
string(64) "ee6c399f0838ea455dcac60ac85e31a9c32399c4ae8a2718ff2d442930601cc9"
bool(true)
string(64) "944a11747a029904dd2e926f7c1e81bd96e1de9882e4f61cfdc3a13cd4076fb3"
bool(true)
string(64) "a207e28a6474ce4bfea8ea0d3c7f2fb5ca086b40f9ee8538a14912826d0989b2"
bool(true)
string(64) "4a3d6bbf9031c8ae4bd43b88eedbc3558928d356e156efa94b03fbabcd5d63fe"
bool(true)
string(64) "21f36e3d0e67c4dc3740f1448d0fb1f4fe21297ea584a18cb3e789ca682065fa"
bool(true)
string(64) "380efa66d568eaadd72690c2e6910556fda81802bc7fdacea80e4e58a01255ed"
bool(true)
string(64) "676afc66d0ec7fbb4a05f0b65a3073821b8c45f626030e0324df884eb67bcf79"
bool(true)
string(64) "1f153d379ba354a868dbb4ba327ab9ac020b5f9e0d84de24a658fdce78092e52"
bool(true)
string(64) "79e6f5fb8e97acd5f97e5a9f84e60ffdb380673bc968584b27576ac6f2ddc1cd"
bool(true)
string(64) "8b19f896e28f3c43134de42ef043e35927840ba2d1ccadb793cca78a16190ef6"
bool(true)
string(64) "ddbe711dbdf6e051e1ccca663cb7ed12d2abdcc877d53d1784d4cfbad178f564"
bool(true)
string(64) "cc875cc9c4e35d262dd7c467456748a67fe3894dc509765ce4104d1f85cba33f"
bool(true)
string(64) "d631a10b3b5985e559dc760a0e3132381c08ea62b3ee69c9ad33040d82c2a451"
bool(true)
string(64) "bc70e41cdf73adcf85d1f8e436122e5c339375f999609ea1d67c9aa4acbb2484"
bool(true)
string(64) "229de890a2b533529b27c77e197d726cb7ab4a8d92b3162e2ca5eaa8240e9250"
bool(true)
string(64) "21f1887054bf74bcac0edc79f2248c2b3cd57fbdeeea9ace28db20f8a3f2f8af"
bool(true)
string(64) "fd8ddb696da493908dd9bace25bd31d64ae79d4a42914c23275bbb0aefaff137"
bool(true)
string(64) "5484647d6f5143412f352797dcb710e16a365ed40e3bc8bb0b90c04efb1ecacb"
bool(true)
string(64) "2be2df6e24e16789b50c56af5c51d79727421ac6d0b7f3367d1e8c1e416fb25d"
bool(true)
string(64) "75ea7ffb134feffe1503a60f4e5066eaef59aecc4762f98ff0af160b58099ce1"
bool(true)
string(64) "d3cd5a9daf010a1a611f45472051409d72cb8e8bb9e6f20c67d990e1fa9186c0"
bool(true)
string(64) "a43efec105fb0785c72e64dd6514b53307c855b948efe2d707b8874d6d0ced98"
bool(true)
string(64) "1d580b27c9e20c10b3ed451395f4301f8c9443f8ab90150a2ce5b2988af17f4f"
bool(true)
string(64) "2a375688c595985c55371f99e690a565d823231bb8e66632362e132a5a5169ac"
bool(true)
string(64) "d7c5bf390482bb28a3f7d8e7d12ec3d6fa94441a894ebd64aefebf1dfa081aad"
bool(true)
string(64) "7e1e0994d36882d1e408ec87138807c670f5e633dbab1bd1bbe75480b2030746"
bool(true)
string(64) "a9f8a1efaec0cd8cc101eeb1b97b111cffb62e9f004cac6a44641444c0a650df"
bool(true)
string(64) "aef8e78d5f3db337a469bdada200848a8300f53a3a4667f5e89b0c5195c96934"
bool(true)
string(64) "b47c3144d82975ea9f6be60858de647ad8bd8d143807325efb63df490a26fc4d"
bool(true)
string(64) "adfb34f7a5dd6f33572cd7911e7466484becfae9fa637b45d3eb2471a71702bf"
bool(true)
string(64) "75b4dbd44e342128335998a971768b825e411e19b12cbce001dfb93f2cb22468"
bool(true)
string(64) "f6a30d933fda2e499e7adabfe79e2f6c905fe475a18da5b02454e29821e4498f"
bool(true)
string(64) "390ada924d8e3009d048c769faad4a6efa0a790fedef3d9a37237195e2afa2c9"
bool(true)
string(64) "6b77dff0e6c1916c08b9eddf87abd998380a1470fd48a2dd135d73166bc624ef"
bool(true)
string(64) "a678dbf517c67e82b5f27de41e7bf1b35eb14b68310408740159acf2c523b4b7"
bool(true)
string(64) "c3037f56b3425c61c6efc452f2f6a21532f6cdeebb3638b1013d9bf66b66dab0"
bool(true)
string(64) "09e3ca822b84d510fcf579e4d62187c9452a2dd68fb216dedf0fc10f1b0d4121"
bool(true)
string(64) "4f9d41ec341ca5cfd36c2bffe25834cbb2f5080508620e05d65768186ae0b91a"
bool(true)
string(64) "2ee94c569e2806b15807a9ebc9eb337897be3a27b90ff14614cf8db5135027a3"
bool(true)
string(64) "30a9a03ddc55cd3d80054f6e25baea74c292b85f05336a870154ce09325533df"
bool(true)
string(64) "430ac75a46e8f6451a6a08cf5fc9eb8ef7b29ff6195b25505db209b2d338dd3c"
bool(true)
string(64) "7cc466dbf4a5f57cc2bed2ea8bf5af83c421bda56f7282896a06a70a1a9c7d19"
bool(true)
string(64) "5d98b83610eb9ebc82aacf1c7a0a7dc81843db4811a676bac57cec07785cb8f0"
bool(true)
string(64) "2066b69dbda8c9e7ad7e32b2d62a73396ddbd4761fca40076178ec013d798f24"
bool(true)
string(64) "bb17e359a999f787f0e5afa0f5b2fc0de227ffa63cee2aac91829bef0a2b0423"
bool(true)
string(64) "3f8d3d104ca916b0137ce4ebbed07bd9732d441adeebcae707e8eaf5dedc6e0e"
bool(true)
string(64) "0014f61968a636700bff556a8b96c1396a748786432d3c412cd6d2494410df2a"
bool(true)
string(64) "f6868d84a89c87c8fcc84c33f0a5f9fc6ab10e15a277138e80f14c6463d14db8"
bool(true)
string(64) "d05f7b46c5b39e2029715bda935b57f8664b82005b0c5503622b1fc4b8097755"
bool(true)
string(64) "ce172e9012d52ba5fc860ca6b3c976cff4c373b751b29d8ee8c35e0c2bf5c993"
bool(true)
string(64) "6d67406517dc8eaf298033d35166e183516c3cfbd1ba9b9ca74cd90093f77201"
bool(true)
string(64) "3ca9a8b30cc26bdfbeafe1e3a80e302a942c53579c4aeb0bfcf4431f0b3d4cb2"
bool(true)
string(64) "8bee382d979bb9d7cb403bbad711ae7748fe0dacf87d07214c624e2ef5c09dc0"
bool(true)
string(64) "673a18a6a2fa3db0c75527da418c8efe4db678bcd41722a296a18c65049a6653"
bool(true)
string(64) "ae65e66335bfceaed2a6dca40c97b969fdad129046209ef3982b8101e2762bfd"
bool(true)
string(64) "2ebfbc853c8b34d50bd27af13482bf154a5d1c961abe6c77124c309afab511be"
bool(true)
string(64) "1f066a9547d1963a5d0dcaaa8a3c9fda60e47474e9bb95569f5d82ac4a3a122d"
bool(true)