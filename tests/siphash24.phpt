--TEST--
Siphash-2-4 test
--FILE--
<?php
$testVectors = [
        [
            'input_len' => 0,
            'output' => '310e0edd47db6f72',
        ],
        [
            'input_len' => 1,
            'output' => 'fd67dc93c539f874',
        ],
        [
            'input_len' => 2,
            'output' => '5a4fa9d909806c0d',
        ],
        [
            'input_len' => 3,
            'output' => '2d7efbd796666785',
        ],
        [
            'input_len' => 4,
            'output' => 'b7877127e09427cf',
        ],
        [
            'input_len' => 5,
            'output' => '8da699cd64557618',
        ],
        [
            'input_len' => 6,
            'output' => 'cee3fe586e46c9cb',
        ],
        [
            'input_len' => 7,
            'output' => '37d1018bf50002ab',
        ],
        [
            'input_len' => 8,
            'output' => '6224939a79f5f593',
        ],
        [
            'input_len' => 9,
            'output' => 'b0e4a90bdf82009e',
        ],
        [
            'input_len' => 10,
            'output' => 'f3b9dd94c5bb5d7a',
        ],
        [
            'input_len' => 11,
            'output' => 'a7ad6b22462fb3f4',
        ],
        [
            'input_len' => 12,
            'output' => 'fbe50e86bc8f1e75',
        ],
        [
            'input_len' => 13,
            'output' => '903d84c02756ea14',
        ],
        [
            'input_len' => 14,
            'output' => 'eef27a8e90ca23f7',
        ],
        [
            'input_len' => 15,
            'output' => 'e545be4961ca29a1',
        ],
        [
            'input_len' => 16,
            'output' => 'db9bc2577fcc2a3f',
        ],
    ];

    $key = '';
    for ($i = 0; $i < 16; $i++) {
        $key .= \chr($i);
    }
    $hash = new \FlexId\Siphash($key);

    $input = '';
    $lastInputLen = 0;
    foreach ($testVectors as $vector) {
        for ($i = $lastInputLen; $i < $vector['input_len']; $i++) {
            $input .= \chr($i);
        }
        $lastInputLen = $vector['input_len'];
        var_dump($hash->hash($input));
    }

    // test message/number interoperability
    $h1 = $hash->hash(\pack('Q', PHP_INT_MAX), null);
    $h2 = $hash->hash('', PHP_INT_MAX);
    var_dump($h1 === $h2);
    var_dump($h1);

    $h1 = $hash->hash(\pack('Q', 0), null);
    $h2 = $hash->hash('', 0);
    var_dump($h1 === $h2);
    var_dump($h1);

    for ($i = 1; $i < 100000; $i += 10000) {
        $h1 = $hash->hash(\pack('Q', $i) . 'some test', null);
        $h2 = $hash->hash('some test', $i);
        var_dump($h1 === $h2);
        var_dump($h1);
    }
?>
--EXPECT--
int(8246050544436514353)
int(8428550223375919101)
int(967288799772626778)
int(-8833979346009227731)
int(-3519680893311416393)
int(1762690195596617357)
int(-3762398573937892402)
int(-6124331488671313609)
int(-7785046478206851998)
int(-7061500321231936336)
int(8817410102741809651)
int(-814255129352229465)
int(8439340791604635131)
int(1507111754042457488)
int(-638443999941954834)
int(-6833708440360172059)
int(4551675220716592091)
bool(true)
int(-4023475788996417830)
bool(true)
int(4166820438245540263)
bool(true)
int(8207428085505679980)
bool(true)
int(6867863868948951468)
bool(true)
int(5494865773126797280)
bool(true)
int(7633235074938398847)
bool(true)
int(1295027883651701278)
bool(true)
int(-2055549646470348944)
bool(true)
int(6226172660203518415)
bool(true)
int(8800776081893801528)
bool(true)
int(2644140069123804843)
bool(true)
int(6047892395969365948)