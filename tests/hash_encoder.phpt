--TEST--
Hash encoder test
--FILE--
<?php
        $alphabet = 'R9jHJSAcLBfFroKqTzXvgxtNbsp83DEkVMIn-y0m5hdG_P47awQl26uYZUeW1iCO';

        for ($i = 33; $i < \strlen($alphabet); $i++) {
            $encodedIds = [];
            $expected = 0;
            $encoder = new \FlexId\HashEncoder(\substr($alphabet, 0, $i));
            $incrementalHash = hash_init('sha256');
            for ($j = 1; $j < 70000; $j++) {
                $encoded = $encoder->encode($j);
                $encodedIds[$encoded] = 1;
                $expected++;
                hash_update($incrementalHash, $encoded);
                //var_dump($i, $encoded);
                if ($j !== $encoder->decode($encoded)) {
                    var_dump("failed to decode $j");
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
            var_dump(hash_final($incrementalHash));
            var_dump(count($encodedIds) === $expected);
        }
?>
--EXPECT--
string(64) "be2e97156d5662ba54f4d9145f7591c39f093256e72379ef6b2a2f480723e59b"
bool(true)
string(64) "44430b04d1c82d0893591e681f053b1ca243b14b0a71b1c489ca174da688fbb4"
bool(true)
string(64) "5405ac448702f2ab34e9ed153cc193e89c83a1d8ed99fae122361a768acd74f1"
bool(true)
string(64) "f6099293a0919699cd3511e3c41cc9e7228bddae46251b34be73cd492d57051f"
bool(true)
string(64) "70744d9886fe57716023a52e2f1ddcf6161b294a7318b7131f78854302cc6f8a"
bool(true)
string(64) "ebda3dc83856068a4313ae62ccd58823a725741b1b4b502541e1da4891d0fde2"
bool(true)
string(64) "124399f660fa24a12a83bbcbc0443b7438033a49532607fafefb02a575c0294d"
bool(true)
string(64) "11d912dd8046e112fff6b5c6e544f89a81a07411779b77484cc3adbf2b94f4b3"
bool(true)
string(64) "36aa00cefbcdc12746c30a95e1844cf1ae297cb17ca70053d9a4ccce59be024a"
bool(true)
string(64) "1bb1695306cbc9b63a2c84563d51a0e633730e4e703a87504076597cbc644681"
bool(true)
string(64) "e7f9ebfc5ea600d80b154fcb3bc77797dc053aeabb37afa1e0d3667bf0959ed8"
bool(true)
string(64) "2b11823293ba46e8722f5085486eb32cc7ceed078d4ea6b04e75f542e0d986cc"
bool(true)
string(64) "d4a43657e7b1d1fb1cee0d6b5b6f54db2915cfcf2b00271ab308d8971c1c3179"
bool(true)
string(64) "905122a0a06049d642e98b94a7017e15dea38fa1af87ea03627401a392f5aedd"
bool(true)
string(64) "1d36528463f687bd6a6e3131ce5172e66015930717c1b8d97a81a640be0c7f0c"
bool(true)
string(64) "92366d413b9c347ebf3b6dd8b5c04a39a2143ba1a7f962e950ee155c97bcf872"
bool(true)
string(64) "c6307220f28d40f107f6b36286437ac554fd940128dd2e543ce84b2d23106ab4"
bool(true)
string(64) "28dc91323f2fabe747c5665d018e0366e18747b9db6de076f99b9eb48872791a"
bool(true)
string(64) "6dcbfff01b5ea091bf64b31db1a6dc4e7e5496ce3096599e6ede878767eb8433"
bool(true)
string(64) "1824cc8eaa51b7f62b56c2cd3015ef1e9f833feaa05d6ad3e2a88ceb85d2df3c"
bool(true)
string(64) "682a799e806e3dae0832766a5544639157c63fad84fe40f41503d0f6230b24ab"
bool(true)
string(64) "4c08e5d741585462d30203ea17af74ce2b8a8d5e74a3cd225d86d957b3ad126a"
bool(true)
string(64) "b37992c5eb3b600e0f14a45b48a702049897c1bc7d9b26581a009454d0f6054b"
bool(true)
string(64) "638433ccb1f27eaee27aeff9b38db0e090003c6de385009839de683dc96c8f45"
bool(true)
string(64) "9292351c880709d5667a4c1984ce14fa01a544342dca5ec483d0bdc08fff70bb"
bool(true)
string(64) "957c9a861995102d4b5e3c73454b5b1052d5b4e89c0df69955e0365b9f0c66e2"
bool(true)
string(64) "d5a2a1cd620e40ece89f57f52245fa658cc01daa4ca7fc8187347d5b36aaf0c9"
bool(true)
string(64) "d019e9f4a19da93de67ce21cbac2f17cec1d78edbb56dbee2a1c9fe3a9f5c7c9"
bool(true)
string(64) "fc97b74f881530cd157bcf897036ad2cf09b2baf55fbde237e6a3f1ad033ebbd"
bool(true)
string(64) "f5484aea89032201015376903dee3d24bf566b1fcb85e159dc58062eaf5da1c9"
bool(true)
string(64) "8fb66040fdaacd03b32e61eab1b91b9b72e56235bb57b038c06dcc7f1fff47a3"
bool(true)
