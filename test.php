<?php
#########################################################################################################################
#                                                                                                                       #
#                                   Second part of project for IPP course - test                                        #
#                                                                                                                       #
#                                   Author: Marek Varga                                                                 #
#                                   Login: xvarga14                                                                     #
#                                                                                                                       #
#########################################################################################################################

# define global variables
$recursive = 0;                     # recursion desired
$path = getcwd();                   # path to tests
$parseFile = "compiler";           # parse
$intFile = "interpret.py";          # interpret
$src = array();                     # array of .src files
$in = array();                      # array of .in files
$out = array();                     # array of .out files
$rc = array();                      # array of .rc files
$tmpFiles1 = array();               # array of temporary files created during testing - contains output files form parse not required for interpret
$tmpFiles2 = array();               # array of temporary files created during testing - contains output files from parse required for interpret
$html = new DOMDocument('1.0', 'UFT-8');                # DOMDocument object for output HTML
$passedParse = 0;                   # value to keep track of passed tests for parse
$failedParse = 0;                   # value to keep track of failed tests for parse
$passedInterpret = 0;               # value to keep track of passed tests for interpret
$failedInterpret = 0;               # value to keep track of failed tests for interpret

# set up html
$root = initializeHTML();

# check input arguments
handleArgs();

# load tests
loadTests($path);

# put .src files through parse
$body = testParse();

# put output files from parse through interpret
#testInterpret();

# end editing html
saveHTML();

# delete .tmp files
deleteTmpFiles();

/**
 * Function parses arguments, check if they are in right amount and if they meet required form.
 */
function handleArgs()
{
    global $recursive;
    global $path;
    global $parseFile;
    global $intFile;

    $shortOpts = "";
    $longOpts = array(
        "help",
        "directory:",
        "recursive",
        "parse-script:",
        "int-script:"
    );

    # get arguments
    $options = getopt($shortOpts, $longOpts);

    foreach ($options as $key=>$value) {
        if ($key === "help" && count($options) == 1) {
            echo "This is test script for parse.php and interpret.py scripts.";
            echo "Run by php test.php [--directory=pat] [--recursive] [--parse-script=file] [--in-script=file]";
            exit(0);
        } elseif ($key === "recursive") {
            $recursive = 1;
        } elseif ($key === "directory") {
            $path = $value;
        } elseif ($key === "parse-script") {
            $parseFile = $value;
        } elseif ($key === "int-script") {
            $intFile = $value;
        } else {
            # undefined arguments are present
            errorMsg(10);
        }
    }
}

/**
 * Function loads tests from desired directory.
 *
 * @param $dirPath string path to searched dir
 */
function loadTests($dirPath)
{
    global $recursive;
    global $src;
    global $in;
    global $out;
    global $rc;

    # get content of directory
    $files = scandir($dirPath);

    foreach ($files as $file) {
        if ($file === "." || $file === "..") {
            continue;
        }
        if ($recursive === 1 && is_dir($dirPath.'/'.$file) ) {
            loadTests($dirPath.'/'.$file);
        }
        if (preg_match('/^[\p{L}*[0-9]*]*\.src$/u',$file) == 1) {
            $src[] = $dirPath.'/'.$file;
        }
        if (preg_match('/^[\p{L}*[0-9]*]*\.in$/u',$file) == 1) {
            $in[] = $dirPath.'/'.$file;
        }
        if (preg_match('/^[\p{L}*[0-9]*]*\.out$/u',$file) == 1) {
            $out[] = $dirPath.'/'.$file;
        }
        if (preg_match('/^[\p{L}*[0-9]*]*\.rc$/u',$file) == 1) {
            $rc[] = $dirPath.'/'.$file;
        }
    }
}

/**
 * Function for putting .src files through parse
 */
function testParse()
{
    global $tmpFiles1;
    global $tmpFiles2;
    global $parseFile;
    global $src;
    global $rc;
    global $html;
    global $failedParse;
    global $passedParse;

    # create body of html
    $body = $html->createElement("body");
    $h = $html->createElement("h1", "This is script for testing parse.php and interpret.py scripts");
    $body->appendChild($h);
    $p = $html->createElement("p", "Testing parse.php");
    $body->appendChild($p);

    # find path to .rc files
    $tmp = explode("/", explode(".rc", $rc[0], -1)[0]);
    $rcPath = explode($tmp[count($tmp)-1], $rc[0]);

    # create table
    $table = $html->createElement("table");
    $tableAttribute = $html->createAttribute("style");
    $tableAttribute->value = "width:50%";
    $table->appendChild($tableAttribute);
    $tr = $html->createElement("tr");
    $th = $html->createElement("th", "Test name");
    $tr->appendChild($th);
    $th = $html->createElement("th", "Test result");
    $tr->appendChild($th);
    $th = $html->createElement("th", "RC");
    $tr->appendChild($th);
    $th = $html->createElement("th", "Expected RC");
    $tr->appendChild($th);
    $table->appendChild($tr);
    $body->appendChild($table);

    foreach ($src as $source) {
        $sourceName = explode(".src", $source, -1);
        exec("./$parseFile < $source > $sourceName[0].tmp 2> /dev/null",$output, $returnCode);

        # find test name
        $testName = explode("/", $sourceName[0]);

        # create .rc file if it does not exist
        $rcFile = getRC($testName[count($testName)-1]);
        if ( $rcFile === null) {
            file_put_contents($rcPath[0].$testName[count($testName)-1].'.rc', 0);
            $rc[] = $rcPath[0].$testName[count($testName)-1].'.rc';
        }

        # check return code value
        if ($returnCode >= 0 && $returnCode < 32) {
            $tr = $html->createElement("tr");
            $td = $html->createElement("td", $testName[count($testName)-1]);
            $tr->appendChild($td);

            $tmpRC = trim(file_get_contents($rcFile));
            if (intval($tmpRC) !== $returnCode) {
                $td = failedTableData();
                $failedParse++;
            } else {
                $td = successTableData();
                $passedParse++;
            }
            $tr->appendChild($td);
            $td = $html->createElement("td", $returnCode);
            $tr->appendChild($td);
            $td = $html->createElement("td", $tmpRC);
            $tr->appendChild($td);
            $tmpFiles1[] = $sourceName[0].'.tmp';
        } else {
            $tmpFiles2[] = $sourceName[0].'.tmp';
        }

        $table->appendChild($tr);
    }
    $body->appendChild($table);

    return $body;
}

/**
 * Function for putting files created by parse through interpret
 */
function testInterpret()
{
    global $tmpFiles1;
    global $tmpFiles2;
    global $intFile;
    global $in;
    global $out;
    global $html;
    global $body;
    global $failedInterpret;
    global $passedInterpret;

    # find path to .in files
    $tmp = explode("/", explode(".in", $in[0], -1)[0]);
    $inPath = explode($tmp[count($tmp)-1], $in[0]);

    # find path to .out files
    $tmp = explode("/", explode(".out", $out[0], -1)[0]);
    $outPath = explode($tmp[count($tmp)-1], $out[0]);

    $p = $html->createElement("p", "Testing interpret.py");
    $body->appendChild($p);
    $br = $html->createElement("br");
    $body->appendChild($br);

    # create table
    $table = $html->createElement("table");
    $tableAttribute = $html->createAttribute("style");
    $tableAttribute->value = "width:70%";
    $table->appendChild($tableAttribute);
    $tr = $html->createElement("tr");
    $th = $html->createElement("th", "Test name");
    $tr->appendChild($th);
    $th = $html->createElement("th", "Return code");
    $tr->appendChild($th);
    $th = $html->createElement("th", "Diff");
    $tr->appendChild($th);
    $th = $html->createElement("th", "RC");
    $tr->appendChild($th);
    $th = $html->createElement("th", "Expected RC");
    $tr->appendChild($th);
    $table->appendChild($tr);
    $body->appendChild($table);

    foreach ($tmpFiles2 as $source) {
        $sourceName = explode(".tmp", $source, -1);

        # find name of input file
        $testName = explode("/", $sourceName[0]);
        $inFile = getIN($testName[count($testName)-1]);
        if ( $inFile === null) {
            file_put_contents($inPath[0].$testName[count($testName)-1].'.in', "");
            $inFile = $inPath[0].$testName[count($testName)-1].'.in';
        }

        # run test
        exec("python3.6 $intFile --source=$source < $inFile > $sourceName[0].int 2> /dev/null",$output, $returnCode);

        # get requested return code from .rc file
        $rcFile = getRC($testName[count($testName)-1]);
        $tmpRC = trim(file_get_contents($rcFile));

        # compare return codes
        $tr = $html->createElement("tr");
        $td = $html->createElement("td", $testName[count($testName)-1]);
        $tr->appendChild($td);
        if (intval($tmpRC) !== $returnCode) {
            $td = failedTableData();
            $failedInterpret++;
        } else {
            $td = successTableData();
            $passedInterpret++;
        }
        $tr->appendChild($td);

        # get name of .out file
        $outFile = getOUT($testName[count($testName)-1]);
        if ( $outFile === null) {
            $outFile = $outPath[0].$testName[count($testName)-1].'.out';
        }

        # compare different in output files
        exec("diff -N $sourceName[0].int $outFile", $output, $returnCode2);
        if ($returnCode2 == 0) {
            $td = successTableData();
            $passedInterpret++;
        } else {
            $td = failedTableData();
            $failedInterpret++;
        }

        $tr->appendChild($td);
        $td = $html->createElement("td", $returnCode);
        $tr->appendChild($td);
        $td = $html->createElement("td", $tmpRC);
        $tr->appendChild($td);

        # add table row to table
        $table->appendChild($tr);

        # add created files to array for deleting
        $tmpFiles1[] = $sourceName[0].'.int';
    }

    $body->appendChild($table);

}

/**
 * Function looks for .rc file and returns given file or null.
 *
 * @param $name string name of .rc to look for
 *
 * @return null|string name of .rc file + path
 */
function getRC($name)
{
    global $rc;

    foreach ($rc as $item) {
        if (strpos($item, $name.'.rc')){
            return $item;
        }
    }

    return null;
}

/**
 * Function looks for .in file and returns given file or null.
 *
 * @param $name string name of .in to look for
 *
 * @return null|string name of .int file + path
 */
function getIN($name)
{
    global $in;

    foreach ($in as $item) {
        if (strpos($item, $name.'.in')){
            return $item;
        }
    }

    return null;
}

/**
 * Function looks for .out file and returns given file or null.
 *
 * @param $name string name of .out to look for
 *
 * @return null|string name of .out file + path
 */
function getOUT($name)
{
    global $out;

    foreach ($out as $item) {
        if (strpos($item, $name.'.out')){
            return $item;
        }
    }

    return null;
}

/**
 * Function initializes HTML
 *
 * @return DOMElement
 */
function initializeHTML()
{
    global $html;

    $html->preserveWhiteSpace = false;
    $html->formatOutput = true;
    $root = $html->createElement("html");
    $head = $html->createElement("head");
    $title = $html->createElement("title", "Test");
    $head->appendChild($title);
    $style = $html->createElement("style", "table, th, td{border: 1px solid black; border-collapse:collapse;}");
    $head->appendChild($style);
    $align = $html->createElement("style", "td {padding: 5px;}");
    $head->appendChild($align);
    $root->appendChild($head);

    return $root;
}

/**
 * Function creates <td> tag with data for table
 *
 * @return DOMElement <td> tag with data for table
 */
function successTableData()
{
    global $html;

    $td = $html->createElement("td","OK");
    $tdStyle = $html->createAttribute("style");
    $tdStyle->value = "background-color: #00ff00";
    $td->appendChild($tdStyle);

    return $td;
}

/**
 * Function creates <td> tag with data for table
 *
 * @return DOMElement <td> tag with data for table
 */
function failedTableData()
{
    global $html;

    $td = $html->createElement("td","FAILED");
    $tdStyle = $html->createAttribute("style");
    $tdStyle->value = "background-color: #ff6600";
    $td->appendChild($tdStyle);

    return $td;
}

/**
 * Function saves and outputs HTML
 */
function saveHTML()
{
    global $html;
    global $root;
    global $body;

    # create stat of passed tests
    createStats();

    $root->appendChild($body);
    $html->appendChild($root);
    echo $html->saveHTML();

}

/**
 * Function creates list of passed/failed tests
 */
function createStats()
{
    global $html;
    global $body;
    global $passedParse;
    global $failedParse;
    global $passedInterpret;
    global $failedInterpret;

    $ul = $html->createElement("ul");
    $ulStyle = $html->createAttribute("style");
    $ulStyle->value = "list-style-type:square";
    $ul->appendChild($ulStyle);
    $testsNum = $passedParse+$failedParse+$passedInterpret+$failedInterpret;
    $li = $html->createElement("li", "Executed tests = $testsNum");
    $ul->appendChild($li);
    $passed = $passedParse + $passedInterpret;
    $li = $html->createElement("li", "Passed tests = $passed");
    $ul->appendChild($li);
    $failed = $failedInterpret + $failedParse;
    $li = $html->createElement("li", "Failed tests = $failed");
    $ul->appendChild($li);
    $parseTests = $passedParse + $failedParse;
    $li = $html->createElement("li", "Tests for parse = $parseTests");
    $ul1 = $html->createElement("ul");
    $li1 = $html->createElement("li", "Passed = $passedParse");
    $ul1->appendChild($li1);
    $li1 = $html->createElement("li", "Failed = $failedParse");
    $ul1->appendChild($li1);
    $li->appendChild($ul1);
    $ul->appendChild($li);
    $interpretTests = $passedInterpret + $failedInterpret;
    $li = $html->createElement("li", "Tests for interpret = $interpretTests");
    $ul1 = $html->createElement("ul");
    $li1 = $html->createElement("li", "Passed = $passedInterpret");
    $ul1->appendChild($li1);
    $li1 = $html->createElement("li", "Failed = $failedInterpret");
    $ul1->appendChild($li1);
    $li->appendChild($ul1);
    $ul->appendChild($li);
    $body->appendChild($ul);
}

/**
 * Function deletes tmp files created during testing.
 */
function deleteTmpFiles()
{
    global $tmpFiles1;
    global $tmpFiles2;

    foreach ($tmpFiles1 as $tmpFile){
        exec("rm -f $tmpFile");
    }

    foreach ($tmpFiles2 as $tmpFile){
        exec("rm -f $tmpFile");
    }
}

/**
 * Function for handling error messages.
 *
 * @param $number int number of error message
 */
function errorMsg($number)
{
    switch ($number) {
        case 10:
            fwrite(STDERR, "Missing parameter or wrong number of parameters");
            exit(10);
            break;
    }
}

?>
